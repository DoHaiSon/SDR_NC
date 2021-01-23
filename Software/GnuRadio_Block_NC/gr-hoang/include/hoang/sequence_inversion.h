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


#ifndef INCLUDED_HOANG_SEQUENCE_INVERSION_H
#define INCLUDED_HOANG_SEQUENCE_INVERSION_H

#include <hoang/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace hoang {

    /*!
     * \brief <+description of block+>
     * \ingroup hoang
     *
     */
    class HOANG_API sequence_inversion : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<sequence_inversion> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of hoang::sequence_inversion.
       *
       * To avoid accidental use of raw pointers, hoang::sequence_inversion's
       * constructor is in a private implementation
       * class. hoang::sequence_inversion::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace hoang
} // namespace gr

#endif /* INCLUDED_HOANG_SEQUENCE_INVERSION_H */

