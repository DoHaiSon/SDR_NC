/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_NODER_CONTROLLER_IMPL_H
#define INCLUDED_DNC_NODER_CONTROLLER_IMPL_H

#include <DNC/nodeR_controller.h>

namespace gr {
  namespace DNC {

    class nodeR_controller_impl : public nodeR_controller
    {
     private:
	enum state_rx0
	{
		ST_RX0_IDLE,
		ST_RX0_CHECK_NODE_ID,
		ST_RX0_CHECK_PACKET_NUMBER,
		ST_RX0_CHECK_ENDED_PACKET_NUMBER,
		ST_RX0_CHECK_SESSION_NUMBER,
		ST_RX0_GET_DATA,
		ST_RX0_CHECK_END_PACKET
	};
	enum state_rx1
	{
		ST_RX1_IDLE,
		ST_RX1_CHECK_NODE_ID,
		ST_RX1_CHECK_PACKET_NUMBER,
		ST_RX1_CHECK_ENDED_PACKET_NUMBER,
		ST_RX1_CHECK_SESSION_NUMBER,
		ST_RX1_GET_DATA,
		ST_RX1_CHECK_END_PACKET
	};
	enum state_tx
	{
		ST_INIT,
		ST_MANAGE_DATA_TRANS,
		ST_HEADER_TRANS,
		ST_SESSION_NUMBER_TRANS,
		ST_DATA_TRANS
	};
	state_rx0 d_rx0_state;
	state_rx1 d_rx1_state;
	state_tx  d_tx_state;

	int d_buffer_size;
	int d_packet_size;
	int d_guard_interval;
	unsigned char d_nodeA_id;
	unsigned char d_nodeB_id;
	unsigned char d_nodeR_id;
	int d_network_coding;

	int d_check_rx0_id_index;
	int d_check_rx0_id_count;
	int d_rx0_packet_number_index;
	unsigned char d_rx0_pkt_no;
	int d_rx0_data_index;
	int d_rx0_check_end_packet_count;
	int d_rx0_check_end_packet_index;
	int d_check_rx0_session_count;
	int d_check_rx0_session_index;

	int d_branch;
	int d_check_rx1_id_index;
	int d_check_rx1_id_count;
	int d_rx1_packet_number_index;
	int d_check_rx1_session_index;
	int d_check_rx1_session_count;
	int d_rx1_check_end_packet_count;
	int d_rx1_check_end_packet_index;
	unsigned char d_rx1_pkt_no;
	int d_rx1_data_index;
	int d_trans_header_index;
	int d_session_trans_index;
	int d_buffer_index;
	int d_count;
	int d_data_trans_index;
	unsigned char d_node_id;
	unsigned char d_tx_node_id;
	unsigned char d_rx1_node_id;
	unsigned char d_tx_packet_number;
	unsigned char d_next_session_number;
	unsigned char d_nodeA_ended_packet_number;
	unsigned char d_nodeB_ended_packet_number;
	int d_check_ended_packet_idx;
	unsigned char d_check_ended_packet_number[6];
	unsigned char d_rx0_packet_number[3];
	unsigned char d_rx1_packet_number[3];
	std::vector<unsigned char> d_rx0_packet;
	std::vector<unsigned char> d_rx1_packet;
	std::vector<unsigned char> d_data_nodeA_buffer;
	std::vector<unsigned char> d_data_nodeB_buffer;
	std::vector<unsigned char> d_xored_data_buffer;
	std::vector<unsigned char> d_packet_no_nodeA_buffer;
	std::vector<unsigned char> d_packet_no_nodeB_buffer;
	std::vector<unsigned char> d_xored_packet_no_buffer;

     public:
      nodeR_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeR_id, int network_coding);
      ~nodeR_controller_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_NODER_CONTROLLER_IMPL_H */

