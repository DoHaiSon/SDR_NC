/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_LC_DECODING_H
#define INCLUDED_DNC_LC_DECODING_H

#include <DNC/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace DNC {

    /*!
     * \brief <+description of block+>
     * \ingroup DNC
     *
     */
    class DNC_API LC_decoding : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<LC_decoding> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of DNC::LC_decoding.
       *
       * To avoid accidental use of raw pointers, DNC::LC_decoding's
       * constructor is in a private implementation
       * class. DNC::LC_decoding::make is the public interface for
       * creating new instances.
       */
      static sptr make(int image_width, int image_height);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_LC_DECODING_H */

