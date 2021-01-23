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
#include "sequence_combination_impl.h"
#include <cstdio>

namespace gr {
  namespace hoang {

    sequence_combination::sptr
    sequence_combination::make()
    {
      return gnuradio::get_initial_sptr
        (new sequence_combination_impl());
    }

    /*
     * The private constructor
     */
    sequence_combination_impl::sequence_combination_impl()
      : gr::block("sequence_combination",
              gr::io_signature::make2(2, 2, sizeof(char),sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)))
    {}

    /*
     * Our virtual destructor.
     */
    sequence_combination_impl::~sequence_combination_impl()
    {
    }

    void
    sequence_combination_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = noutput_items; 
    }

    int
    sequence_combination_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items_o = ninput_items_v[0];
	int ninput_items_t = ninput_items_v[1];
	//int ninput_items_t = ninput_items_v[0];
        const char *in_o = (const char *) input_items[0];
        const char *in_t = (const char *) input_items[1];
	char *out = (char *) output_items[0];
	//printf("ninput_items = %i\n",ninput_items);
	int ni = 0;
	/*while (ni < (ninput_items + ninput_items)){
		if (ni < (ninput_items)) out[no] = in_o[ni];
		else out[no] = in_t[ni - ninput_items];
		ni++;
		no++;	
	}*/
	while (ni < ninput_items_o + ninput_items_t-1 && ni < noutput_items) {
		//printf("ninput_items_1 = %i\n",ninput_items_o);
		//printf("ninput_items_2 = %i\n",ninput_items_t);
		if (ni < (ninput_items_o)) out[ni] = in_o[ni];
		else out[ni] = in_t[ni - ninput_items_o];
		ni++;		
	}


        consume_each (ni);
        return ni;
    }

  } /* namespace hoang */
} /* namespace gr */

