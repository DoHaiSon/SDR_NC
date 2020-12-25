/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_RELAY_CONTROL_IMPL_H
#define INCLUDED_DNC_RELAY_CONTROL_IMPL_H

#include <DNC/relay_control.h>
#include <time.h>

namespace gr {
  namespace DNC {

    class relay_control_impl : public relay_control
    {
     private:
	enum state_t0
	{
		ST_BEACON_TRANS,
		ST_RECV_PACKET,
		ST_XORED_DATA,
		ST_HEADER_TRANS,
		ST_XORED_PACKET_NUMBER_TRANS,
		ST_XORED_DATA_TRANS
	};
	enum state_t1
	{
		ST_DO_BEACON_TRANS,
		ST_DO_XORED_DATA_TRANS
	};
	state_t0 d_state;
	state_t1 d_state_trans;

	int d_data_size;
	unsigned char d_end_nodeA_id;
	unsigned char d_end_nodeB_id;
	unsigned char d_relay_node_id;
	int d_timeout;

	clock_t d_clock;
	float d_clock_diff;
	int d_beacon_index;

	unsigned char d_next_packet_number;
	unsigned char d_trans_packet_number;
	unsigned char d_beacon_id;
	int d_packet_no;
	int d_req_data_retrans_index;
	int d_header_index;
	int d_packet_number_nodeA_index;
	int d_packet_number_nodeB_index;
	int d_packet_number_nodeA_count;
	int d_packet_number_nodeB_count;
	int d_xored_packet_number_index;
	int d_xored_data_index;
	int d_data_nodeA_index;
	int d_data_nodeB_index;
	bool d_recved_data_nodeA;
	bool d_recved_data_nodeB;
	bool d_reach_packet_number_nodeA;
	bool d_reach_packet_number_nodeB;
	bool d_reach_data_nodeA;
	bool d_reach_data_nodeB;
	bool d_beacon_trans;
	std::vector<unsigned char> d_data_nodeA;
	std::vector<unsigned char> d_data_nodeB;
	std::vector<unsigned char> d_xored_data;

     public:
      relay_control_impl(int data_size, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id, int timeout);
      ~relay_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_RELAY_CONTROL_IMPL_H */

