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
#include "convolutional_decoder_impl.h"
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

convolutional_decoder::sptr
convolutional_decoder::make(const std::vector<int> gen1,
							const std::vector<int> gen2)
{
	return gnuradio::get_initial_sptr(new convolutional_decoder_impl(gen1, gen2));
}

/*
     * The private constructor
     */
convolutional_decoder_impl::convolutional_decoder_impl(const std::vector<int> gen1,
													   const std::vector<int> gen2)
	: gr::block("convolutional_decoder",
				gr::io_signature::make(1, 1, sizeof(char)),
				gr::io_signature::make(1, 1, sizeof(char))),
	  d_gen1(gen1),
	  d_gen2(gen2),
	  generator1(0x7),
	  generator2(0x5)
{
}

/*
     * Our virtual destructor.
     */
convolutional_decoder_impl::~convolutional_decoder_impl()
{
}

void convolutional_decoder_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
	ninput_items_required[0] = noutput_items * 2;
}

int convolutional_decoder_impl::general_work(int noutput_items,
											 gr_vector_int &ninput_items_v,
											 gr_vector_const_void_star &input_items,
											 gr_vector_void_star &output_items)
{
	static bool done = false;
	if (done)
	{
		return -1;
	}
	const char *in = (const char *)input_items[0];
	char *out = (char *)output_items[0];
	int ninput_items = ninput_items_v[0];
	int outputs_[8];

	int ni = 0, no = 0;
	int **mapStateInput = initializeOutputs();
	int trellis_size = ninput_items/2;
	int trellis[trellis_size][4];
	int path_metrics[trellis_size + 1][4];
	path_metrics[0][0] = 0;
	path_metrics[0][1] = 0;
	path_metrics[0][2] = 0;
	path_metrics[0][3] = 0;
	while (ni < ninput_items)
	{
		int current_bits = in[ni] * 2 + in[ni + 1];
		printf("current bit: %d\n", current_bits);
		for (int i = 0; i < 4; i ++) {
			printf("current state: %d\n", i);
			int pre_state_1 = ((i << 1) | 1) & ((1 << 2) - 1);
			printf("pre state 1: %d\n", pre_state_1);
			int pre_state_2 = i << 1 & ((1 << 2) - 1);
			printf("pre state 1: %d\n", pre_state_2);
			int pm1 = branchMetric(current_bits, pre_state_1, (i >> 1) & 1, mapStateInput) + path_metrics[ni/2][pre_state_1];
			int pm2 = branchMetric(current_bits, pre_state_2, (i >> 1) & 1, mapStateInput) + path_metrics[ni/2][pre_state_2];
			printf("Pm1 = %d, pm2 = %d\n\n\n", pm1, pm2);
			if (pm1 < pm2) {
				path_metrics[ni/2 + 1][i] = pm1;
				trellis[ni/2][i] = pre_state_1;
			} else {
				path_metrics[ni/2 + 1][i] = pm2;
				trellis[ni/2][i] = pre_state_2;
			}
		}
		ni += 2;
		no ++;
	}

	int temp = path_metrics[6][0];
	int final_state = 0;
	for (int i = 0; i < 4; i ++) {
		if (temp > path_metrics[trellis_size][i]) {
			temp = path_metrics[trellis_size][i];
			final_state = i;
		}
	}

	printf("temp: %d, i: %d \n", temp, final_state);
	for (int i = ni/2 - 1; i >= 0; i --) {
		printf("i: %d, state = %d, trellis = %d\n", i, final_state, trellis[i][final_state]);
		final_state = trellis[i][final_state];
	}

	consume_each(ni);
	done = true;
	return no;
}

int **convolutional_decoder_impl::initializeOutputs()
{
	int **mapStateInput = new int *[4];
	for (int i = 0; i < 4; i++)
	{
		mapStateInput[i] = new int[2];
		for (int j = 0; j < 2; j++)
		{
			int xn2 = i & 1;
			int xn1 = (i >> 1) & 1;
			int codebit1 = (j * 1 + xn2 * 1 + xn1 * 1) % 2;
			int codebit2 = (j * 1 + xn2 * 0 + xn1 * 1) % 2;
			mapStateInput[i][j] = (codebit1 << 1) | codebit2;
		}
	}
	return mapStateInput;
}

int convolutional_decoder_impl::hammingDistance(int x, int y)
{
	int dist = 0;
	for (int i = 0; i < 2; ++i)
	{
		int bitX = (x >> i) & 1;
		int bitY = (y >> i) & 1;
		if (bitX != bitY)
		{
			++dist;
		}
	}

	return dist;
}

int convolutional_decoder_impl::branchMetric(int received_bit,
											 int current_state,
											 int input,
											 int **mapStateInput)
{
	int output = mapStateInput[current_state][input];
	return hammingDistance(received_bit, output);
}


std::string convolutional_decoder_impl::decode(const char *msg, int nin, int **mapStateInput)
{
	
}

} /* namespace DNC2 */
} /* namespace gr */
