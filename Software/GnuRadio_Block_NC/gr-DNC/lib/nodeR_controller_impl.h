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
		ST_RX0_CHECK_ENDED_PACKET,
		ST_RX0_GET_DATA,
		ST_RX0_CHECK_GUARD_INTERVAL
	};
	enum state_rx1
	{
		ST_RX1_IDLE,
		ST_RX1_CHECK_NODE_ID,
		ST_RX1_CHECK_PACKET_NUMBER,
		ST_RX1_CHECK_ENDED_PACKET,
		ST_RX1_GET_DATA,
		ST_RX1_CHECK_GUARD_INTERVAL
	};
	enum state_tx
	{
		ST_MANAGE_DATA_TRANS,
		ST_HEADER_TRANS,
		ST_MAX_XOR_PACKET_NUMBER_TRANS,
		ST_CONTROL_INFO_TRANS,
		ST_DATA_TRANS,
		ST_GUARD_INTERVAL_TRANS,
		ST_END_PACKET_HEADER_TRANS,
		ST_END_PACKET_NUMBER_TRANS,
		ST_CHECK_TIMEOUT
	};
	state_rx0 d_rx0_state;
	state_rx1 d_rx1_state;
	state_tx d_tx_state;

	int d_buffer_size;
	int d_packet_size;
	int d_guard_interval;
	unsigned char d_nodeA_id;
	unsigned char d_nodeB_id;
	unsigned char d_nodeC_id;
	unsigned char d_nodeR_id;
	int d_network_coding;
	float d_priority;

	int d_ctrl_rx0;
	unsigned char d_rx0_node_id;
	int d_check_rx0_id_count;
	int d_check_rx0_id_index; 
	int d_rx0_packet_number_index;
	int d_rx0_check_ended_packet_index;
	int d_rx0_data_index;
	int d_rx0_check_guard_count;
	int d_rx0_check_guard_index;
	unsigned char d_rx0_pkt_no;
	unsigned char d_nodeA_ended_packet_number;

	int d_ctrl_rx1;
	unsigned char d_rx1_node_id;
	int d_check_rx1_id_count;
	int d_check_rx1_id_index; 
	int d_rx1_packet_number_index;
	int d_rx1_check_ended_packet_index;
	int d_rx1_data_index;
	int d_rx1_check_guard_count;
	int d_rx1_check_guard_index;
	unsigned char d_rx1_pkt_no;
	unsigned char d_nodeB_ended_packet_number;

	unsigned char d_max_xor_packet_number;
	bool d_send_max_xor_packet_number;
	bool d_sent_max_xor_packet_number;
	int d_send_max_xor_pkt_no_index;
	int d_send_max_xor_pkt_no_count;
	int d_ctrl_tx;
	bool d_ctrl_shift;
	bool d_erase_rx0;
	bool d_erase_rx1;
	bool d_first_rcv_rx0;
	bool d_first_rcv_rx1;
	int d_buffer_index;

	unsigned char d_tx_node_id;
	unsigned char d_tx_packet_number;
	int d_tx_guard_index;
	int d_trans_header_index;
	int d_data_trans_index;

	int d_shift;
	bool d_nodeA_send_ended_packet;
	bool d_nodeB_send_ended_packet;
	int d_tx_ended_packet_number_index;
	unsigned char d_tx_ended_packet_number;

	unsigned char d_rx0_packet_number[3];
	unsigned char d_rx1_packet_number[3];
	unsigned char d_rx0_ended_packet[3];
	unsigned char d_rx1_ended_packet[3];
	std::vector<unsigned char> d_rx0_packet;
	std::vector<unsigned char> d_rx1_packet;
	std::vector<unsigned char> d_data_nodeA_buffer;
	std::vector<unsigned char> d_data_nodeB_buffer;
	std::vector<unsigned char> d_xored_data_buffer;
	std::vector<unsigned char> d_packet_no_nodeA_buffer;
	std::vector<unsigned char> d_packet_no_nodeB_buffer;
	std::vector<unsigned char> d_xored_packet_no_buffer;

	int d_count_pkt_rx0;
	int d_count_pkt_rx1;
	int d_tx_ended_packet_nodeA;
	int d_tx_ended_packet_nodeB;

     public:
      nodeR_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding, float priority);
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

