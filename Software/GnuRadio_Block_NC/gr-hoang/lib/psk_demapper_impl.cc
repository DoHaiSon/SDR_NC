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
#include "psk_demapper_impl.h"
#include <vector>
#include <sys/time.h>
#include <cstdio>
#include <math.h>

namespace gr {
  namespace hoang {

    psk_demapper::sptr
    psk_demapper::make(int mapping_mode)
    {
      return gnuradio::get_initial_sptr
        (new psk_demapper_impl(mapping_mode));
    }

    /*
     * The private constructor
     */
    psk_demapper_impl::psk_demapper_impl(int mapping_mode)
      : gr::block("psk_demapper",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(char))), d_mapping_mode(mapping_mode), d_angles(mapping_mode,0), d_i(0), d_byte(0)
    {
	printf("init::Digital Demapper:: %i\n", mapping_mode);
	int i = 0;
	float pi =  3.141592653589793238462643383279502884197169399375;
	printf("init::Angles Matrix:: ");
	while (i < mapping_mode) {
		d_angles[i] = pi*(2*i+1)/mapping_mode;
		//d_symbols_psk[i] = gr_complex(cos(theta), sin(theta));
		printf("%.4f, ", d_angles[i]);//(float) (2*i-1)/mapping_mode);
		i++;
	}
	printf("\n");
	printf("%.4f, %.4f\n", asin(0), acos(0));
	set_output_multiple(1);
    }

    /*
     * Our virtual destructor.
     */
    psk_demapper_impl::~psk_demapper_impl()
    {
    }

    void psk_demapper_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        int pos = log(d_mapping_mode)/log(2);
        ninput_items_required[0] = (8/pos) * noutput_items;
    }

    int psk_demapper_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	char *out = (char *) output_items[0];

	int ni = 0;
	int no = 0;

	float real = 0.0;
	float imag = 0.0;
	float alpha;
	int pos = log(d_mapping_mode)/log(2);
	float pi =  3.141592653589793238462643383279502884197169399375;
	int byteDec = -1;

	while (ni < ninput_items && no < noutput_items)
	{
		real = in[ni].real();
		imag = in[ni].imag();
		//alpha = asin(imag);
		//if (real < 0) alpha = pi - alpha;
		//else if (imag < 0) alpha = 2*pi + alpha;
		if (real == 0) alpha = asin(imag); 
		else {
			alpha = atan(imag/real);
			if (real < 0) alpha = pi + alpha;
			else if (imag < 0) alpha = 2*pi + alpha;
		}		

		for (int i = 0;i<d_mapping_mode-1;i++) {
		   //if ((alpha - d_angles[i]) >= 0 && (alpha - d_angles[i])<(pi/d_mapping_mode)) byteDec = i;
			if (alpha >= d_angles[d_mapping_mode-1] || alpha < d_angles[0]) { byteDec = 0; break;}
			else if ((alpha >= d_angles[i]) && (alpha < d_angles[i+1])) {byteDec = i+1; break;};
		}

		//if (byteDec == -1) byteDec = d_mapping_mode - 1;
		//printf("real: %.4f, imag: %.4f, alpha: %.4f, byteDec: %i\n ", real, imag, alpha, byteDec);
		//printf("byteDec: %i or %#02X \n", byteDec, (unsigned char) byteDec);
		d_byte |= ((unsigned char) byteDec << d_i);
		d_i += pos;
		ni++;

		if (d_i % 8 == 0)	{
			out[no] = d_byte;
			d_i = 0;
			d_byte = 0x0;
			no++;
		}
	}

	consume_each(ni);
	return no;
   }

  } /* namespace hoang */
} /* namespace gr */

