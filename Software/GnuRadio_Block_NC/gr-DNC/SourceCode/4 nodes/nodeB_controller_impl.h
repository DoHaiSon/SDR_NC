
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_NODEB_CONTROLLER_IMPL_H
#define INCLUDED_DNC_NODEB_CONTROLLER_IMPL_H

#include <DNC/nodeB_controller.h>

namespace gr {
  namespace DNC {

    class nodeB_controller_impl : public nodeB_controller
    {
     private:
	enum state_tx
	{
		ST_BUFFER_MANAGEMENT,
		ST_HEADER_TRANS,
		ST_DATA_TRANS,
		ST_GUARD_INTERVAL_TRANS,
		ST_END_PACKET_HEADER_TRANS,
		ST_END_PACKET_NUMBER_TRANS
	};
	enum state_rx
	{
		ST_IDLE,
		ST_CHECK_NODE_ID,
		ST_CHECK_PACKET_NUMBER,
		ST_CHECK_SESSION_NUMBER
	};
	state_tx d_tx_state;
	state_rx d_rx_state;

	int d_buffer_size;
	int d_packet_size;
	int d_guard_interval;
	unsigned char d_nodeR_id;
	unsigned char d_nodeB_id;

	unsigned char d_node_id_check;
	unsigned char d_session_node_id;
	int d_check_rx_id_index;
	int d_check_rx_id_count;
	int d_check_recv_packet_number_index;

	unsigned char d_next_session_number;
	int d_check_session_count;
	int d_check_session_index;
	bool d_send;

	bool d_load_data;
	int d_load_data_index;
	unsigned char d_load_packet_number;
	unsigned char d_ended_packet_number;
	int d_load_packet_index;
	int d_number_of_session_packets;

	int d_tx_buffer_index;
	int d_trans_header_index;
	int d_trans_data_index;
	int d_guard_index;
	int d_ended_packet_number_trans_index;

	std::vector<unsigned char> d_loaded_packet_number;
	std::vector<unsigned char> d_transmitted_packet_number;
	std::vector<unsigned char> d_transmitted_data_buffer;
	std::vector<unsigned char> d_confirm_packet_number;
	unsigned char d_rx_packet_number[3];
	void reset();

     public:
      nodeB_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeR_id, unsigned char nodeB_id);
      ~nodeB_controller_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_NODEB_CONTROLLER_IMPL_H */

