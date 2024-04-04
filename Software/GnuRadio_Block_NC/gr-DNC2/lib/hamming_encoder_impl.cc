/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "hamming_encoder_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <time.h>
#include <sys/time.h>

namespace gr
{
namespace DNC2
{

hamming_encoder::sptr
hamming_encoder::make(int m, int image_size, int packet_size)
{
	return gnuradio::get_initial_sptr(new hamming_encoder_impl(m, image_size, packet_size));
}

/*
     * The private constructor
     */
hamming_encoder_impl::hamming_encoder_impl(int m, int image_size, int packet_size)
	: gr::block("hamming_encoder",
				gr::io_signature::make(1, 1, sizeof(char)),
				gr::io_signature::make(1, 1, sizeof(char))),
	  d_image_size(image_size), d_packet_size(packet_size), d_offset(0),
	  d_packet_index(0), d_ctrl(0), d_packet_no(0),
	  d_state(ST_PACKETIZE)
{
	//N = pow(2,m)-1;
	//K = N - M;

	N = 7;
	K = 4;
	M = 3;
}

hamming_encoder_impl::~hamming_encoder_impl()
{
}

void hamming_encoder_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
	ninput_items_required[0] = noutput_items / 2;
}

int hamming_encoder_impl::general_work(int noutput_items,
									   gr_vector_int &ninput_items_v,
									   gr_vector_const_void_star &input_items,
									   gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const char *in = (const char *)input_items[0];
	char *out = (char *)output_items[0];
	int ni = 0;
	int no = 0;
	int i = 0;
	char msg;

	while (no < noutput_items)
	{
		switch (d_state)
		{
		case ST_ENCODING:
		{
			if (ni < ninput_items)
			{
				if (d_ctrl == 0)
				{
					msg = (in[ni] & 0x0F);
					out[no++] = encode(msg);
					d_packet_index++;
					d_offset++;
					d_ctrl = 1;
					if (d_packet_index == d_packet_size)
					{
						d_packet_index = 0;
						d_state = ST_PACKETIZE;
					}
				}
				else
				{
					msg = ((in[ni] >> 4) & 0x0F);
					out[no++] = encode(msg);
					d_packet_index++;
					d_offset++;
					d_ctrl = 0;
					ni++;
					if (d_packet_index == d_packet_size)
					{
						d_packet_index = 0;
						d_state = ST_PACKETIZE;
					}
				}
			}
			else
			{
				consume_each(ni);
				return no;
			}
			break;
		}
		case ST_PACKETIZE:
		{
			if (d_offset < (2 * d_image_size))
			{
				d_packet_no++;
				out[no++] = 0x00;
				d_state = ST_ENCODING;
			}
			else
			{
				out[no++] = 0x01;
				d_state = ST_FREE;
			}
			break;
		}
		case ST_FREE:
		{
			out[no++] = 0x01;
			consume_each(ni);
			return no;
		}
		}
	}
	consume_each(ni);
	return no;
}
char hamming_encoder_impl::encode(char msg)
{
	int i, j, l, index;
	int n, k;
	char code[1024];
	char red[1024], info[1024];
	int m;
	int parity[10];
	int syn;
	int error;

	int test, result;

	m = 3;
	n = pow(2, m) - 1;
	k = n - m;

	// Compute parity positions
	parity[1] = 1;
	for (i = 2; i <= m; i++)
		parity[i] = (parity[i - 1] << 1) & 0xfffffffe;

	//printf("parity positions: ");
	//for (i=1; i<=m; i++) printf("%2d ", parity[i]); printf("\n");

	// Generatemessage
	for (i = 1; i <= k; i++)
		info[i] = ((msg >> (i - 1)) & 0x1);

	//printf("information bits = ");
	//for (j=1; j<=k; j++) printf("%1d", info[j]);
	//printf("\n");

	// Compute parity bits
	for (j = 1; j <= m; j++)
	{
		red[j] = 0;
		l = 0;
		for (i = 1; i <= n; i++)
		{
			// Check that "i" is not a parity position = not a power of 2
			result = 0;
			test = 1;
			for (index = 1; index <= m; index++)
			{
				if (i == test)
					result = 1;
				test *= 2;
			}
			if (!result)
			{
				l++;
				if ((i >> (j - 1)) & 0x01)
					red[j] ^= info[l];
			}
		}
	}

	//printf("parity bits = ");
	//for (j=1; j<=m; j++) printf("%1d", red[j]);
	//printf("\n");

	// Transmit codeword
	i = 1;
	l = 1;
	for (j = 1; j <= n; j++)
		if (j == parity[l] && l <= m)
		{
			code[j] = red[l];
			l++;
		}
		else
		{
			code[j] = info[i];
			i++;
		}

	int ni = 1;
	int d_counter = 0;
	char d_byte;
	while (ni <= 7)
	{
		d_byte |= ((code[ni] & 0x1) << d_counter);
		d_counter++;
		ni++;
	}

	return d_byte;
}

} /* namespace DNC2 */
} /* namespace gr */
