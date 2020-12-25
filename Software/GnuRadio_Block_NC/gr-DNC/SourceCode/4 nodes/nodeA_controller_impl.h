
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_NODEA_CONTROLLER_IMPL_H
#define INCLUDED_DNC_NODEA_CONTROLLER_IMPL_H

#include <DNC/nodeA_controller.h>

namespace gr {
  namespace DNC {

    class nodeA_controller_impl : public nodeA_controller
    {
     private:
	enum state_tx
	{
		ST_TX_BUFFER_MANAGEMENT,
		ST_TX_HEADER_TRANS,
		ST_TX_DATA_TRANS,
		ST_TX_GUARD_INTERVAL_TRANS,
		ST_TX_END_PACKET_HEADER_TRANS,
		ST_TX_END_PACKET_NUMBER_TRANS,
		ST_TX_SESSION_NUMBER_TRANS
	};
	enum state_rx0
	{
		ST_RX0_IDLE,
		ST_RX0_CHECK_NODE_ID,
		ST_RX0_CHECK_PACKET_NUMBER,
		ST_RX0_CHECK_SESSION_NUMBER
	};
	enum state_rx1
	{
		ST_RX1_IDLE,
		ST_RX1_CHECK_NODE_ID,
		ST_RX1_CHECK_PACKET_NUMBER,
		ST_RX1_CHECK_SESSION_NUMBER
	};
	state_tx d_tx_state;
	state_rx0 d_rx0_state;
	state_rx1 d_rx1_state;

	int d_buffer_size;
	int d_packet_size;
	int d_guard_interval;
	unsigned char d_nodeA_id;
	unsigned char d_nodeC_id;
	unsigned char d_nodeR_id;

	int d_branch;
	bool d_load_data;
	unsigned char d_rx0_node_id_check;
	int d_check_rx0_id_index;
	int d_check_rx0_id_count;
	int d_check_rx0_packet_number_index;

	unsigned char d_rx1_node_id_check;
	int d_check_rx1_id_index;
	int d_check_rx1_id_count; 
	int d_check_rx1_packet_number_index;

	unsigned char d_next_session_node_id;
	unsigned char d_curr_session_node_id;
	unsigned char d_next_session_number;
	int d_check_session_count;
	int d_check_session_index;
	bool d_send;

	int d_load_data_index;
	unsigned char d_load_packet_number;
	unsigned char d_ended_packet_number;
	int d_load_packet_index;
	int d_number_of_session_packets;
	bool d_end_data;

	bool d_send_data;
	int d_tx_buffer_index;
	int d_trans_header_index;
	int d_trans_data_index;
	int d_guard_index;
	int d_ended_packet_number_trans_index;
	int d_session_trans_index;
	unsigned char d_tx_node_id;

	std::vector<unsigned char> d_loaded_packet_number;
	std::vector<unsigned char> d_transmitted_packet_number;
	std::vector<unsigned char> d_transmitted_data_buffer;
	std::vector<unsigned char> d_confirm_packet_number;
	unsigned char d_rx0_packet_number[3];
	unsigned char d_rx1_packet_number[3];
	void reset();


     public:
      nodeA_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeC_id, unsigned char nodeR_id);
      ~nodeA_controller_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_NODEA_CONTROLLER_IMPL_H */

