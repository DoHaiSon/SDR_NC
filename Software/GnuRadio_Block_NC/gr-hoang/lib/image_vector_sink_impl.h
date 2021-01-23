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

#ifndef INCLUDED_HOANG_IMAGE_VECTOR_SINK_IMPL_H
#define INCLUDED_HOANG_IMAGE_VECTOR_SINK_IMPL_H

#include <hoang/image_vector_sink.h>

namespace gr {
  namespace hoang {

    class image_vector_sink_impl : public image_vector_sink
    {
     private:
      const char* d_filename;
      std::vector<int> d_data;
      std::vector<tag_t> d_tags;
      int d_vlen;
      int d_column;
      int *a;

     public:
      image_vector_sink_impl(const char *filename, int vlen, int column);
      ~image_vector_sink_impl();

      void reset() { d_data.clear(); }
      std::vector<int> data() const;
      std::vector<tag_t> tags() const;

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace hoang
} // namespace gr

#endif /* INCLUDED_HOANG_IMAGE_VECTOR_SINK_IMPL_H */

