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

#ifndef INCLUDED_COGNC_VFDM_HEADER_REMOVAL_IMPL_H
#define INCLUDED_COGNC_VFDM_HEADER_REMOVAL_IMPL_H

#include <CogNC/vfdm_header_removal.h>

namespace gr {
  namespace CogNC {

    class vfdm_header_removal_impl : public vfdm_header_removal
    {
     private:
	enum state
	{
		ST_IDLE,
		ST_HEADER_CHECK,
		ST_DATA_OUT
	};
	state d_state;
	
	int d_data_burst_size;
	int d_header;
	int d_header_size;
	
	int d_header_idx;
	int d_header_count;	
	int d_data_idx;

     public:
      vfdm_header_removal_impl(int data_burst_size, int header, int header_size);
      ~vfdm_header_removal_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_VFDM_HEADER_REMOVAL_IMPL_H */

