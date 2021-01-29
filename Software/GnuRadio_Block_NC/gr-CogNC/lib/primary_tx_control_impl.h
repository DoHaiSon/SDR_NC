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

#ifndef INCLUDED_COGNC_PRIMARY_TX_CONTROL_IMPL_H
#define INCLUDED_COGNC_PRIMARY_TX_CONTROL_IMPL_H

#include <CogNC/primary_tx_control.h>

namespace gr {
  namespace CogNC {

    class primary_tx_control_impl : public primary_tx_control
    {
     private:
      enum state_tx
	{
		ST_WAIT,
		ST_BUFFER_MANAGEMENT,
		ST_TRANSMITTER_ID_TRANS,
		ST_PACKET_NO_TRANS,
		ST_DATA_TRANS,
		ST_GUARD_INTERVAL_TRANS
	};
	enum state_rx
	{
		ST_RECEIVE,
		//ST_CHECK_RECEIVER_ID,
		//ST_CHECK_PACKET_NO_CONFIRM,
		ST_CHECK_SESSION_NUMBER
	};
	state_tx d_tx_state;
	state_rx d_rx_state;

	int d_packet_size;
	int d_guard_interval;
	int d_transmitter_id;
	int d_receiver_id;
	int d_ack_burst_size;
	int d_channel_size;

	int d_buffer_size;
	int d_load_packet_index;
	int d_load_data_index;
	int d_number_of_session_packets;
	unsigned char d_load_packet_number;
	bool d_load_data;
	bool d_loaded_data;
	int d_tx_buffer_index;
	int d_header_index;
	int d_data_index;
	int d_guard_index;
	int d_check_rx_id_count;
	int d_check_rx_id_index;
	int d_check_recv_packet_number_index;
	unsigned char d_expected_session_number;
	int d_check_session_index;
	int d_check_session_count;
	bool d_channel_buffer_full;

	std::vector<unsigned char> d_transmitted_data_buffer;
	std::vector<unsigned char> d_loaded_packet_number;
	std::vector<unsigned char> d_transmitted_packet_number;
	std::vector<unsigned char> d_confirm_packet_number;
	std::vector<gr_complex> d_channel_buffer;
	unsigned char d_rx_packet_number[3];
	void reset();

     public:
      primary_tx_control_impl(int packet_sz, int guard_interval, unsigned char source_id, unsigned char destination_id, int ack_burst_size, int channel_size);
      ~primary_tx_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items_v,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_PRIMARY_TX_CONTROL_IMPL_H */

