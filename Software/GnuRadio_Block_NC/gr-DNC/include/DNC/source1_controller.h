/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_SOURCE1_CONTROLLER_H
#define INCLUDED_DNC_SOURCE1_CONTROLLER_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API source1_controller : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<source1_controller> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::source1_controller.
       *
       * To avoid accidental use of raw pointers, DNC::source1_controller's
       * constructor is in a private implementation
       * class. DNC::source1_controller::make is the public interface for
       * creating new instances.
       */
      static sptr make(int buffer_size, int packet_size, int guard_interval, unsigned char source1_id, unsigned char source2_id, unsigned char relay1_id, unsigned char relay2_id, unsigned char destination1_id, unsigned char destination2_id, int network_coding);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_SOURCE1_CONTROLLER_H */

