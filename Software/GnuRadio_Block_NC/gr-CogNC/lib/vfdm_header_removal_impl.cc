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
#include "vfdm_header_removal_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    vfdm_header_removal::sptr
    vfdm_header_removal::make(int data_burst_size, int header, int header_size)
    {
      return gnuradio::get_initial_sptr
        (new vfdm_header_removal_impl(data_burst_size, header, header_size));
    }

    /*
     * The private constructor
     */
    vfdm_header_removal_impl::vfdm_header_removal_impl(int data_burst_size, int header, int header_size)
      : gr::block("vfdm_header_removal",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	d_data_burst_size(data_burst_size), d_header(header), d_header_size(header_size), d_state(ST_IDLE), d_header_idx(0), d_header_count(0), d_data_idx(0)
    {
	if (header_size>=data_burst_size)
		throw std::runtime_error("Invalid parameter! Header size must be smaller than Data burst size!\n");
    }

    /*
     * Our virtual destructor.
     */
    vfdm_header_removal_impl::~vfdm_header_removal_impl()
    {
    }

    void
    vfdm_header_removal_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    vfdm_header_removal_impl::general_work (int noutput_items,
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
	    switch(d_state)
	    {
		case ST_IDLE:
		{
			out[no] = in[ni];
			ni++; no++;
			d_header_idx ++;
			if (d_header_idx == 3)
			{
				d_header_idx = 0;
				d_state = ST_HEADER_CHECK;
			}
			break;
		}
		case ST_HEADER_CHECK: // eliminate the redundant 0x00 bytes
		{
			ni++;
			d_header_count ++;
			if (d_header_count == 3)
			{
				d_header_count = 0;
				d_state = ST_DATA_OUT;
			}
			break;
		}
		case ST_DATA_OUT: // read data and push to output
		{	
			out[no] = in[ni];
			ni++; no++;
			d_data_idx ++;	
			if (d_data_idx == d_data_burst_size)
			{
				d_data_idx = 0;
				d_state = ST_IDLE;
			} 
			break;
		}
	    }
		/*switch (d_state)
		{
		case ST_IDLE:
		{
			if(in[ni] == d_header)
			{
				d_state = ST_HEADER_CHECK;
				d_header_idx++;
				d_header_count++;
			}
			ni++;
			break;
		}
		case ST_HEADER_CHECK:
		{
			if(in[ni]==d_header)
			{
				d_header_count++;
			}
			ni++;
			d_header_idx++;
			if(d_header_idx==d_header_size)
			{
				if(d_header_count==d_header_size)
				{
					d_state = ST_DATA_OUT;
				}
				else
				{
					d_state = ST_IDLE;
					ni = ni - 1;
				}
				d_header_idx = 0;
				d_header_count = 0;
			}
			break;
		}
		case ST_DATA_OUT:
		{
			out[no] = in[ni];
			no ++;
			ni ++;
			d_data_idx++;
			if(d_data_idx == (d_data_burst_size-d_header_size))
			{
				d_data_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		}
		}*/
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

