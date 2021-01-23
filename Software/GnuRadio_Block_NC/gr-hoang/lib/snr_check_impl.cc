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
#include "snr_check_impl.h"

namespace gr {
  namespace hoang {

    snr_check::sptr
    snr_check::make(int para_size, float snr_thres)
    {
      return gnuradio::get_initial_sptr
        (new snr_check_impl(para_size, snr_thres));
    }

    /*
     * The private constructor
     */
    snr_check_impl::snr_check_impl(int para_size, float snr_thres)
      : gr::block("snr_check",
              gr::io_signature::make2(2, 2, para_size * sizeof(gr_complex), sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), d_snr_thres(snr_thres), d_para_size(para_size), 
					d_null(para_size,gr_complex(0,0))
    {  
	
    }

    /*
     * Our virtual destructor.
     */
    snr_check_impl::~snr_check_impl()
    {
    }

    void
    snr_check_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
	  for (unsigned i = 0; i < ninputs; i++)
	    ninput_items_required[i] = 1;
    }

    int
    snr_check_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	int ninput_items_data = ninput_items[0];
	int ninput_items_snr = ninput_items[1];
        
	const gr_complex *in_data = (const gr_complex *) input_items[0];
	const float *in_snr = (const float *) input_items[1];
        gr_complex *out = (gr_complex *) output_items[0];

	int ni = 0, no = 0;

	while (ni < ninput_items_data && no < noutput_items) {
		if (in_snr[ni] >= d_snr_thres) 
			memcpy(&out[ni * d_para_size], &in_data[ni * d_para_size], d_para_size * sizeof(gr_complex));
		else //out[no] = d_null; 
			for (int k = 0;k<d_para_size;k++) out[no+k] = gr_complex(0,0);
		ni++;
		no+= d_para_size;		
	}

        consume_each (ni);

        return no;
    }

  } /* namespace hoang */
} /* namespace gr */

