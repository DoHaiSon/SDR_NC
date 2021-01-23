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
#include "qam16_demapper_impl.h"
#include <vector>
#include <sys/time.h>
#include <cstdio>

namespace gr {
  namespace hoang {

    qam16_demapper::sptr
    qam16_demapper::make()
    {
      return gnuradio::get_initial_sptr
        (new qam16_demapper_impl());
    }

    /*
     * The private constructor
     */
    qam16_demapper_impl::qam16_demapper_impl()
      : gr::block("qam16_demapper",
		gr::io_signature::make(1, 1, sizeof(gr_complex)),
		gr::io_signature::make(1, 1, sizeof(char))), d_i(0), d_byte(0)
    {
	printf("init::qam4_demapper\n");
	set_output_multiple(1);
	d_one = 0;
	d_zero = 0;
    }

    /*
     * Our virtual destructor.
     */
    qam16_demapper_impl::~qam16_demapper_impl()
    {
    }

    void
    qam16_demapper_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = 2 * noutput_items;
    }

    int
    qam16_demapper_impl::general_work (int noutput_items,
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

	while (ni < ninput_items && no < noutput_items)
	{
		real = in[ni].real();
		imag = in[ni].imag();
		//printf("real: %.4f, imag: %.4f\n ", real, imag);

		if (real >= 0.5) {
			if(imag >= 0.5)  	d_byte |= (0xF << d_i);
			else if (imag >= 0) 	d_byte |= (0xE << d_i);
			else if (imag >= -0.5) 	d_byte |= (0xA << d_i);
			else 			d_byte |= (0xB << d_i);
		} else if (real >= 0.0) {
			if(imag >= 0.5)  	d_byte |= (0xD << d_i);
			else if (imag >= 0) 	d_byte |= (0xC << d_i);
			else if (imag >= -0.5) 	d_byte |= (0x8 << d_i);
			else 			d_byte |= (0x9 << d_i);
		} else if (real >= -0.5) {
			if(imag >= 0.5)  	d_byte |= (0x5 << d_i);
			else if (imag >= 0) 	d_byte |= (0x4 << d_i);
			else if (imag >= -0.5) 	d_byte |= (0x0 << d_i);
			else 			d_byte |= (0x1 << d_i);
		} else	{
			if(imag >= 0.5)  	d_byte |= (0x7 << d_i);
			else if (imag >= 0) 	d_byte |= (0x6 << d_i);
			else if (imag >= -0.5) 	d_byte |= (0x2 << d_i);
			else 			d_byte |= (0x3 << d_i);
		}

		d_i += 4;
		ni++;

		if (d_i % 8 == 0)
		{
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

