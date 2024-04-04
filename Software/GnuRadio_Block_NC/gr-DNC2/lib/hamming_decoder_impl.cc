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
#include "hamming_decoder_impl.h"

namespace gr
{
namespace DNC2
{

hamming_decoder::sptr
hamming_decoder::make(int m)
{
	return gnuradio::get_initial_sptr(new hamming_decoder_impl(m));
}

/*
     * The private constructor
     */
hamming_decoder_impl::hamming_decoder_impl(int m)
	: gr::block("hamming_decoder",
				gr::io_signature::make(1, 1, sizeof(char)),
				gr::io_signature::make(1, 1, sizeof(char)))
{
	N = pow(2, m) - 1;
	K = N - M;

	N = 7;
	K = 4;
	M = 3;
}

/*
     * Our virtual destructor.
     */
hamming_decoder_impl::~hamming_decoder_impl()
{
}

void hamming_decoder_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
	ninput_items_required[0] = noutput_items * 2;
}

int hamming_decoder_impl::general_work(int noutput_items,
									   gr_vector_int &ninput_items_v,
									   gr_vector_const_void_star &input_items,
									   gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0]; //std::cout<<ninput_items<<std::endl;

	const char *in = (const char *)input_items[0];
	char *out = (char *)output_items[0];

	int ni = 0;
	int no = 0;
	char c1 = 0;
	char c2 = 0;
	while (ni < ninput_items && no < noutput_items)
	{
		c1 = decode(in[ni++]); //std::cout<<ninput_items<<"___"<<noutput_items<<std::endl;
		c2 = decode(in[ni++]);
		out[no++] = c1 | (c2 << 4);
	}
	consume_each(ni);
	return no;
}
char hamming_decoder_impl::decode(char codeword)
{
	int m = 3;
	int n = pow(2, m) - 1;
	int k = n - m;
	char code[7];
	char msg[4];
	int parity[10];
	int syn = 0;
	int i = 0;
	int j = 0, l = 0;

	// Compute parity positions
	parity[1] = 1;
	for (i = 2; i <= m; i++)
		parity[i] = (parity[i - 1] << 1) & 0xfffffffe;
	//read codeword in reversed order
	for (i = 1; i <= n; i++)
		//info[i] = ( random() >> 10) & 0x01;
		code[i] = ((codeword >> (i - 1)) & 0x1);

	//Compute syndrome vector
	for (i = 1; i <= n; i++)
		if (code[i])
			syn ^= i;

	//printf("syndrome = %d\n", syn);

	// Correct error if needed
	if (syn)
		code[syn] ^= 1;

	//Compute message
	i = 1;
	l = 1;
	for (j = 1; j <= n; j++)
	{
		if (j == parity[l] && l <= m)
		{
			l++;
		}
		else
		{
			msg[i] = code[j];
			i++;
		}
	}

	int ni = 1;
	int d_counter = 0;
	char d_byte;
	while (ni <= 4)
	{
		d_byte |= ((msg[ni] & 0x1) << d_counter);
		d_counter++;
		ni++;
	}
	return d_byte;
	//printf("estimate = ");
	//for (j=1; j<=n; j++) printf("%1d", code[j]);
	//printf("\n");
}
} /* namespace DNC2 */
} /* namespace gr */
