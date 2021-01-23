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
#include "char_to_int_impl.h"

namespace gr {
  namespace hoang {

    char_to_int::sptr
    char_to_int::make()
    {
      return gnuradio::get_initial_sptr
        (new char_to_int_impl());
    }

    /*
     * The private constructor
     */
    char_to_int_impl::char_to_int_impl()
      : gr::block("char_to_int",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(int)))
    {}

    /*
     * Our virtual destructor.
     */
    char_to_int_impl::~char_to_int_impl()
    {
    }

    void
    char_to_int_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    char_to_int_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int ninput_items = ninput_items_v[0];
        const unsigned char *in = (const unsigned char *) input_items[0];
        int *out = (int *) output_items[0];

        int ni = 0;

        while (ni < ninput_items) {
            out[ni] = (int) in[ni];
            ni ++;
        }
        consume_each (ni);

        // Tell runtime system how many output items we produced.
        return ni;
    }

  } /* namespace hoang */
} /* namespace gr */

