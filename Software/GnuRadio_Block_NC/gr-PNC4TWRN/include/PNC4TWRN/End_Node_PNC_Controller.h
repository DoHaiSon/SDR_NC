
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_PNC4TWRN_END_NODE_PNC_CONTROLLER_H
#define INCLUDED_PNC4TWRN_END_NODE_PNC_CONTROLLER_H

#include <PNC4TWRN/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace PNC4TWRN {

    /*!
     * \brief <+description of block+>
     * \ingroup PNC4TWRN
     *
     */
    class PNC4TWRN_API End_Node_PNC_Controller : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<End_Node_PNC_Controller> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of PNC4TWRN::End_Node_PNC_Controller.
       *
       * To avoid accidental use of raw pointers, PNC4TWRN::End_Node_PNC_Controller's
       * constructor is in a private implementation
       * class. PNC4TWRN::End_Node_PNC_Controller::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fft_length, int occupied_tones, int cp_length, int node);
    };

  } // namespace PNC4TWRN
} // namespace gr

#endif /* INCLUDED_PNC4TWRN_END_NODE_PNC_CONTROLLER_H */

