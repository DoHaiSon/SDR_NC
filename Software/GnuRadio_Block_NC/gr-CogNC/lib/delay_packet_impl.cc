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
#include "delay_packet_impl.h"
#include <math.h>
#include <iostream>
#include <cstdio>

namespace gr {
  namespace CogNC {

    delay_packet::sptr
    delay_packet::make(int packet_length, int delay_coeff, float phase_shift)
    {
      return gnuradio::get_initial_sptr
        (new delay_packet_impl(packet_length, delay_coeff, phase_shift));
    }

    /*
     * The private constructor
     */
    delay_packet_impl::delay_packet_impl(int packet_length, int delay_coeff, float phase_shift)
      : gr::block("delay_packet",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), 
		d_delay_coeff(delay_coeff), 
		d_packet_length(packet_length), 
		d_state(ST_DELAY),
		d_phase_shift (phase_shift),
		d_phase_idx(0),
		d_phase_val(gr_complex(0,0)),
		d_data_index(0)
    {}

    /*
     * Our virtual destructor.
     */
    delay_packet_impl::~delay_packet_impl()
    {
    }

    void
    delay_packet_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items - d_delay_coeff*d_packet_length;
    }

    int
    delay_packet_impl::general_work (int noutput_items,
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
	    if (d_delay_coeff*d_packet_length == 0) 
	    {
		d_state = ST_DATA_OUT;
	    }
	    switch (d_state)
	    {
		case ST_DELAY: 
		{	
			out[no] = gr_complex(0, 0);
			d_data_index ++;
			if (d_data_index == d_delay_coeff*d_packet_length) 
			{
				d_state = ST_DATA_OUT;
			}
			no ++;
			break;
		}
		case ST_DATA_OUT: 
		{
			d_phase_val = std::polar(1.0, d_phase_idx * d_phase_shift);
			out[no] = d_phase_val*in [ni];
			d_phase_idx ++;
			no ++; ni ++;
			break;
		}
	    }
	}

        consume_each (ni);
	return no;
    }

  } /* namespace CogNC */
} /* namespace gr */

