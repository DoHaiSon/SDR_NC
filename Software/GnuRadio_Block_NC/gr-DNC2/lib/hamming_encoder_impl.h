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

#ifndef INCLUDED_DNC2_HAMMING_ENCODER_IMPL_H
#define INCLUDED_DNC2_HAMMING_ENCODER_IMPL_H

#include <DNC2/hamming_encoder.h>

namespace gr {
  namespace DNC2 {

    class hamming_encoder_impl : public hamming_encoder
    {
     private:
	enum state
	{
		ST_ENCODING,
		ST_PACKETIZE,
		ST_FREE
	};
	state d_state;

	int K;
	int N;
	int M;
	int d_image_size;
	int d_packet_size;
	int d_offset;
	int d_packet_index;
	int d_packet_no;
	int d_ctrl;

     public:
      hamming_encoder_impl(int m, int image_size, int packet_size);
      ~hamming_encoder_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
      char encode(char info);
    };

  } // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_HAMMING_ENCODER_IMPL_H */

