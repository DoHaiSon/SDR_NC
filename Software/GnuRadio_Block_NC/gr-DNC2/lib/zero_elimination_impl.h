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

#ifndef INCLUDED_DNC2_zero_elimination_IMPL_H
#define INCLUDED_DNC2_zero_elimination_IMPL_H

#include <DNC2/zero_elimination.h>

namespace gr {
  namespace DNC2 {

    class zero_elimination_impl : public zero_elimination
    {
     private:
      // Nothing to declare in this block.
	int d_node_id;
	int d_data_len;
	int d_data_index;
	std::vector<unsigned char> d_data;
	enum state_t
	{
		ST_ZERO_ELIMINATE,
		ST_DATA_READ
	};
	state_t d_state;


     public:
      zero_elimination_impl(int node_id, int data_len);
      ~zero_elimination_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_zero_elimination_IMPL_H */

