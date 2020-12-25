
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_FLOW_CONTROLLER_CC_IMPL_H
#define INCLUDED_DNC_FLOW_CONTROLLER_CC_IMPL_H

#include <DNC/flow_controller_cc.h>

namespace gr {
  namespace DNC {

    class flow_controller_cc_impl : public flow_controller_cc
    {
     private:
	unsigned char d_key;
	int d_control_mode;
	unsigned char d_rcv_key;
	bool d_check_key;

     public:
      flow_controller_cc_impl(unsigned char key, int control_mode);
      ~flow_controller_cc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_FLOW_CONTROLLER_CC_IMPL_H */

