/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_SOURCE1_CONTROLLER_IMPL_H
#define INCLUDED_DNC_SOURCE1_CONTROLLER_IMPL_H

#include <DNC/source1_controller.h>

namespace gr {
  namespace DNC {

    class source1_controller_impl : public source1_controller
    {
     private:
	enum state_rx
	{
		ST_RX_IDLE,
		ST_RX_CHECK_NODE_ID,
		ST_RX_CHECK_PACKET_NUMBER,
		ST_RX_CHECK_NODE_REQUEST
	};
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
	state_tx d_tx_state;
	state_rx d_rx_state;

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

	bool d_transmit;
	bool d_load_data;
	unsigned char d_rx_node_id;
	int d_check_rx_node_id_index;
	int d_check_rx_node_id_count;
	unsigned char d_rx_packet_number[3];
	int d_check_rx_packet_number_index;
	unsigned char d_rx_node_request[3];
	int d_check_rx_node_request_index;

     public:
      source1_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char source1_id, unsigned char source2_id, unsigned char relay1_id, unsigned char relay2_id, unsigned char destination1_id, unsigned char destination2_id, int network_coding);
      ~source1_controller_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_SOURCE1_CONTROLLER_IMPL_H */

