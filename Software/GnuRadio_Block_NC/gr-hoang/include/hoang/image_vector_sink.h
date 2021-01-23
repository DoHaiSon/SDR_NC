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


#ifndef INCLUDED_HOANG_IMAGE_VECTOR_SINK_H
#define INCLUDED_HOANG_IMAGE_VECTOR_SINK_H

#include <hoang/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace hoang {

    /*!
     * \brief <+description of block+>
     * \ingroup hoang
     *
     */
    class HOANG_API image_vector_sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<image_vector_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of hoang::image_vector_sink.
       *
       * To avoid accidental use of raw pointers, hoang::image_vector_sink's
       * constructor is in a private implementation
       * class. hoang::image_vector_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char *filename, int vlen = 1, int column = 256);

      virtual void reset() = 0;
      virtual std::vector<int> data() const = 0;
      virtual std::vector<tag_t> tags() const = 0;
    };

  } // namespace hoang
} // namespace gr

#endif /* INCLUDED_HOANG_IMAGE_VECTOR_SINK_H */

