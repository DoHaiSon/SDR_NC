/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_COGNC_FLOW_FILLING_IMPL_H
#define INCLUDED_COGNC_FLOW_FILLING_IMPL_H

#include <CogNC/flow_filling.h>

namespace gr {
  namespace CogNC {

    class flow_filling_impl : public flow_filling
    {
     private:
	enum state
	{
		ST_BUFFER_FILL,
		ST_BUFFER_CHECK,
		ST_BUFFER_OUT	
	};
	state d_state;
	int d_buffer_idx;
	int d_buffer_sz;
	std::vector<gr_complex> d_buffer;

     public:
      flow_filling_impl(int buffer_sz);
      ~flow_filling_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_FLOW_FILLING_IMPL_H */

