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

#ifndef INCLUDED_COGNC_PRIMARY_SOURCE_CONTROL_IMPL_H
#define INCLUDED_COGNC_PRIMARY_SOURCE_CONTROL_IMPL_H

#include <CogNC/primary_source_control.h>

namespace gr {
  namespace CogNC {

    class primary_source_control_impl : public primary_source_control
    {
     private:
     	enum state_t
	{
		ST_RECEIVE,
		ST_GET_ORIGIN_DATA,
		ST_TRANSMIT
		//ST_SOURCE_ID_TRANS,
		//ST_PACKET_NUMBER_TRANS,
		//ST_DATA_TRANS
	};
	state_t d_state;

	int d_packet_size;
	unsigned char d_source_id;
	unsigned char d_destination_id;
	unsigned char d_beacon_id;
	int d_ack_burst_size;
	int d_channel_size;
	int count;

	int d_check_destination_id_count;
	int d_check_destination_id_index;
	unsigned char d_next_packet_number;
	unsigned char d_trans_packet_number;
	int d_check_index;
	int d_beacon_count;
	int d_check_next_packet_number_count;
	int d_check_last_packet_number_count;
	bool d_check_recv_beacon;
	bool d_check_recv_ack;
	bool d_check_recv_this_ack;
	int d_origin_data_index;
	int d_header_index;
	int d_packet_number_count;
	int d_packet_number_index;
	int d_data_trans_index;
	int d_xored_data_index;
	int d_data_out_index;
	std::vector<unsigned char> d_temp_data;
	std::vector<unsigned char> d_origin_data;
	std::vector<unsigned char> d_xored_data;
	std::vector<unsigned char> d_data_out;
	std::vector<gr_complex> d_channel_buffer;

     public:
      primary_source_control_impl(int packet_size, unsigned char source_id, unsigned char destination_id, int ack_burst_size, int channel_size);
      ~primary_source_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_PRIMARY_SOURCE_CONTROL_IMPL_H */

