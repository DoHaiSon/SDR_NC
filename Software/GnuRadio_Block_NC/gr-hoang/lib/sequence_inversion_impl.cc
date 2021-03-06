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
#include "sequence_inversion_impl.h"
#include <stdio.h>

namespace gr {
  namespace hoang {

    sequence_inversion::sptr
    sequence_inversion::make()
    {
      return gnuradio::get_initial_sptr
        (new sequence_inversion_impl());
    }

    /*
     * The private constructor
     */
    sequence_inversion_impl::sequence_inversion_impl()
      : gr::block("sequence_inversion",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)))
    {}

    /*
     * Our virtual destructor.
     */
    sequence_inversion_impl::~sequence_inversion_impl()
    {
    }

    void
    sequence_inversion_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items;
    }

    int
    sequence_inversion_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];        
	const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];
	int ni = 0, no = 0;
	while (ni < (ninput_items)) { //printf("%i\n", ninput_items);
		out[no] = in[ninput_items - ni - 1];
		ni++;
		no++;	
	}

        consume_each (ni);
        return no;
    }

  } /* namespace hoang */
} /* namespace gr */

