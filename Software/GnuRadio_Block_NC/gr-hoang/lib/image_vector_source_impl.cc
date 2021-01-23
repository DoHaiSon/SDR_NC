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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "image_vector_source_impl.h"
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <math.h>

namespace gr {
  namespace hoang {

    image_vector_source::sptr
    image_vector_source::make(const char *filename, int image_size, 
                 bool repeat, int vlen, const std::vector<tag_t> &tags)
    {
      return gnuradio::get_initial_sptr
        (new image_vector_source_impl(filename, image_size, repeat,vlen,tags));
    }

    /*
     * The private constructor
     */
    image_vector_source_impl::image_vector_source_impl(const char *filename, int image_size,
		bool repeat, int vlen, const std::vector<tag_t> &tags)
      : gr::sync_block("image_vector_source",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(unsigned char) * vlen)),
      d_filename(filename),
      d_repeat(repeat),
      d_offset(0),
      d_vlen(vlen),
      d_image_size (image_size),
      d_tags(tags),
      d_tagpos(0),
      d_data(image_size,0X0)

    {
      /*Read data from file*/
      int no = 0, temp;
      std::ifstream f(filename);
      std::ofstream fw("/home/khachoang/GNU_Radio/gr-hoang/lib/imatext_rx_source.txt");
      if (f.is_open()) 
      {
	int idx = 0;
         while (no < d_image_size)
         {
            f>>temp; d_data[no] = (unsigned char) temp;
	    if (((no+1)%6) == 0) { fw<<idx<<std::endl;idx++;}
	    else fw<<idx<<" ";
 	    no ++;
         }
         f.close (); fw.close();
      }  else throw std::runtime_error("Error: Can't open file");
	
      if(tags.size() == 0) {
        d_settags = 0;
      }
      else {
        d_settags = 1;
        set_output_multiple(data.size() / vlen);
      }
      if((data.size() % vlen) != 0)
        throw std::invalid_argument("data length must be a multiple of vlen");
    }

    image_vector_source_impl::~image_vector_source_impl()
    {
    }

    int
    image_vector_source_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      unsigned char *optr = (unsigned char *) output_items[0];
      /*Repeat*/
      if(d_repeat) {
        unsigned int size = d_data.size ();
        unsigned int offset = d_offset;
        if(size == 0)
          return -1;

        if(d_settags) {
          int n_outputitems_per_vector = d_data.size() / d_vlen;
          for(int i = 0; i < noutput_items; i += n_outputitems_per_vector) {
            // FIXME do proper vector copy
            memcpy((void *) optr, (const void*)&d_data[0], size*sizeof (unsigned char));
            optr += size;
            for(unsigned t = 0; t < d_tags.size(); t++) {
              add_item_tag(0, nitems_written(0)+i+d_tags[t].offset,
                           d_tags[t].key, d_tags[t].value);
            }
          }
        }
        else {
          for(int i = 0; i < noutput_items*d_vlen; i++) {
            optr[i] = d_data[offset++];
	    if(offset >= size) {
              offset = 0;
            }
          }
        }

        d_offset = offset;
        return noutput_items;
      /*No repeat*/
      }
      else {
        if(d_offset >= d_data.size ())
          return -1;  // Done!

        unsigned int n = std::min((unsigned int)d_data.size() - d_offset,
                              (unsigned int)noutput_items*d_vlen);
        for(unsigned int i = 0; i < n; i++) {
          optr[i] = d_data[d_offset + i]; 
        }
        for(unsigned t = 0; t < d_tags.size(); t++) {
          if((d_tags[t].offset >= d_offset) && (d_tags[t].offset < d_offset+n))
            add_item_tag(0, d_tags[t].offset, d_tags[t].key, d_tags[t].value);
        }
        d_offset += n;
        return n/d_vlen;
      }
    }

  } /* namespace hoang */
} /* namespace gr */

