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

#ifndef INCLUDED_COGNC_PRIMARY_RX_CONTROL_IMPL_H
#define INCLUDED_COGNC_PRIMARY_RX_CONTROL_IMPL_H

#include <CogNC/primary_rx_control.h>

namespace gr {
  namespace CogNC {

    class primary_rx_control_impl : public primary_rx_control
    {
     private:
      enum state_rx
	{
		ST_IDLE,
		ST_CHECK_TRANSMITTER_ID,
		ST_CHECK_PACKET_NO,
		ST_GET_DATA,
		ST_CHECK_END_PACKET
	};
	enum state_tx
	{
		ST_HEADER_TRANS,
		ST_SESSION_REQUEST_TRANS,
		ST_CHECK_PACKET,
		ST_CONFIRM_TRANS,
		ST_CHECK_DATA_OUT,
		ST_RECVED_DATA_OUT
	};
	state_tx d_tx_state;
	state_rx d_rx_state;

	int d_packet_size;
	unsigned char d_transmitter_id;
	unsigned char d_receiver_id;

	int d_buffer_size;
	int d_curr_part;
	int d_check_transmitter_id_index;
	int d_check_transmitter_id_count;
	int d_check_recv_packet_number_index;
	unsigned char d_rx_pkt_no;
	int d_data_index;
	int d_check_end_packet_count;
	int d_check_end_packet_index;
	unsigned char d_next_out_packet_no;
	int d_confirm_index;
	int d_pkt_no_confirm_index;
	int d_data_out_index;
	int d_count_recvd_packet;
	bool d_recv_packet;
	bool d_session_request_trans;
	int d_header_index;
	int d_session_request_index;
	unsigned char d_session_no;
	bool d_data_out;
	bool d_data_out_full;
	std::vector<unsigned char> d_received_data_buffer;
	std::vector<unsigned char> d_received_packet_number_buffer;
	std::vector<unsigned char> d_rx_packet;
	unsigned char d_rx_packet_number[3];


     public:
      primary_rx_control_impl(int packet_sz, unsigned char source_id, unsigned char destination_id);
      ~primary_rx_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_PRIMARY_RX_CONTROL_IMPL_H */

