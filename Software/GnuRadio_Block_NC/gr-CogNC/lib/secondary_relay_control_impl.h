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

#ifndef INCLUDED_COGNC_SECONDARY_RELAY_CONTROL_IMPL_H
#define INCLUDED_COGNC_SECONDARY_RELAY_CONTROL_IMPL_H

#include <CogNC/secondary_relay_control.h>

namespace gr {
  namespace CogNC {

    class secondary_relay_control_impl : public secondary_relay_control
    {
     private:
      	enum state_rx0
	{
		ST_RX0_IDLE,
		ST_RX0_CHECK_NODEA_ID,
		ST_RX0_CHECK_PACKET_NUMBER,
		ST_RX0_CHECK_SESSION_NUMBER,
		ST_RX0_GET_DATA,
		ST_RX0_CHECK_END_PACKET
	};
	enum state_rx1
	{
		ST_RX1_IDLE,
		ST_RX1_CHECK_NODEB_ID,
		ST_RX1_CHECK_PACKET_NUMBER,
		ST_RX1_CHECK_SESSION_NUMBER,
		ST_RX1_GET_DATA,
		ST_RX1_CHECK_END_PACKET
	};
	enum state_tx
	{
		ST_TX_IDLE,
		ST_TX_CONFIRM_TRANS,
		ST_TX_SESSION_NUMBER_TRANS,
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
	int d_buffer_size;
	int d_GI_downlink;
	int d_guard_interval;
	int d_end_nodeA_id;
	int d_end_nodeB_id;
	int d_relay_node_id;

	int d_confirm_index;
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
	bool d_confirm_sent;
	bool d_check_full_xored;
	bool d_check_out;
	int d_rx0_check_session_index;
	int d_rx0_check_session_count;
	int d_rx1_check_session_index;
	int d_rx1_check_session_count;
	unsigned char d_expected_session_no;
	unsigned char d_rx0_pkt_no;
	unsigned char d_rx1_pkt_no;
	unsigned char d_confirm_req[6];
	unsigned char d_rx0_packet_number[3];
	unsigned char d_rx1_packet_number[3];

	std::vector<unsigned char> d_rx0_packet;
	std::vector<unsigned char> d_rx1_packet;
	std::vector<unsigned char> d_packet_no_nodeA;
	std::vector<unsigned char> d_packet_no_nodeB;
	std::vector<unsigned char> d_xored_packet_no;
	std::vector<unsigned char> d_packet_nodeA;
	std::vector<unsigned char> d_packet_nodeB;
	std::vector<unsigned char> d_xored_packet;
	std::vector<unsigned char> d_packet_no_out;
	std::vector<unsigned char> d_packet_out;
	void set_data_out(int xored_pos,unsigned char xored_pkt_no);

     public:
      secondary_relay_control_impl(int packet_size, int buffer_size, int guard_interval_downlink, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_id);
      ~secondary_relay_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_SECONDARY_RELAY_CONTROL_IMPL_H */

