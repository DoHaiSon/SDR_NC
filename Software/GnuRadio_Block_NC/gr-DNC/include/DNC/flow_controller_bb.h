
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_FLOW_CONTROLLER_BB_H
#define INCLUDED_DNC_FLOW_CONTROLLER_BB_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API flow_controller_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<flow_controller_bb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::flow_controller_bb.
       *
       * To avoid accidental use of raw pointers, DNC::flow_controller_bb's
       * constructor is in a private implementation
       * class. DNC::flow_controller_bb::make is the public interface for
       * creating new instances.
       */
      static sptr make(unsigned char key, int control_mode);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_FLOW_CONTROLLER_BB_H */

