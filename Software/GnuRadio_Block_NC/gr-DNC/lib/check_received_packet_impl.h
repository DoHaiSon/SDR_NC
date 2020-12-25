/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_CHECK_RECEIVED_PACKET_IMPL_H
#define INCLUDED_DNC_CHECK_RECEIVED_PACKET_IMPL_H

#include <DNC/check_received_packet.h>
#include <time.h> 

namespace gr {
  namespace DNC {

    class check_received_packet_impl : public check_received_packet
    {
     private:
	enum state_t
	{
		ST_WAIT_HEADER,
		ST_CHECK_HEADER,
		ST_HEADER_OUT,
		ST_OUT
	};
	state_t d_state;
	clock_t t0, t1;
	int d_out_size;
	char d_check_node_id;
	int d_data_index;
	int d_header_count;
	int d_node_id_count;
	
     public:
      check_received_packet_impl(int data_size, char check_node_id);
      ~check_received_packet_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_CHECK_RECEIVED_PACKET_IMPL_H */

