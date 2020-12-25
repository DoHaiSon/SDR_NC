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

#ifndef INCLUDED_DNC2_IMAGE_VECTOR_SOURCE_IMPL_H
#define INCLUDED_DNC2_IMAGE_VECTOR_SOURCE_IMPL_H

#include <DNC2/image_vector_source.h>

namespace gr {
  namespace DNC2 {

    class image_vector_source_impl : public image_vector_source
    {
     private:
      std::vector<int> data;
      std::vector<unsigned char> d_data;
      bool d_repeat;
      int d_packet_size;
      int d_data_index;
      int d_end_data_idx;
      int d_pkt_no;
      unsigned int d_offset;
      unsigned char d_out_pkt_no;
      int d_channel_coding;
      int d_vlen;
      int d_image_size;
      bool d_settags;
      std::vector<tag_t> d_tags;
      unsigned int d_tagpos;
      const char* d_filename;
      bool d_check_end;
     public:
      image_vector_source_impl(const char *filename, int image_size, int packet_size,
		bool repeat, int channel_coding, int vlen, const std::vector<tag_t> &tags);
      ~image_vector_source_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_IMAGE_VECTOR_SOURCE_IMPL_H */

