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
#include "flow_filling_impl.h"
#include <math.h>
#include <cstdio>

namespace gr {
  namespace CogNC {

    flow_filling::sptr
    flow_filling::make(int buffer_sz)
    {
      return gnuradio::get_initial_sptr
        (new flow_filling_impl(buffer_sz));
    }

    /*
     * The private constructor
     */
    flow_filling_impl::flow_filling_impl(int buffer_sz)
      : gr::block("flow_filling",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), d_buffer_sz(buffer_sz), d_state(ST_BUFFER_FILL), d_buffer_idx(0)
    {
	d_buffer.resize(d_buffer_sz);
	std::fill(d_buffer.begin(), d_buffer.end(), gr_complex(0,0));
    }

    /*
     * Our virtual destructor.
     */
    flow_filling_impl::~flow_filling_impl()
    {
    }

    void
    flow_filling_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        //ninput_items_required[0] = noutput_items - 1;
    }

    int
    flow_filling_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {	
	int ninput_items = ninput_items_v[0];
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];
	
	int ni = 0;
	int no = 0;
	int nidx;
	//printf("ni = %i,\t", ninput_items);
	//printf("no = %i\n", noutput_items);
	
	/*if (ninput_items > 0)
	{
		printf("ni = %i, no = %i \n", ninput_items, noutput_items);
}*/
		if (ninput_items > noutput_items) 
		{
			while (no < noutput_items)
			{
				out[no] = in[ni];
				no ++;
				ni ++;
			}
		} 
		else 
		{	
			while (no < ninput_items)
			{
				out[no] = in[ni];
				no ++;
				ni ++;
			}
			while (no < noutput_items)
			{
				out[no] = gr_complex(0,0);
				no ++;
			}		
		}
	/*} else {
		//printf("no = %i,\n", noutput_items);
		while (no < (int) noutput_items)
		{
		out[no] = gr_complex(0,0);
		no ++;
		}
	}	
	/*while (ni < ninput_items)
	{
		printf("ni = %i,\t", ninput_items);
		d_buffer[d_buffer_idx] = in[ni];
		d_buffer_idx ++; 
		ni ++; 
		/*switch (d_state)
		{
		case ST_BUFFER_FILL:
			d_buffer[d_buffer_idx] = in[ni];
			out[no] = gr_complex(0,0);
			d_buffer_idx ++; 
			ni ++; 
			no ++;
			if (d_buffer_idx == d_buffer_sz)
			{
				d_buffer_idx = 0;
				d_state = ST_BUFFER_OUT;
			}
			break;
		case ST_BUFFER_OUT:
			out[no] = d_buffer[d_buffer_idx];
			d_buffer_idx ++;
			no ++;
			if (d_buffer_idx == d_buffer_sz)
			{
				d_buffer_idx = 0;
				d_state = ST_BUFFER_FILL;
			}
			break;
		}
	}
	if (d_buffer_idx == d_buffer_sz)
	{
		while (d_buffer_idx > 0)
		{
			out[no] = d_buffer[d_buffer_sz - d_buffer_idx];
			no ++;			
			d_buffer_idx --;
		}
		while (no < noutput_items)
		{
			out[no] = gr_complex(0,0);
			no ++;
		}
        } else {
		while (no < noutput_items)
		{
			out[no] = gr_complex(0,0);
			no ++;
		}
	}*/

        consume_each (ni);
        return no;
    }

  } /* namespace CogNC */
} /* namespace gr */
