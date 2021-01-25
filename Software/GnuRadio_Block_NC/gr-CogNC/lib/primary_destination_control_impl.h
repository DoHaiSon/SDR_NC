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

#ifndef INCLUDED_COGNC_PRIMARY_DESTINATION_CONTROL_IMPL_H
#define INCLUDED_COGNC_PRIMARY_DESTINATION_CONTROL_IMPL_H

#include <CogNC/primary_destination_control.h>

namespace gr {
  namespace CogNC {

    class primary_destination_control_impl : public primary_destination_control
    {
     private:
      	enum state_t0
	{
		ST_BEACON_TRANS,
		ST_WAIT,
		ST_CHECK_ID,
		ST_CHECK_PACKET_NO,
		ST_DATA_OUT,
		ST_ACK_TRANS,
	};
	enum state_t1
	{
		ST_DO_BEACON_TRANS,
		ST_DO_XORED_DATA_TRANS
	};
	state_t0 d_state;
	state_t1 d_state_trans;

	int d_packet_size;
	unsigned char d_source_id;
	unsigned char d_destination_id;
	int d_ack_burst_size;
	int d_time_out;

	clock_t d_clock;
	float d_clock_diff;
	int d_beacon_index;
	
	int d_check_id_index;
	int d_check_source_id_count;
	int d_check_destination_id_count;
	unsigned char d_next_packet_number;
	unsigned char d_trans_packet_number;
	unsigned char d_beacon_id;
	int d_packet_no;
	int d_req_data_retrans_index;
	int d_header_index;
	int d_packet_number_index;
	int d_packet_number_count;
	int d_data_index;
	bool d_recved_data;
	bool d_reach_packet_number;
	bool d_reach_data;
	bool d_beacon_trans;
	std::vector<unsigned char> d_data;
      public:
      primary_destination_control_impl(int packet_size, unsigned char source_id, unsigned char destination_id, int time_out);
      ~primary_destination_control_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace CogNC
} // namespace gr

#endif /* INCLUDED_COGNC_PRIMARY_DESTINATION_CONTROL_IMPL_H */

