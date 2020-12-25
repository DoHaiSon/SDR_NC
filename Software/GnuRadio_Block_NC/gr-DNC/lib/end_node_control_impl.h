/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_END_NODE_CONTROL_IMPL_H
#define INCLUDED_DNC_END_NODE_CONTROL_IMPL_H

#include <DNC/end_node_control.h>

namespace gr {
  namespace DNC {

    class end_node_control_impl : public end_node_control
    {
     private:
	enum state_t
	{
		ST_IDLE,
		ST_CHECK_PACKET_TYPE,
		ST_STORE_RECV_DATA,
		ST_XOR_OPERATION,
		ST_GET_ORIGIN_DATA,
		ST_HEADER_TRANS,
		ST_PACKET_NUMBER_TRANS,
		ST_DATA_TRANS
	};
	state_t d_state;

	int d_data_size;
	unsigned char d_end_node_id;
	unsigned char d_relay_node_id;
	unsigned char d_beacon_id;

	unsigned char d_next_packet_number;
	unsigned char d_trans_packet_number;
	int d_check_index;
	int d_beacon_count;
	int d_check_packet_number_count;
	bool d_check_recv_beacon;
	bool d_check_recv_xored_data;
	bool d_check_xored_data_out;
	int d_origin_data_index;
	int d_header_index;
	int d_packet_number_count;
	int d_packet_number_index;
	int d_data_trans_index;
	int d_xored_data_index;
	int d_data_out_index;
	std::vector<unsigned char> d_temp_data;
	std::vector<unsigned char> d_origin_data;
	std::vector<unsigned char> d_xored_data;
	std::vector<unsigned char> d_data_out;
     public:
      end_node_control_impl(int data_size, unsigned char end_node_id, unsigned char relay_node_id);
      ~end_node_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_END_NODE_CONTROL_IMPL_H */

