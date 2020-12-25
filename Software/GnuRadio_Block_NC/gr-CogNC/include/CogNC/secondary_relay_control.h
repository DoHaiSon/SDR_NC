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


#ifndef INCLUDED_COGNC_SECONDARY_RELAY_CONTROL_H
#define INCLUDED_COGNC_SECONDARY_RELAY_CONTROL_H

#include <CogNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace CogNC {

    /*!
     * \brief <+description of block+>
     * \ingroup CogNC
     *
     */
    class COGNC_API secondary_relay_control : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<secondary_relay_control> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of CogNC::secondary_relay_control.
       *
       * To avoid accidental use of raw pointers, CogNC::secondary_relay_control's
       * constructor is in a private implementation
       * class. CogNC::secondary_relay_control::make is the public interface for
       * creating new instances.
       */
      static sptr make(int packet_size, int buffer_size, int guard_interval_downlink, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_id);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_SECONDARY_RELAY_CONTROL_H */

