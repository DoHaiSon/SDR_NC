/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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
#include "radom_complex_generator_impl.h"
#include <math.h>
#include <iostream>

namespace gr {
  namespace CogNC {

    radom_complex_generator::sptr
    radom_complex_generator::make(int vector_length, bool repeat)
    {
      return gnuradio::get_initial_sptr
        (new radom_complex_generator_impl(vector_length, repeat));
    }

    /*
     * The private constructor
     */
    radom_complex_generator_impl::radom_complex_generator_impl(int vector_length, bool repeat)
      : gr::block("radom_complex_generator",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), d_vector_length(vector_length), d_repeat(repeat)
    {}

    /*
     * Our virtual destructor.
     */
    radom_complex_generator_impl::~radom_complex_generator_impl()
    {
    }

    void
    radom_complex_generator_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
    }

    int
    radom_complex_generator_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

	//d_real = ((double) rand() / (RAND_MAX));
	//d_imag = ((double) rand() / (RAND_MAX));
	//out
	//std::cout<<"";// d_real<<" ,"<<d_imag<<std::endl;
        
	return noutput_items;
    }

  } /* namespace CogNC */
} /* namespace gr */

