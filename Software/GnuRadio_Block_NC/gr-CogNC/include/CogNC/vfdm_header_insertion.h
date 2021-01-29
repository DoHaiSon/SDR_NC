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


#ifndef INCLUDED_COGNC_VFDM_HEADER_INSERTION_H
#define INCLUDED_COGNC_VFDM_HEADER_INSERTION_H

#include <CogNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace CogNC {

    /*!
     * \brief <+description of block+>
     * \ingroup CogNC
     *
     */
    class COGNC_API vfdm_header_insertion : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<vfdm_header_insertion> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of CogNC::vfdm_header_insertion.
       *
       * To avoid accidental use of raw pointers, CogNC::vfdm_header_insertion's
       * constructor is in a private implementation
       * class. CogNC::vfdm_header_insertion::make is the public interface for
       * creating new instances.
       */
      static sptr make(int data_burst_size, int header, int header_size);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_VFDM_HEADER_INSERTION_H */

