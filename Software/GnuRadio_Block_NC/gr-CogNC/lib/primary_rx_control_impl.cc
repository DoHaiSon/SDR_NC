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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "primary_rx_control_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    primary_rx_control::sptr
    primary_rx_control::make(int packet_sz, unsigned char source_id, unsigned char destination_id)
    {
      return gnuradio::get_initial_sptr
        (new primary_rx_control_impl(packet_sz, source_id, destination_id));
    }

    /*
     * The private constructor
     */
    primary_rx_control_impl::primary_rx_control_impl(int packet_sz, unsigned char source_id, unsigned char destination_id)
      : gr::block("primary_rx_control",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
	     d_packet_size(packet_sz), d_transmitter_id(source_id), d_receiver_id(destination_id), 
	      d_buffer_size(255), d_curr_part(0), d_check_transmitter_id_count(0), d_check_transmitter_id_index(0),
	      d_check_recv_packet_number_index(0), d_rx_pkt_no(0x00), d_data_index(0), d_check_end_packet_count(0),
	      d_check_end_packet_index(0), d_next_out_packet_no(0x01), d_confirm_index(0), d_count_recvd_packet(0),
	      d_data_out_index(0), d_pkt_no_confirm_index(0), d_recv_packet(false), d_header_index(0), d_session_request_index(0),
	      d_session_no(0x01), d_session_request_trans(false), d_data_out(false), d_data_out_full(false),
	      d_rx_state(ST_IDLE), d_tx_state(ST_HEADER_TRANS)
    {
	if (source_id==0x00)
		throw std::runtime_error("Invalid parameter! source ID must be different from 0...!!!\n");
	if (destination_id==0x00)
		throw std::runtime_error("Invalid parameter! destination ID must be different from 0...!!!\n");

	d_received_data_buffer.resize(d_buffer_size*d_packet_size);
	std::fill(d_received_data_buffer.begin(), d_received_data_buffer.end(), 0x00);

	d_received_packet_number_buffer.resize(d_buffer_size);
	std::fill(d_received_packet_number_buffer.begin(), d_received_packet_number_buffer.end(), 0x00);

	d_rx_packet.resize(d_packet_size);
	std::fill(d_rx_packet.begin(), d_rx_packet.end(), 0x00);
	
	for(int i = 0; i<3; i++)
	{
		d_rx_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    primary_rx_control_impl::~primary_rx_control_impl()
    {
    }

    void
    primary_rx_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    primary_rx_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems = ninput_items_v[0];
	int nOutputItems = noutput_items;

        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out0 = (unsigned char *) output_items[0];
        unsigned char *out1 = (unsigned char *) output_items[1];

	int ni = 0;
	int no = 0;
	while(ni<nInputItems)
	{
		switch(d_rx_state)
		{
		case ST_IDLE:
		{
			if(in[ni]==d_transmitter_id)
			{
				d_rx_state = ST_CHECK_TRANSMITTER_ID;
				d_check_transmitter_id_index++;
				d_check_transmitter_id_count++;
			}
			ni++;
			break;
		}
		case ST_CHECK_TRANSMITTER_ID:
		{
			if(in[ni]==d_transmitter_id)
			{
				d_check_transmitter_id_count++;
			}
			d_check_transmitter_id_index++;
			ni++;
			if(d_check_transmitter_id_index==3)
			{
				if(d_check_transmitter_id_count==3)
				{
					d_rx_state = ST_CHECK_PACKET_NO;
				}
				else
				{
					ni = ni - 1;
					d_rx_state = ST_IDLE;
				}
				d_check_transmitter_id_index = 0;
				d_check_transmitter_id_count = 0;
			}
			break;
		}
		case ST_CHECK_PACKET_NO:
		{
			d_rx_packet_number[d_check_recv_packet_number_index] = in[ni];
			d_check_recv_packet_number_index++;
			ni++;
			if(d_check_recv_packet_number_index==3)
			{
				d_check_recv_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx_packet_number[0];
				b = d_rx_packet_number[1];
				c = d_rx_packet_number[2];
				//if(a==b&&b==c) { rx_pkt_no = a; recv_pkt = true; }
				if(a==b) { rx_pkt_no = a; recv_pkt = true;}
				else 
				{
					if(b==c) { rx_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) { rx_pkt_no = c; recv_pkt = true;}
					}
				}
				if(recv_pkt == true&&d_session_request_trans==true)
				{
					if(rx_pkt_no!=0x00)
					{
						d_rx_pkt_no = rx_pkt_no;
						d_rx_state = ST_GET_DATA;
					}
					else
					{
						d_rx_state = ST_IDLE;
					}
				}
				else
				{
					d_rx_state = ST_IDLE;
				}
			}
			break;
		}
		case ST_GET_DATA:
		{
			d_rx_packet[d_data_index] = in[ni];
			ni++;
			d_data_index++;
			if(d_data_index==d_packet_size)
			{
				d_rx_state = ST_CHECK_END_PACKET;
				d_data_index = 0;
			}
			break;
		}
		case ST_CHECK_END_PACKET:
		{
			if(in[ni]==0x00)
			{
				d_check_end_packet_count++;
			}
			d_check_end_packet_index++;
			ni++;
			if(d_check_end_packet_index==6)
			{
				if(d_check_end_packet_count>4)
				{
					int idx = (int)d_rx_pkt_no - 1;
					if(d_received_packet_number_buffer[idx]==0x00)
					{
						d_recv_packet = true;
						d_received_packet_number_buffer[idx] = d_rx_pkt_no;
						d_count_recvd_packet++;
						//std::cout<<d_count_recvd_packet<<", ";
						std::cout<<"idx = "<<idx+1<<"\n";
						std::cout<<"\nreceived "<<d_count_recvd_packet<<" packets \n";
						int rcv_data_idx = 0;
						for(int k = 0; k<d_packet_size; k++)
						{
							rcv_data_idx = idx*d_packet_size+k;
							d_received_data_buffer[rcv_data_idx] = d_rx_packet[k];
						}
					}
				}
				d_rx_state = ST_IDLE;
				d_check_end_packet_count = 0;
				d_check_end_packet_index = 0;				
			}
			break;
		}
		}
	}
	while(no<nOutputItems)
	{
		if(d_data_out==true)
		{
			if(d_data_out_index==0)
			{
				out1[no] = d_receiver_id;
				d_data_out_index++;
			}
			else
			{
				int pkt_out_idx = ((int)d_next_out_packet_no - 1)*d_packet_size+d_data_out_index-1;
				out1[no] = d_received_data_buffer[pkt_out_idx];
				d_data_out_index++;
				if(d_data_out_index==(d_packet_size+1))
				{
					d_data_out_index = 0;
					if(d_next_out_packet_no==0xFF)
					{
						d_next_out_packet_no = 0x01;
						d_data_out_full = true;
					}
					else
					{
						d_next_out_packet_no++;
					}
					d_data_out = false;
				}
			}
		}
		else
		{
			out1[no] = 0x00;
		}
		switch(d_tx_state)
		{
		case ST_HEADER_TRANS:
		{
			if(d_header_index<3)
			{
				out0[no] = d_receiver_id;
				d_header_index++;
				no++;
				break;
			}
			if(d_header_index>=3&&d_header_index<6)
			{
				out0[no] = 0x00;
				d_header_index++;
				no++;
				if(d_header_index==6)
				{
					d_header_index = 0;
					d_tx_state = ST_SESSION_REQUEST_TRANS;
				}
				break;
			}
		}
		case ST_SESSION_REQUEST_TRANS:
		{
			out0[no] = d_session_no;
			d_session_request_index++;
			no++;
			if(d_session_request_index==6)
			{
				d_session_request_index = 0;
				d_session_request_trans = true;
				if(d_recv_packet==true)
				{
					d_tx_state = ST_CHECK_PACKET;
					d_pkt_no_confirm_index = 0;
				}
				else
				{
					d_tx_state = ST_HEADER_TRANS;
				}
			}
			break;
		}
		case ST_CHECK_PACKET:
		{
			out0[no] = 0x00;
			no++;
			if(d_data_out_full==true)
			{
				d_data_out_full=false;
				d_recv_packet = false;
				d_session_request_trans = false;
				d_tx_state = ST_HEADER_TRANS;
				d_count_recvd_packet = 0;
				if(d_session_no==0xFF)
				{
					d_session_no = 0x01;
				}
				else
				{
					d_session_no++;
				}
				for(int k = 0; k<d_buffer_size; k++)
				{
					d_received_packet_number_buffer[k] = 0x00;
				}
				break;
			}
			int pkt_out_idx = (int)d_next_out_packet_no - 1;
			if(d_received_packet_number_buffer[pkt_out_idx]==d_next_out_packet_no&&d_data_out==false)
			{
				std::cout<<"next out packet number = "<<(int)d_next_out_packet_no<<"\n";
				d_data_out = true;
			}
			if(d_received_packet_number_buffer[d_pkt_no_confirm_index]!=0x00)
			{
				d_tx_state = ST_CONFIRM_TRANS;
			}
			else
			{
				d_pkt_no_confirm_index++;
				if(d_pkt_no_confirm_index==d_buffer_size)
				{
					d_pkt_no_confirm_index = 0;
				}
			}
			break;			
		}
		case ST_CONFIRM_TRANS:
		{
			if(d_confirm_index<3)
			{
				out0[no] = d_receiver_id;
				d_confirm_index++;
				no++;
			}
			else
			{
				out0[no] = d_received_packet_number_buffer[d_pkt_no_confirm_index];
				d_confirm_index++;
				no++;
				if(d_confirm_index==6)
				{
					d_confirm_index = 0;
					d_pkt_no_confirm_index++;
					if(d_pkt_no_confirm_index==d_buffer_size)
					{
						d_pkt_no_confirm_index = 0;
					}
					d_tx_state = ST_CHECK_PACKET;
				}
			}
			break;
		}
		}
	}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (ni);

        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace CogNC */
} /* namespace gr */

