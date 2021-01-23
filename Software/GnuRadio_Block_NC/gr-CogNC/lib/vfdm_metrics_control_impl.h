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

#ifndef INCLUDED_COGNC_VFDM_METRICS_CONTROL_IMPL_H
#define INCLUDED_COGNC_VFDM_METRICS_CONTROL_IMPL_H

#include <CogNC/vfdm_metrics_control.h>
#include <gnuradio/fft/fft.h>
#include <vector>
#include <sys/time.h>

namespace gr {
  namespace CogNC {

    class vfdm_metrics_control_impl : public vfdm_metrics_control
    {
     private:
	int d_data_symbol_length;
	int d_channel_size;
	//int d_metrics_size;
	int d_normalize_h;
	int d_null_channel_count;

	fft::fft_complex *d_ifft;

	gr_complex* d_nulled_syms;
	gr_complex* d_nulled_h_t;
 
	struct timeval t_begin,t_end,t_temp;
	unsigned long mtime, mtime_t;

     public:
      vfdm_metrics_control_impl(int data_symbol_length, int channel_size, int normalize_channel);
      ~vfdm_metrics_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_VFDM_METRICS_CONTROL_IMPL_H */

