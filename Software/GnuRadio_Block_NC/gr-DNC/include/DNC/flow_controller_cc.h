
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_FLOW_CONTROLLER_CC_H
#define INCLUDED_DNC_FLOW_CONTROLLER_CC_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API flow_controller_cc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<flow_controller_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::flow_controller_cc.
       *
       * To avoid accidental use of raw pointers, DNC::flow_controller_cc's
       * constructor is in a private implementation
       * class. DNC::flow_controller_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(unsigned char key, int control_mode);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_FLOW_CONTROLLER_CC_H */

