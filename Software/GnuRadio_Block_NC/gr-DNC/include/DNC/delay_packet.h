/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/


#ifndef INCLUDED_DNC_DELAY_PACKET_H
#define INCLUDED_DNC_DELAY_PACKET_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API delay_packet : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<delay_packet> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::delay_packet.
       *
       * To avoid accidental use of raw pointers, DNC::delay_packet's
       * constructor is in a private implementation
       * class. DNC::delay_packet::make is the public interface for
       * creating new instances.
       */
      static sptr make(int data_size, int max_delay);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_DELAY_PACKET_H */

