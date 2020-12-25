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
#include "hybrid_stream_controller_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    hybrid_stream_controller::sptr
    hybrid_stream_controller::make(int cr_mode, int ofdm_stream_length, int vfdm_stream_length)
    {
      return gnuradio::get_initial_sptr
        (new hybrid_stream_controller_impl(cr_mode, ofdm_stream_length, vfdm_stream_length));
    }

    /*
     * The private constructor
     */
    hybrid_stream_controller_impl::hybrid_stream_controller_impl(int cr_mode, int ofdm_stream_length, int vfdm_stream_length)
      : gr::block("hybrid_stream_controller",
              gr::io_signature::make2(2, 2, sizeof(gr_complex)*ofdm_stream_length, sizeof(gr_complex)*vfdm_stream_length),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), d_cr_mode(cr_mode),
	d_ofdm_length(ofdm_stream_length), d_vfdm_length(vfdm_stream_length), d_ofdm_count(0)
    {
	if (ofdm_stream_length!=vfdm_stream_length)
		throw std::runtime_error("Invalid parameter! VFDM frame and OFDM frame should be equal in length!\n");	
    }

    /*
     * Our virtual destructor.
     */
    hybrid_stream_controller_impl::~hybrid_stream_controller_impl()
    {
    }

    void
    hybrid_stream_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items/d_ofdm_length;
    }

    int
    hybrid_stream_controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items_0 = ninput_items_v[0];
	int ninput_items_1 = ninput_items_v[1];
        const gr_complex *in_ofdm = (const gr_complex *) input_items[0];
        const gr_complex *in_vfdm = (const gr_complex *) input_items[1];
        gr_complex *out = (gr_complex *) output_items[0];

	int ni0 = 0;
	int ni1 = 0;
	int nidx_ofdm = 0;
	int nidx_vfdm = 0;
	int no = 0;
	int i, j;
	 
	if (d_cr_mode == 0)
	{
		while (ni0<ninput_items_0 && ni1<ninput_items_1 && no<noutput_items)
		{
			unsigned int d_preamble_length = 144;
			memcpy(&out[no], &in_ofdm[nidx_ofdm], d_preamble_length * sizeof(gr_complex));
			no += d_preamble_length;
			for (j=d_preamble_length;j<d_vfdm_length;j++)
			{
				out[no] = in_ofdm[nidx_ofdm + j]+in_vfdm[nidx_vfdm+j];
				no++;
			}
			no++;
			nidx_ofdm += d_ofdm_length;
			ni0 ++;
			nidx_vfdm += d_vfdm_length;
			ni1 ++;
			printf("HYBRID frame transmitted \n");
		}
	} else 
	while (ni0<ninput_items_0 && no<noutput_items)
	{
		
		if (ninput_items_1-ni1 == 0) 
		{
			//if (d_cr_mode == 1)
			//{
				memcpy(&out[no], &in_ofdm[nidx_ofdm], d_ofdm_length * sizeof(gr_complex));
				nidx_ofdm += d_ofdm_length;
				ni0 ++;
				no+=d_ofdm_length;
				d_ofdm_count++;
				//printf("OFDM frames no %i transmitted \n",d_ofdm_count);
			//} else {
			//	for (i=0;i<d_ofdm_length;i++)
			//	{
			//		out[no] = gr_complex(0,0);
			//		no++;
			//	}
			//}		
		}
		else if (ninput_items_1 - ni1 > 0)
		{
			unsigned int d_preamble_length = 144;
			memcpy(&out[no], &in_ofdm[nidx_ofdm], d_preamble_length * sizeof(gr_complex));
			no += d_preamble_length;
			for (j=d_preamble_length;j<d_vfdm_length;j++)
			{
				out[no] = in_ofdm[nidx_ofdm + j]+in_vfdm[nidx_vfdm+j];
				no++;
			}
			no++;
			nidx_ofdm += d_ofdm_length;
			ni0 ++;
			nidx_vfdm += d_vfdm_length;
			ni1 ++;
			printf("%i OFDM frames transmitted \n", d_ofdm_count);
			printf("HYBRID frame transmitted \n");
			d_ofdm_count = 0;
		}
		else if (ninput_items_1 - ni1 < 0)
		{
			printf("---------------------------------Invalid State!\n");
		}
	}
        consume (0, ni0);
        consume (1, ni1);
        return no;
    } /* general work */

  } /* namespace CogNC */
} /* namespace gr */
