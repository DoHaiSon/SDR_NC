/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/



#ifndef INCLUDED_DNC_RECEIVER_CONTROL_P2P_H
#define INCLUDED_DNC_RECEIVER_CONTROL_P2P_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API receiver_control_p2p : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<receiver_control_p2p> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::receiver_control_p2p.
       *
       * To avoid accidental use of raw pointers, DNC::receiver_control_p2p's
       * constructor is in a private implementation
       * class. DNC::receiver_control_p2p::make is the public interface for
       * creating new instances.
       */
      static sptr make(int packet_size, unsigned char transmitter_id, unsigned char receiver_id);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_RECEIVER_CONTROL_P2P_H */

