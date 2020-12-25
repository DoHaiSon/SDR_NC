/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_END_NODE_CONTROLLER_H
#define INCLUDED_DNC_END_NODE_CONTROLLER_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API end_node_controller : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<end_node_controller> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::end_node_controller.
       *
       * To avoid accidental use of raw pointers, DNC::end_node_controller's
       * constructor is in a private implementation
       * class. DNC::end_node_controller::make is the public interface for
       * creating new instances.
       */
      static sptr make(int packet_size, int guard_interval, unsigned char end_node_id, unsigned char relay_node_id);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_END_NODE_CONTROLLER_H */

