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


#ifndef INCLUDED_DNC2_IMAGE_VECTOR_SOURCE_H
#define INCLUDED_DNC2_IMAGE_VECTOR_SOURCE_H

#include <DNC2/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace DNC2 {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC2
     *
     */
    class DNC2_API image_vector_source : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<image_vector_source> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC2::image_vector_source.
       *
       * To avoid accidental use of raw pointers, DNC2::image_vector_source's
       * constructor is in a private implementation
       * class. DNC2::image_vector_source::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char *filename, int image_size, int packet_size,
                 bool repeat, int channel_coding, int vlen, const std::vector<tag_t> &tags);
    };

  } // namespace DNC2
} // namespace gr

#endif /* INCLUDED_DNC2_IMAGE_VECTOR_SOURCE_H */

