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
#include "signal_gain_impl.h"
#include <stdio.h>
#include <math.h>

namespace gr {
  namespace hoang {

    signal_gain::sptr
    signal_gain::make(float gain)
    {
      return gnuradio::get_initial_sptr
        (new signal_gain_impl(gain));
    }

    /*
     * The private constructor
     */
    signal_gain_impl::signal_gain_impl(float gain)
      : gr::block("signal_gain",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), d_gain(gain)
    {}

    /*
     * Our virtual destructor.
     */
    signal_gain_impl::~signal_gain_impl()
    {
    }

    void
    signal_gain_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items;
    }

    int
    signal_gain_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	int ni = 0;
	int no = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		out[no].real() = d_gain*in[ni].real();
		out[no].imag() = d_gain*in[ni].imag();
		ni ++;
		no ++;
	}

	consume_each(ni);
	return no;
}

  } /* namespace hoang */
} /* namespace gr */

