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
#include "image_text_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <fstream>
#include <math.h>

namespace gr {
  namespace hoang {

    image_text_sink::sptr
    image_text_sink::make(const char *filename)
    {
      return gnuradio::get_initial_sptr
        (new image_text_sink_impl(filename));
    }

    /*
     * The private constructor
     */
    image_text_sink_impl::image_text_sink_impl(const char *filename)
      : gr::sync_block("image_text_sink",
              gr::io_signature::make(1,1,sizeof(int)),
              gr::io_signature::make(0,0,0)),d_filename(filename)
    {}

    /*
     * Our virtual destructor.
     */
    image_text_sink_impl::~image_text_sink_impl()
    {
    }

    int
    image_text_sink_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      const int *in = (int*)input_items[0];
      int i = 0;
      int size = noutput_items;

	  int ni = 0;
	  int temp;

      /*std::ofstream ftest("/home/khachoang/GNU_Radio/gr-hoang/lib/test.txt");
      if (ftest.is_open())
      {
         while (i < noutput_items)
         {
            for (int j = 0; j<9; j++)
            {
                ftest<<i<<" ";
            }
            ftest<<i<<std::endl;
            i ++;
         }
         ftest.close ();
         //throw std::runtime_error("Finished");
      }
      else throw std::runtime_error("Error: Can't open file");*/

      std::ofstream fw(d_filename);

      if (fw.is_open())
      {
         while (ni < 256)
         {
            //if (ni == 0) printf("%i \n",noutput_items);
            temp = in[ni];
            if (((ni+1)%10) == 0) fw<<temp<<std::endl;
            else fw<<temp<<" ";
            ni ++;
         }
         fw.close ();
         //throw std::runtime_error("Finished");
      }
      else throw std::runtime_error("Error: Can't open file");

        return ni;
    }

  } /* namespace hoang */
} /* namespace gr */

