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
#include "image_vector_sink_impl.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>

namespace gr {
  namespace hoang {

    image_vector_sink::sptr
    image_vector_sink::make(const char *filename, int vlen, int column)
    {
      return gnuradio::get_initial_sptr
        (new image_vector_sink_impl(filename, vlen, column));
    }

    /*
     * The private constructor
     */
    image_vector_sink_impl::image_vector_sink_impl(const char *filename, int vlen, int column)
      : gr::sync_block("image_vector_sink",
              gr::io_signature::make(1, 1, sizeof(unsigned char)*vlen),
              gr::io_signature::make(0, 0, 0)), d_filename(filename), d_vlen(vlen), d_column(column)
    {}

    /*
     * Our virtual destructor.
     */
    image_vector_sink_impl::~image_vector_sink_impl()
    {
    }

    std::vector<int>
    image_vector_sink_impl::data() const
    {
      return d_data;
    }

    std::vector<tag_t>
    image_vector_sink_impl::tags() const
    {
      return d_tags;
    }


    int
    image_vector_sink_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      std::ofstream fw(d_filename);
      unsigned char *iptr = (unsigned char*)input_items[0];
      if (fw.is_open())
      {
         for(int i = 0; i < noutput_items * d_vlen; i++)
         {
            d_data.push_back ((int) iptr[i]); //a[i] = d_data[i];
            if (((i+1)%d_column) == 0) { fw<<d_data[i]<<std::endl;}
	    else fw<<d_data[i]<<" ";
         }
	std::cout<<d_data.size();
        fw.close ();
      }
      else throw std::runtime_error("Error: Can't open file");
      //printf("a = ");
      //for (int j = 0; j<d_data.size();j++) { a[j] = d_data[j];}
      //printf("\n");*/
      //FILE *fp;
      //fp = fopen("/home/khachoang/GNU_Radio/gr-hoang/lib/imatext_rx_sink.txt","wb");
      //int a[10]={1,2,3,4,5,6,7,8,9,10};
      //char str[] = "This is tutorialspoint.com";
      //fwrite(a, 1 , d_data.size() , fp);
      //fwrite(str , 1 , sizeof(str) , fp);
      //fclose (fp);

      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
      d_tags.insert(d_tags.end(), tags.begin(), tags.end());
      return noutput_items;
    }

  } /* namespace hoang */
} /* namespace gr */

