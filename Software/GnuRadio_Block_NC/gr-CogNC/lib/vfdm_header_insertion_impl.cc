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
#include "vfdm_header_insertion_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    vfdm_header_insertion::sptr
    vfdm_header_insertion::make(int data_burst_size, int header, int header_size)
    {
      return gnuradio::get_initial_sptr
        (new vfdm_header_insertion_impl(data_burst_size, header, header_size));
    }

    /*
     * The private constructor
     */
    vfdm_header_insertion_impl::vfdm_header_insertion_impl(int data_burst_size, int header, int header_size)
      : gr::block("vfdm_header_insertion",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	d_data_burst_size(data_burst_size), d_header(header), d_header_size(header_size), d_state(ST_HEADER_INSERTION), d_header_idx(0), d_data_idx(0)
    {
	if (header_size>=data_burst_size)
		throw std::runtime_error("Invalid parameter! Header size must be smaller than Data burst size!\n");
    }

    /*
     * Our virtual destructor.
     */
    vfdm_header_insertion_impl::~vfdm_header_insertion_impl()
    {
    }

    void
    vfdm_header_insertion_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    vfdm_header_insertion_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	int ninput_items = ninput_items_v[0];	
	
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

	int ni = 0;
	int no = 0;

	while (ni<ninput_items && no<noutput_items)
	{
		switch (d_state)
		{
		case ST_HEADER_INSERTION:
		{
			out[no] = (unsigned char) d_header;
			no ++;
			d_header_idx++;
			if(d_header_idx == d_header_size)
			{
				d_header_idx = 0;
				d_state = ST_DATA;
			}
			break;
		}
		case ST_DATA:
		{
			out[no] = in[ni];
			no ++;
			ni ++;
			d_data_idx++;
			if(d_data_idx == (d_data_burst_size-d_header_size))
			{
				d_data_idx = 0;
				d_state = ST_HEADER_INSERTION;
			}	
			break;
		}
		} 
	}
        // Do <+signal processing+>
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (ni);

        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace CogNC */
} /* namespace gr */

