/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_COGNC_SECONDARY_ENDNODE_CONTROL_IMPL_H
#define INCLUDED_COGNC_SECONDARY_ENDNODE_CONTROL_IMPL_H

#include <CogNC/secondary_endnode_control.h>

namespace gr {
  namespace CogNC {

    class secondary_endnode_control_impl : public secondary_endnode_control
    {
     private:
      	enum state_tx
	{
		ST_REQUEST_TRANS,
		ST_SESSION_NUMBER_TRANS,
		ST_BUFFER_MANAGEMENT,
		ST_HEADER_TRANS,
		ST_DATA_TRANS,
		ST_GUARD_INTERVAL_TRANS,
		ST_ZERO_TRANS,
		ST_SLEEP
	};
	enum state_rx
	{
		ST_IDLE,
		ST_CHECK_RELAY_NODE_ID,
		ST_CHECK_PACKET_NUMBER,
		ST_CHECK_SESSION_NUMBER,
		ST_GET_DATA,
		ST_CHECK_CONFIRM_PACKET_NUMBER,
		ST_XOR_DATA,
		ST_CHECK_END_PACKET
	};
	state_tx d_tx_state;
	state_rx d_rx_state;

	int d_packet_size;
	int d_buffer_size;
	int d_GI_downlink;
	int d_guard_interval;
	unsigned char d_end_node_id;
	unsigned char d_relay_node_id;

	int d_load_packet_index;
	int d_next_out_packet_no;
	int d_number_of_packet_out;
	int d_number_of_session_packets;

	int d_request_index;
	int d_tx_buffer_index;
	int d_check_rx_id_index;
	int d_check_rx_id_count;
	bool d_check_confirm;
	bool d_load_data;
	int d_load_data_index;
	int d_trans_header_index;
	int d_trans_data_index;
	int d_guard_index;
	int d_check_session_index;
	int d_check_session_count;
	unsigned char d_load_packet_number;
	unsigned char d_session_no;
	bool d_data_out;
	int d_number_of_packets_out;
	int d_data_out_index;
	int d_packets_out_index;
	int d_rx_data_xored_index;
	unsigned char d_xor_rx_pkt_no;
	int d_check_recv_packet_number_index;
	int d_zero_trans_index;
	int d_check_end_packet_index;
	int d_check_end_packet_count;
	int d_session_trans_index;
	std::vector<unsigned char> d_loaded_packet_number;
	std::vector<unsigned char> d_transmitted_packet_number;
	std::vector<unsigned char> d_transmitted_data_buffer;
	std::vector<unsigned char> d_received_packet_number;
	std::vector<unsigned char> d_received_data_buffer;
	std::vector<unsigned char> d_rx_xored_packet;
	unsigned char d_request[6];
	unsigned char d_rx_packet_number[3];
	void reset();

     public:
      secondary_endnode_control_impl(int packet_size, int buffer_size, int guard_interval_downlink, int guard_interval, unsigned char end_node_id, unsigned char relay_id);
      ~secondary_endnode_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_SECONDARY_ENDNODE_CONTROL_IMPL_H */

