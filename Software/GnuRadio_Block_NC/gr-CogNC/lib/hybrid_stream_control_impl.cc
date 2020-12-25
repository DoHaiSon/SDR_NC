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
#include "hybrid_stream_control_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    hybrid_stream_control::sptr
    hybrid_stream_control::make(int ofdm_stream_length, int vfdm_stream_length)
    {
      return gnuradio::get_initial_sptr
        (new hybrid_stream_control_impl(ofdm_stream_length, vfdm_stream_length));
    }

    /*
     * The private constructor
     */
    hybrid_stream_control_impl::hybrid_stream_control_impl(int ofdm_stream_length, int vfdm_stream_length)
      : gr::block("hybrid_stream_control",
              gr::io_signature::make2(2, 2, sizeof(gr_complex),sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), 
	d_ofdm_length(ofdm_stream_length), d_vfdm_length(vfdm_stream_length), d_ofdm_out(false), d_vfdm_out(false), d_ofdm_out_finish(true), d_vfdm_out_finish(true), d_ofdm_buffer_idx(0), d_vfdm_buffer_idx(0), d_ofdm_out_idx(0), d_hybrid_out_idx(0), d_ofdm_frame_count(0), d_in_state(ST_OFDM_CHECK), d_out_state(ST_IDLE)
    {
	if (ofdm_stream_length!=vfdm_stream_length)
		throw std::runtime_error("Invalid parameter! VFDM frame and OFDM frame should be equal in length!\n");	
		
	d_ofdm_buffer.resize(ofdm_stream_length);
	std::fill(d_ofdm_buffer.begin(), d_ofdm_buffer.end(), 0x00);

	d_vfdm_buffer.resize(vfdm_stream_length);
	std::fill(d_vfdm_buffer.begin(), d_vfdm_buffer.end(), 0x00);
	
	gettimeofday(&t_begin, NULL);
    }

    /*
     * Our virtual destructor.
     */
    hybrid_stream_control_impl::~hybrid_stream_control_impl()
    {
    }

    void
    hybrid_stream_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
	/*unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;*/
    }

    int
    hybrid_stream_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	int ninput_items_0 = ninput_items_v[0];
	int ninput_items_1 = ninput_items_v[1];
        const gr_complex *in_ofdm = (const gr_complex *) input_items[0];
        const gr_complex *in_vfdm = (const gr_complex *) input_items[1];
        gr_complex *out = (gr_complex *) output_items[0];

	int ni0 = 0, ni1 = 0;
	int no = 0;
	//printf("ni0: %i, ni1: %i, no: %i \n", ninput_items_0, ninput_items_1, noutput_items);
	
	while (ni1 < ninput_items_1 && d_vfdm_out_finish == true)
	{
		//printf("VFDM Frame storing ... \n");
		d_vfdm_buffer[d_vfdm_buffer_idx] = in_vfdm[ni1];
		d_vfdm_buffer_idx ++;
		ni1 ++;
		if (d_vfdm_buffer_idx == d_vfdm_length)
		{
			printf("VFDM Frame stored \n");
			d_vfdm_buffer_idx = 0;
			d_vfdm_out_finish = false;
			d_vfdm_out = true;
			break;
		}
	} 
	
	while (ni0 < ninput_items_0 && d_ofdm_out_finish == true)
	{
		//printf("OFDM Frame storing %i... \n",d_ofdm_buffer_idx);
		if (d_ofdm_buffer_idx == 0) gettimeofday(&t_begin, NULL);
		d_ofdm_buffer[d_ofdm_buffer_idx] = in_ofdm[ni0];
		d_ofdm_buffer_idx ++;
		ni0 ++;
		if (d_ofdm_buffer_idx == d_ofdm_length)
		{
			//printf("OFDM Frame stored, ninput_items_1 = %i \n",ninput_items_1);
			gettimeofday(&t_end, NULL);
			mtime = (t_end.tv_sec*(uint64_t)1000000 + t_end.tv_usec) - 
					(t_begin.tv_sec*(uint64_t)1000000 + t_begin.tv_usec);
			//printf("OFDM storing time: %lu ms or %lu us\n",mtime/1000,mtime);
			d_ofdm_buffer_idx = 0;
			d_ofdm_out = true;
			d_ofdm_out_finish = false;
			break;
		}
	}
	//**************************************************************
	while (no < noutput_items)
	{
		switch (d_out_state)
		{
		case ST_IDLE:
		{
			//printf("ST_IDLE\n");
			if (d_ofdm_out == true)
			{
				if (d_vfdm_out == true)
				{
					//printf("IDLE --> HYBRID_OUT\n");
					d_out_state = ST_HYBRID_OUT;
				}
				else
				{
					//printf("IDLE --> OFDM_OUT\n");
					d_out_state = ST_OFDM_OUT;					
				}	
			}
			out[no] = gr_complex(0,0);
			no ++;
			break;
		}
		case ST_OFDM_OUT:
		{
			//printf("OFDM OUT %i\n",d_ofdm_out_idx);
			if (d_ofdm_out_idx == 0) gettimeofday(&t_begin, NULL);
			out[no] = d_ofdm_buffer[d_ofdm_out_idx];
			d_ofdm_out_idx ++;
			no ++;
			if (d_ofdm_out_idx == d_ofdm_length)
			{
				gettimeofday(&t_end, NULL);
				mtime = (t_end.tv_sec*(uint64_t)1000000 + t_end.tv_usec) - 
					(t_begin.tv_sec*(uint64_t)1000000 + t_begin.tv_usec);
				//printf("OFDM out time: %lu ms or %lu us\n",mtime/1000,mtime);
				
				//printf("OFDM transmitted\n");
				d_ofdm_frame_count ++;
				d_ofdm_out_idx = 0;
				d_ofdm_out_finish = true;
				d_ofdm_out = false;
				d_out_state = ST_IDLE;
			}
			break;
		}
		case ST_HYBRID_OUT:
		{	
			//std::cout<<"state = ST_HYBRID_OUT "<<d_hybrid_out_idx<<"\n";
			int d_preamble_length = 144;
			if (d_hybrid_out_idx < d_vfdm_length)
			{
				out[no] = d_ofdm_buffer[d_hybrid_out_idx];
				d_hybrid_out_idx ++;
				no ++;
			}
			if (d_hybrid_out_idx >= d_preamble_length && d_hybrid_out_idx < d_vfdm_length)
			{
				out[no] = d_ofdm_buffer[d_hybrid_out_idx] + d_vfdm_buffer[d_hybrid_out_idx];
				d_hybrid_out_idx ++;
				no ++;
			}
			if (d_hybrid_out_idx == d_vfdm_length)
			{
				printf("%i OFDM frames transmitted \n", d_ofdm_frame_count);
				printf("HYBRID frame transmitted \n");
				d_ofdm_frame_count = 0;
				d_hybrid_out_idx = 0;
				d_ofdm_out_finish = true;
				d_vfdm_out_finish = true;
				d_ofdm_out = false;
				d_vfdm_out = false;
				d_out_state = ST_IDLE;
			}
			break;
		}
		}
	}
        consume (0, ni0);
        consume (1, ni1);

        return no;
    }

  } /* namespace CogNC */
} /* namespace gr */
