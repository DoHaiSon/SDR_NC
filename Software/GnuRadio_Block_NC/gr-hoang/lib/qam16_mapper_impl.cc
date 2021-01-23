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
#include "qam16_mapper_impl.h"
#include <vector>
#include <sys/time.h>
#include <cstdio>

namespace gr {
  namespace hoang {

    qam16_mapper::sptr
    qam16_mapper::make()
    {
      return gnuradio::get_initial_sptr
        (new qam16_mapper_impl());
    }

    /*
     * The private constructor
     */
    qam16_mapper_impl::qam16_mapper_impl()
      : gr::block("qam16_mapper",
		gr::io_signature::make(1,1, sizeof(char)),
                gr::io_signature::make(1,1, sizeof(gr_complex))),d_symbols(16,gr_complex(0, 0))
    {
	printf("init::QAM16_Mapper\n");

	d_symbols[0] = gr_complex(-0.25, -0.25); // 0000
	d_symbols[1] = gr_complex(-0.25, -0.75); // 0001
	d_symbols[2] = gr_complex(-0.75, -0.25); // 0010
	d_symbols[3] = gr_complex(-0.75, -0.75); // 0011
	d_symbols[4] = gr_complex(-0.25, +0.25); // 0100
	d_symbols[5] = gr_complex(-0.25, +0.75); // 0101
	d_symbols[6] = gr_complex(-0.75, +0.25); // 0110
	d_symbols[7] = gr_complex(-0.75, +0.75); // 0111
	d_symbols[8] = gr_complex(+0.25, -0.25); // 1000
	d_symbols[9] = gr_complex(+0.25, -0.75); // 1001
	d_symbols[10] = gr_complex(+0.75, -0.25); // 1010
	d_symbols[11] = gr_complex(+0.75, -0.75); // 1011
	d_symbols[12] = gr_complex(+0.25, +0.25); // 1100
	d_symbols[13] = gr_complex(+0.25, +0.75); // 1101
	d_symbols[14] = gr_complex(+0.75, +0.25); // 1110
	d_symbols[15] = gr_complex(+0.75, +0.75); // 1111

	set_output_multiple(16);
    }

    /*
     * Our virtual destructor.
     */
    qam16_mapper_impl::~qam16_mapper_impl()
    {
    }

    void
    qam16_mapper_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;        
	//ninput_items_required[0] = 2 * noutput_items;
    }

    int
    qam16_mapper_impl::general_work (int noutput_items,
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

	while (ni < ninput_items && no < noutput_items)
	{
		byte = in[ni];

		out[no + 0] = d_symbols[((byte >> 0) & 0xF)];
		out[no + 1] = d_symbols[((byte >> 4) & 0xF)];

		ni++;
		no += 2;
	}

	consume_each(ni);
	return no;
    }

  } /* namespace hoang */
} /* namespace gr */

