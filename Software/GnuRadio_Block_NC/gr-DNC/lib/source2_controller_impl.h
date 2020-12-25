/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_SOURCE2_CONTROLLER_IMPL_H
#define INCLUDED_DNC_SOURCE2_CONTROLLER_IMPL_H

#include <DNC/source2_controller.h>

namespace gr {
  namespace DNC {

    class source2_controller_impl : public source2_controller
    {
     private:
	int d_buffer_size;
	int d_packet_size;
	int d_guard_interval;
	unsigned char d_source1_id;
	unsigned char d_source2_id;
	unsigned char d_relay1_id;
	unsigned char d_relay2_id;
	unsigned char d_destination1_id;
	unsigned char d_destination2_id;
	int d_network_coding;

     public:
      source2_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char source1_id, unsigned char source2_id, unsigned char relay1_id, unsigned char relay2_id, unsigned char destination1_id, unsigned char destination2_id, int network_coding);
      ~source2_controller_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_SOURCE2_CONTROLLER_IMPL_H */

