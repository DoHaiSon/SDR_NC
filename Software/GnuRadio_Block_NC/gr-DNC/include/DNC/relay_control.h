/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_RELAY_CONTROL_H
#define INCLUDED_DNC_RELAY_CONTROL_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API relay_control : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<relay_control> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::relay_control.
       *
       * To avoid accidental use of raw pointers, DNC::relay_control's
       * constructor is in a private implementation
       * class. DNC::relay_control::make is the public interface for
       * creating new instances.
       */
      static sptr make(int data_size, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id, int timeout);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_RELAY_CONTROL_H */

