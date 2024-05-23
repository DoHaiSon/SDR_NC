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

#ifndef INCLUDED_DNC2_CONVOLUTIONAL_DECODER_IMPL_H
#define INCLUDED_DNC2_CONVOLUTIONAL_DECODER_IMPL_H

#include <DNC2/convolutional_decoder.h>

namespace gr
{
namespace DNC2
{

class convolutional_decoder_impl : public convolutional_decoder
{
  private:
	std::vector<int> d_gen1;
	std::vector<int> d_gen2;
	char generator1;
	char generator2;
	int **mapStateInput;

  public:
	convolutional_decoder_impl(const std::vector<int> gen1,
							   const std::vector<int> gen2);
	~convolutional_decoder_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items,
					 gr_vector_int &ninput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items);
	int **initializeOutputs();
	int hammingDistance(int x, int y);
	int branchMetric(int received_bit,
					 int current_state,
					 int input,
					 int **mapStateInput);
	char decode(char codeword);
};

} // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_CONVOLUTIONAL_DECODER_IMPL_H */
