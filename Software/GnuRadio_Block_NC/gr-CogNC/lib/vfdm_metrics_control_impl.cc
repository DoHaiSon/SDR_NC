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
#include "vfdm_metrics_control_impl.h"
#include <cstdio>
#include <string.h>

namespace gr {
  namespace CogNC {

    vfdm_metrics_control::sptr
    vfdm_metrics_control::make(int data_symbol_length, int channel_size, int normalize_channel)
    {
      return gnuradio::get_initial_sptr
        (new vfdm_metrics_control_impl(data_symbol_length, channel_size, normalize_channel));
    }

    /*
     * The private constructor
     */
    vfdm_metrics_control_impl::vfdm_metrics_control_impl(int data_symbol_length, int channel_size, int normalize_channel)
      : gr::block("vfdm_metrics_control",
              gr::io_signature::make2(2, 2, sizeof(gr_complex)*channel_size, sizeof(gr_complex)*data_symbol_length),
				gr::io_signature::make2(2, 2,
						sizeof(gr_complex) * data_symbol_length,
						sizeof(gr_complex) * channel_size)), 
	d_data_symbol_length(data_symbol_length), d_channel_size(channel_size), d_normalize_h(normalize_channel), d_null_channel_count(0)
{
	// Load FFTW
	d_ifft = new fft::fft_complex(channel_size, true);

	d_nulled_syms = new gr_complex[data_symbol_length];
	d_nulled_h_t = new gr_complex[channel_size];
	int i = 0;
	for (i = 0; i < data_symbol_length; i++)
	{
		d_nulled_syms[i] = gr_complex(0, 0);
	}
	for (i = 0; i < channel_size; i++)
	{
		d_nulled_h_t[i] = gr_complex(0, 0);
	}

	gettimeofday(&t_begin, NULL);
}

    /*
     * Our virtual destructor.
     */
    vfdm_metrics_control_impl::~vfdm_metrics_control_impl()
    {
    }

    void
    vfdm_metrics_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
    }

    int
    vfdm_metrics_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	int ninput_items_0 = ninput_items_v[0];        
	int ninput_items_1 = ninput_items_v[1];        
	
	const gr_complex *in_h = (const gr_complex *) input_items[0];
	const gr_complex *in_syms = (const gr_complex *) input_items[1];

	gr_complex *out_vfdm = (gr_complex *) output_items[0];
	gr_complex *out_h_t = (gr_complex *) output_items[1];

	int i = 0;

	int ni0 = 0;
	int ni1 = 0;
	int no = 0;

	int nidx_h = 0;
	int nidx_syms = 0;

	int nodx_vfdm = 0;
	int nodx_h_t = 0;

	while (ni0 < ninput_items_0 && ni1 < ninput_items_1 && no < noutput_items)
	{
		if (in_h[nidx_h] == gr_complex(0,0))
		{
			/****************************************************
			 ZEROS
			 *****************************************************/
			// Send zeros to the all outputs
			/*memcpy(&out_vfdm[nodx_vfdm], &d_nulled_syms[0],
					sizeof(gr_complex) * d_data_symbol_length);
			memcpy(&out_h_t[nodx_h_t], &d_nulled_h_t[0],
					sizeof(gr_complex) * d_channel_size);
			nodx_vfdm += d_data_symbol_length;
			nodx_h_t += d_channel_size;
			no++;*/
			nidx_h += d_channel_size;
			ni0 ++;
			//d_null_channel_count++;
		}
		else
		{
			//std::cout<<"After "<<d_null_channel_count<<" null channels, Got real channel!\n";
			//gettimeofday(&t_end, NULL);
			//mtime = (t_end.tv_sec*(uint64_t)1000000 + t_end.tv_usec) - (t_begin.tv_sec*(uint64_t)1000000 + t_begin.tv_usec);
			
			//std::cout<<"noutput_items: "<<noutput_items<<"\t";
			//printf("Null channels: %i \t Channel arrival time: %lu ms or %lu us\n",d_null_channel_count, mtime/1000,mtime);
			d_null_channel_count = 0;
			/****************************************************
			 VFDM
			 *****************************************************/
			// Copy symbols to the input buffer
			memcpy(d_ifft->get_inbuf(), &in_h[nidx_h],
					d_channel_size * sizeof(gr_complex));

			// Get the input buffer of FFTW
			gr_complex *dst = d_ifft->get_inbuf();

			// half length of complex array (for FFT shift)
			unsigned int len = (unsigned int) (floor(d_channel_size / 2.0));
			memcpy(&dst[0], &in_h[nidx_h + len],
					sizeof(gr_complex) * (d_channel_size - len));
			memcpy(&dst[d_channel_size - len], &in_h[nidx_h],
					sizeof(gr_complex) * len);

			// Execute IFFT
			d_ifft->execute();

			// Send the h_t to the output
			memcpy(&out_h_t[nodx_h_t], d_ifft->get_outbuf(),
					sizeof(gr_complex) * d_channel_size);

			// Normalize h_t
			if (d_normalize_h == 1)
			{
				float max_abs_h = 0.0f;
				float max_abs_h_inv = 0.0f;
				float abs_h;
				for (i = 0; i < d_channel_size; i++)
				{
					abs_h = abs(out_h_t[nodx_h_t + i]);
					if (abs_h > max_abs_h)
					{
						max_abs_h = abs_h;
					}
				}
				if (max_abs_h > 0)
				{
					max_abs_h_inv = 1 / (max_abs_h);
					//std::cout<<"normalized channel: ";
					for (i = 0; i < d_channel_size; i++)
					{
						gr_complex s1 = out_h_t[nodx_h_t + i];
						out_h_t[nodx_h_t + i] = max_abs_h_inv * s1;
						//printf("%.4f+i*%.4f, ", out_h_t[nodx_h_t + i].real(), out_h_t[nodx_h_t + i].imag());
					}
					//std::cout<<std::endl;
				}
			}

			// Send the input symbols to the vfdm_out
			memcpy(&out_vfdm[nodx_vfdm], &in_syms[nidx_syms],
					sizeof(gr_complex) * d_data_symbol_length);
		nodx_vfdm += d_data_symbol_length;
		nodx_h_t += d_channel_size;
		no++;		
		nidx_syms += d_data_symbol_length;
		nidx_h += d_channel_size;
		ni0++;
		ni1++;
		//gettimeofday(&t_begin, NULL);
		}
	}

	consume(0, ni0);
	consume(1, ni1);
	return no;
    }

  } /* namespace CogNC */
} /* namespace gr */
