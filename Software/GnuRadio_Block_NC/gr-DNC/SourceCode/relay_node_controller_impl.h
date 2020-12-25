/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_RELAY_NODE_CONTROLLER_IMPL_H
#define INCLUDED_DNC_RELAY_NODE_CONTROLLER_IMPL_H

#include <DNC/relay_node_controller.h>

namespace gr {
  namespace DNC {

    class relay_node_controller_impl : public relay_node_controller
    {
     private:
	enum state_rx0
	{
		ST_RX0_IDLE,
		ST_RX0_CHECK_NODEA_ID,
		ST_RX0_CHECK_PACKET_NUMBER,
		ST_RX0_GET_DATA,
		ST_RX0_CHECK_END_PACKET
	};
	enum state_rx1
	{
		ST_RX1_IDLE,
		ST_RX1_CHECK_NODEB_ID,
		ST_RX1_CHECK_PACKET_NUMBER,
		ST_RX1_GET_DATA,
		ST_RX1_CHECK_END_PACKET
	};
	enum state_tx
	{
		ST_TX_IDLE,
		ST_TX_CONFIRM_TRANS,
		ST_TX_NODE_ID_TRANS,
		ST_TX_PACKET_NO_TRANS,
		ST_TX_XOR_DATA_TRANS,
		ST_TX_GUARD_TRANS,
		ST_TX_CHECK_DATA_OUT_BUFFER
	};
	state_rx0 d_rx0_state;
	state_rx1 d_rx1_state;
	state_tx d_tx_state;

	int d_packet_size;
	int d_guard_interval;
	int d_end_nodeA_id;
	int d_end_nodeB_id;
	int d_relay_node_id;

	int d_confirm_index;
	int d_buffer_size;
	int d_check_nodeA_id_count;
	int d_check_nodeA_id_index;
	int d_check_nodeB_id_count;
	int d_check_nodeB_id_index;
	int d_rx0_packet_number_index;
	int d_rx1_packet_number_index;
	int d_rx0_data_index;
	int d_rx1_data_index;
	int d_packet_out_index;
	int d_node_id_out_idx;
	int d_packet_no_out_idx;
	int d_xor_data_out_idx;
	int d_guard_idx;
	int d_rx0_check_end_packet_count;
	int d_rx0_check_end_packet_index;
	int d_rx1_check_end_packet_count;
	int d_rx1_check_end_packet_index;
	bool d_check_rx0_req;
	bool d_check_rx1_req;
	unsigned char d_rx0_pkt_no;
	unsigned char d_rx1_pkt_no;
	unsigned char d_confirm_req[6];
	unsigned char d_rx0_packet_number[3];
	unsigned char d_rx1_packet_number[3];

	std::vector<unsigned char> d_rx0_packet;
	std::vector<unsigned char> d_rx1_packet;
	std::vector<unsigned char> d_range_packet_no_nodeA;
	std::vector<unsigned char> d_range_packet_no_nodeB;
	std::vector<unsigned char> d_packet_no_nodeA;
	std::vector<unsigned char> d_packet_no_nodeB;
	std::vector<unsigned char> d_xored_packet_no;
	std::vector<unsigned char> d_packet_nodeA;
	std::vector<unsigned char> d_packet_nodeB;
	std::vector<unsigned char> d_xored_packet;
	std::vector<unsigned char> d_packet_no_out;
	std::vector<unsigned char> d_packet_out;
	void set_data_out(int xored_pos, unsigned char xored_pkt_no);
	void shift_xored_data_buffer();
     public:
      relay_node_controller_impl(int packet_size, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id);
      ~relay_node_controller_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_RELAY_NODE_CONTROLLER_IMPL_H */

