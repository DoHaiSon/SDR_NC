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
	std::string a = decode(in, ninput_items, mapStateInput);
	std::cout << "test:" << a;
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
			int codebit1 = (0 * 1 + xn2 * 1 + xn1 * 1) % 2;
			int codebit2 = (0 * 1 + xn2 * 0 + xn1 * 1) % 2;
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

std::pair<int, int> convolutional_decoder_impl::pathMetric(
	int received_bit,
	const std::vector<int> &prev_path_metrics,
	int input, int **mapStateInput)
{
	int s = (input & ((1 << (3 - 2)) - 1)) << 1;
	int source_state1 = s | 0;
	int source_state2 = s | 1;

	int pm1 = prev_path_metrics[source_state1];
	if (pm1 < std::numeric_limits<int>::max())
	{
		pm1 += branchMetric(received_bit, source_state1, input, mapStateInput);
	}
	int pm2 = prev_path_metrics[source_state2];
	if (pm2 < std::numeric_limits<int>::max())
	{
		pm2 += branchMetric(received_bit, source_state2, input, mapStateInput);
	}

	if (pm1 <= pm2)
	{
		return std::make_pair(pm1, source_state1);
	}
	else
	{
		return std::make_pair(pm2, source_state2);
	}
}

typedef std::vector<std::vector<int> > Trellis;

void convolutional_decoder_impl::updatePathMetrics(int received_bit,
												   std::vector<int> *path_metrics,
												   Trellis * trellis,
												   int **mapStateInput)
{
	std::vector<int> new_path_metrics(path_metrics->size());
	std::vector<int> new_trellis_column(1 << (3 - 1));
	for (int i = 0; i < path_metrics->size(); i++)
	{
		std::pair<int, int> p = pathMetric(received_bit, *path_metrics, i, mapStateInput);
		new_path_metrics[i] = p.first;
		new_trellis_column[i] = p.second;
	}

	*path_metrics = new_path_metrics;
	trellis->push_back(new_trellis_column);
}

std::string convolutional_decoder_impl::decode(const char *msg, int nin, int **mapStateInput)
{
	// Compute path metrics and generate trellis.
	std::vector<std::vector<int> > trellis;
	std::vector<int> path_metrics(1 << (3 - 1),
								  std::numeric_limits<int>::max());
	path_metrics.front() = 0;
	for (int i = 0; i < nin; i += 2)
	{
		std::string current_bits(msg, i, 2);
		// If some bits are missing, fill with trailing zeros.
		// This is not ideal but it is the best we can do.
		if (current_bits.size() < 2)
		{
			current_bits.append(
				std::string(2 - current_bits.size(), '0'));
		}
		int current_int = std::atoi(current_bits.c_str());
		updatePathMetrics(current_int, &path_metrics, &trellis, mapStateInput);
	}

	// Traceback.
	std::string decoded;
	int state = std::min_element(path_metrics.begin(), path_metrics.end()) -
				path_metrics.begin();
	for (int i = trellis.size() - 1; i >= 0; i--)
	{
		decoded += state >> (3 - 2) ? "1" : "0";
		state = trellis[i][state];
	}
	std::reverse(decoded.begin(), decoded.end());

	// Remove (constraint_ - 1) flushing bits.
	return decoded.substr(0, decoded.size() - 3 + 1);
}

} /* namespace DNC2 */
} /* namespace gr */
