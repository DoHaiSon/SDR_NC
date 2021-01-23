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
#include "psk_mapper_impl.h"
#include <cstdio>
#include <math.h>
#include <vector>
#include <sys/time.h>

namespace gr {
  namespace hoang {

    psk_mapper::sptr
    psk_mapper::make(int mapping_mode)
    {
      return gnuradio::get_initial_sptr
        (new psk_mapper_impl(mapping_mode));
    }

    /*
     * The private constructor
     */
    psk_mapper_impl::psk_mapper_impl(int mapping_mode)
      : gr::block("psk_mapper",
             gr::io_signature::make(1, 1, sizeof(char)),
             gr::io_signature::make(1, 1, sizeof(gr_complex))), d_mapping_mode(mapping_mode), d_symbols_psk(mapping_mode,gr_complex(0,0))
    {
	int pos = log(d_mapping_mode)/log(2);
	printf("init::Digital_Mapper:: %i, %i\n", mapping_mode, pos);
	int i = 0;
	float pi =  3.141592653589793238462643383279502884197169399375;
	printf("init::Constellation point:: ");
	while (i < mapping_mode) {
		float theta = 2*pi*i/mapping_mode; printf("%i*pi/%i---",2*i,mapping_mode);
		d_symbols_psk[i] = gr_complex(cos(theta), sin(theta));
		printf("%.4f+1i*(%.4f), ", d_symbols_psk[i].real(), d_symbols_psk[i].imag());
		i++;
	}
	printf("\n");
	set_output_multiple(mapping_mode);
    }

    /*
     * Our virtual destructor.
     */
    psk_mapper_impl::~psk_mapper_impl()
    {
    }

    void
    psk_mapper_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;     
    }

    int
    psk_mapper_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];
	unsigned char *in = (unsigned char *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
        
	int ni = 0;
	int no = 0;
	unsigned char byte = 0x0;
	unsigned char exceed = d_mapping_mode - 1;
	int pos = log(d_mapping_mode)/log(2);
	while (ni < ninput_items && no < noutput_items) {
		byte = in[ni];
		for (int i = 0;i <= 8/pos-1;i++) {
			out[no + i] = d_symbols_psk[((byte >> i*pos) & exceed)];
		}
		ni++;
		no += 8/pos;
    	}
	consume_each(ni);
	return no;
    }

  } /* namespace hoang */
} /* namespace gr */

