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

#ifndef INCLUDED_HOANG_SEQUENCE_SPLITTING_IMPL_H
#define INCLUDED_HOANG_SEQUENCE_SPLITTING_IMPL_H

#include <hoang/sequence_splitting.h>

namespace gr {
  namespace hoang {

    class sequence_splitting_impl : public sequence_splitting
    {
     private:
      int d_vlen;
      int d_cut_position;
      std::vector<unsigned char> seq1;
      std::vector<unsigned char> seq2;

     public:
      sequence_splitting_impl(int vlen, int cut_position);
      ~sequence_splitting_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace hoang
} // namespace gr

#endif /* INCLUDED_HOANG_SEQUENCE_SPLITTING_IMPL_H */

