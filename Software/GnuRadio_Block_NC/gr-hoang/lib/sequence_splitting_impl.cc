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
#include "sequence_splitting_impl.h"

namespace gr {
  namespace hoang {

    sequence_splitting::sptr
    sequence_splitting::make(int vlen, int cut_position)
    {
      return gnuradio::get_initial_sptr
        (new sequence_splitting_impl(vlen, cut_position));
    }

    /*
     * The private constructor
     */
    sequence_splitting_impl::sequence_splitting_impl(int vlen, int cut_position)
      : gr::block("sequence_splitting",
             gr::io_signature::make(1, 1, sizeof(unsigned char)),
             gr::io_signature::make(2, 2, sizeof(unsigned char))), 
	d_cut_position(cut_position), d_vlen(vlen),
	seq1(cut_position,0X00), 
	seq2(vlen - cut_position, 0X00)
    {}

    /*
     * Our virtual destructor.
     */
    sequence_splitting_impl::~sequence_splitting_impl()
    {
    }

    void
    sequence_splitting_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = noutput_items; 
    }

    int
    sequence_splitting_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];
	unsigned char *in = (unsigned char *) input_items[0];
        unsigned char *out_o = (unsigned char *) output_items[0];
        unsigned char *out_t = (unsigned char *) output_items[1];

	int i = 0, iseq1 = 0, iseq2 = 0;
	int ni = 0, no = 0, no_t = 0;
	/*while (i < d_vlen){
		if (i < (d_cut_position)) seq1[iseq1] = in[i];
		else 
		{
			//out_t[no - d_cut_position + 1] = in[ni + 1];
			seq2[iseq2] = in[i];
			iseq2 ++;
		}
		i++;
		iseq1++;
	}*/
	while (ni < ninput_items) {
            while (iseq1 < d_vlen) {
		if (iseq1 < (d_cut_position)) { out_o[iseq1] = in[ni]; iseq1 ++; ni ++;/*out_t[ni] = seq2[ni];*/}
		else { out_t[iseq2] = in[ni]; iseq2 ++; ni ++;}
		if (iseq2 = d_vlen) { iseq1 = 0; iseq2 = 0;};
	    }
	    
	}
	//out_o = iseq1;
	//out_t = iseq2;
        consume_each (ni);
        return no;
    }

  } /* namespace hoang */
} /* namespace gr */

