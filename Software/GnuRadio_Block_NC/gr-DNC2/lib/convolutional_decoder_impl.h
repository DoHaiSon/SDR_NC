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

namespace gr {
  namespace DNC2 {

    class convolutional_decoder_impl : public convolutional_decoder
    {
     private:
      	std::vector<int> d_gen1;
     	std::vector<int> d_gen2;
      	char generator1;
      	char generator2;
	//branch metrix
        std::vector<int> aa;//00
	std::vector<int> ac;//11
	std::vector<int> ba;//11
	std::vector<int> bc;//00
	std::vector<int> cb;//10
	std::vector<int> cd;//01
	std::vector<int> db;//01
	std::vector<int> dd;//10
	//surviving metric
	std::vector<int> Ma;
	std::vector<int> Mb;
	std::vector<int> Mc;
	std::vector<int> Md;
	//trace
	std::vector<int> Aa; 
	std::vector<int> Ab;
	std::vector<int> Ac;
	std::vector<int> Ad;	

     public:
      convolutional_decoder_impl(const std::vector<int> gen1,
			const std::vector<int> gen2);
      ~convolutional_decoder_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
	char decode(char code1,char code2, char gen1, char gen2);
	std::vector<int> branch_metric_calculation(char rcode[], char value);
	void print_vector(std::vector<int>, int len);
    };

  } // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_CONVOLUTIONAL_DECODER_IMPL_H */

