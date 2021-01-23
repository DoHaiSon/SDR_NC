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

#ifndef INCLUDED_HOANG_IMAGE_VECTOR_SOURCE_IMPL_H
#define INCLUDED_HOANG_IMAGE_VECTOR_SOURCE_IMPL_H

#include <hoang/image_vector_source.h>

namespace gr {
  namespace hoang {

    class image_vector_source_impl : public image_vector_source
    {
     private:
      std::vector<int> data;
      std::vector<unsigned char> d_data;
      bool d_repeat;
      unsigned int d_offset;
      int d_vlen;
      int d_image_size;
      bool d_settags;
      std::vector<tag_t> d_tags;
      unsigned int d_tagpos;
      const char* d_filename;

     public:
      image_vector_source_impl(const char *filename, int image_size,
		bool repeat, int vlen, const std::vector<tag_t> &tags);
      ~image_vector_source_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace hoang
} // namespace gr

#endif /* INCLUDED_HOANG_IMAGE_VECTOR_SOURCE_IMPL_H */

