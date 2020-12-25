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
#include "secondary_endnode_control_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    secondary_endnode_control::sptr
    secondary_endnode_control::make(int packet_size, int buffer_size, int guard_interval_downlink, int guard_interval, unsigned char end_node_id, unsigned char relay_id)
    {
      return gnuradio::get_initial_sptr
        (new secondary_endnode_control_impl(packet_size, buffer_size, guard_interval_downlink, guard_interval, end_node_id, relay_id));
    }

    /*
     * The private constructor
     */
    secondary_endnode_control_impl::secondary_endnode_control_impl(int packet_size, int buffer_size, int guard_interval_downlink, int guard_interval, unsigned char end_node_id, unsigned char relay_id)
      : gr::block("secondary_endnode_control",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
	      d_packet_size(packet_size), d_guard_interval(guard_interval), d_end_node_id(end_node_id), d_relay_node_id(relay_id), d_GI_downlink(guard_interval_downlink),
	      d_buffer_size(buffer_size), d_next_out_packet_no(0x01), d_number_of_session_packets(0), d_session_no(0x01),
	      d_session_trans_index(0), d_check_session_index(0), d_check_session_count(0), d_request_index(0), d_check_rx_id_index(0), 	      d_check_rx_id_count(0), d_tx_buffer_index(0), d_load_data_index(0), d_check_confirm(false), d_load_data(false), 	
	      d_load_packet_number(0x01), d_trans_header_index(0), d_trans_data_index(0), d_guard_index(0), 
	      d_packets_out_index(0), d_data_out_index(0), d_xor_rx_pkt_no(0x00), d_rx_data_xored_index(0), 
	      d_check_recv_packet_number_index(0), d_data_out(false), d_zero_trans_index(0), 
	      d_check_end_packet_index(0), d_check_end_packet_count(0), d_number_of_packet_out(0), d_load_packet_index(0),
	      d_tx_state(ST_REQUEST_TRANS), d_rx_state(ST_IDLE)
    {
	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please make guard interval be greater than or equal to 6...!\n");

	if (end_node_id==0x00)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from 0...!!!\n");

	if (relay_id==0x00)
		throw std::runtime_error("Invalid parameter! Relay Node ID must be different from 0...!!!\n");

	if (end_node_id==relay_id)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from Relay Node ID...!!!\n");

	d_loaded_packet_number.resize(d_buffer_size);
	std::fill(d_loaded_packet_number.begin(), d_loaded_packet_number.end(), 0x00);

	d_transmitted_packet_number.resize(d_buffer_size);
	std::fill(d_transmitted_packet_number.begin(), d_transmitted_packet_number.end(), 0x00);

	d_transmitted_data_buffer.resize(d_buffer_size*packet_size);
	std::fill(d_transmitted_data_buffer.begin(), d_transmitted_data_buffer.end(), 0x00);

	d_received_packet_number.resize(d_buffer_size);
	std::fill(d_received_packet_number.begin(), d_received_packet_number.end(), 0x00);

	d_received_data_buffer.resize(d_buffer_size*packet_size);
	std::fill(d_received_data_buffer.begin(), d_received_data_buffer.end(), 0x00);

	d_rx_xored_packet.resize(packet_size);
	std::fill(d_rx_xored_packet.begin(), d_rx_xored_packet.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_request[i] = end_node_id;
		d_request[i+3] = 0x00;
		d_rx_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    secondary_endnode_control_impl::~secondary_endnode_control_impl()
    {
    }

    void
    secondary_endnode_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    void
    secondary_endnode_control_impl::reset()
    {
	d_request_index = 0;
	d_tx_buffer_index= 0;
	d_check_rx_id_index = 0;
	d_check_rx_id_count = 0;
	d_trans_header_index = 0;
	d_trans_data_index = 0;
	d_guard_index = 0;
	d_check_session_index = 0;
	d_check_session_count = 0;
	d_rx_data_xored_index = 0;
	d_check_recv_packet_number_index = 0;
	d_zero_trans_index = 0;
	d_check_end_packet_index = 0;
	d_check_end_packet_count = 0;
	d_session_trans_index = 0;
    }

    int
    secondary_endnode_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
	int nOutputItems = noutput_items;

        const unsigned char *in0 = (const unsigned char *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];
        unsigned char *out0 = (unsigned char *) output_items[0];
	unsigned char *out1 = (unsigned char *) output_items[1];

	int ni0 = 0;
	int ni1 = 0;
	int no = 0;
	
	while(ni0<nInputItems0)
	{
	/* Loading native packets */
	if(d_load_data==true)
	{
		/*if(nInputItems1==0)
		{			
			d_number_of_session_packets = d_load_packet_index;
			std::cout<<"end data"<<std::endl;
			std::cout<<"d_number_of_session_packets = "<<d_number_of_session_packets<<"\n";
			d_load_packet_number = 0x01;
			d_load_packet_index = 0;
			d_load_data_index = 0;
			d_load_data = false;
		}
		else
		{*/
			
		if(ni1<nInputItems1)
		{
			if(d_load_data_index==0)
			{
				if(in1[ni1]==0x01)
				{
					std::cout<<"end data"<<std::endl;
					std::cout<<"d_number_of_session_packets = "<<d_number_of_session_packets<<"\n";
					d_load_packet_number = 0x01;
					d_load_packet_index = 0;
					d_load_data_index = 0;
					d_load_data = false;
				}
				d_load_data_index++;
				ni1++;
			}
			else
			{
				d_transmitted_data_buffer[d_load_packet_index*d_packet_size+d_load_data_index-1] = in1[ni1];
				d_load_data_index++;
				ni1++;
				if(d_load_data_index==(d_packet_size+1))
				{
					d_loaded_packet_number[d_load_packet_index] = d_load_packet_number;
					d_number_of_session_packets++;
					d_load_packet_index++;
					d_load_packet_number++;
					d_load_data_index = 0;
					if(d_load_packet_index==d_buffer_size)
					{
						d_load_packet_number = 0x01;
						d_load_packet_index = 0;
						std::cout<<"d_number_of_session_packets = "<<d_number_of_session_packets<<"\n";
						d_load_data = false;
					}

				}
			}
		}
		else
		{
			consume (0, ni0);
			consume (1, ni1);
			return no;
		}
	}
	else
	{
		/* Receiving states */
		switch(d_rx_state)
		{
		case ST_IDLE:
		{
			if(in0[ni0]==d_relay_node_id)
			{
				d_rx_state = ST_CHECK_RELAY_NODE_ID;
				d_check_rx_id_index++;
				d_check_rx_id_count++;
			}
			ni0++;
			break;
		}
		case ST_CHECK_RELAY_NODE_ID:
		{
			if(in0[ni0]==d_relay_node_id)
			{
				d_check_rx_id_count++;
			}
			d_check_rx_id_index++;
			ni0++;
			if(d_check_rx_id_index==3)
			{
				if(d_check_rx_id_count==3)
				{
					d_rx_state = ST_CHECK_PACKET_NUMBER;
				}
				else
				{
					d_rx_state = ST_IDLE;
					ni0 = ni0 - 1;
				}
				d_check_rx_id_index = 0;
				d_check_rx_id_count = 0;
			}
			break;
		}
		case ST_CHECK_PACKET_NUMBER:
		{
			d_rx_packet_number[d_check_recv_packet_number_index] = in0[ni0];
			d_check_recv_packet_number_index++;
			ni0++;
			if(d_check_recv_packet_number_index==3)
			{
				d_check_recv_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx_packet_number[0];
				b = d_rx_packet_number[1];
				c = d_rx_packet_number[2];
				//if(a==b&&b==c) { rx_pkt_no = a; recv_pkt = true;}
				
				if(a==b) { rx_pkt_no = a; recv_pkt = true;}
				else 
				{
					if(b==c) { rx_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) { rx_pkt_no = c; recv_pkt = true;}
					}
				}
				if(recv_pkt == true)
				{
					//std::cout<<"received packet no "<<(int) rx_pkt_no<<"\n";
					if(rx_pkt_no==0x00&&d_check_confirm==false)
					{
						d_rx_state = ST_CHECK_SESSION_NUMBER;
					}
					else
					{
						if(rx_pkt_no!=0x00&&d_check_confirm == true)
						{
							d_xor_rx_pkt_no = rx_pkt_no;
							//std::cout<<"find out packet number: "<<(int) rx_pkt_no<<std::endl;
							d_rx_state = ST_GET_DATA;
						}
						else
						{
							d_rx_state = ST_IDLE;
						}
					}
				}
				else
				{
					d_rx_state = ST_IDLE;
				}
			}
			break;
		}
		case ST_CHECK_SESSION_NUMBER:
		{
			if(in0[ni0]==d_session_no)
			{
				d_check_session_count++;
			}
			d_check_session_index++;
			ni0++;
			if(d_check_session_index==6)
			{
				if(d_check_session_count==6)
				{
					std::cout<<"Get confirm\n";
					d_number_of_session_packets = 0;
					d_check_confirm = true;
					d_load_data = true;
				}
				//ni0+=48;
				d_rx_state = ST_IDLE;
				d_check_session_count = 0;
				d_check_session_index = 0;
			}
			break;
		}
		case ST_GET_DATA:
		{
			d_rx_xored_packet[d_rx_data_xored_index] = in0[ni0];
			d_rx_data_xored_index++;
			ni0++;
			if(d_rx_data_xored_index==d_packet_size)
			{
				d_rx_data_xored_index = 0;
				d_rx_state = ST_CHECK_END_PACKET;
			}
			break;
		}
		case ST_CHECK_END_PACKET:
		{
			if (d_GI_downlink == 1)
			{			
				if(in0[ni0]==0x00)
				{
					d_check_end_packet_count++;
				}
				d_check_end_packet_index++;
				ni0++;
			}
			if(d_check_end_packet_index==6 || d_GI_downlink == 0)
			{
				if(d_check_end_packet_count>4 || d_GI_downlink == 0)
				{
					int idx = (int)d_xor_rx_pkt_no - 1;
					if(d_received_packet_number[idx]==0x00&&d_transmitted_packet_number[idx]==d_xor_rx_pkt_no)
					{
						std::cout<<"recv packet number "<<(int)d_xor_rx_pkt_no<<"\n";
						for(int i = 0; i<d_packet_size; i++)
						{
							int des = idx*d_packet_size + i;
							d_received_data_buffer[des] = d_transmitted_data_buffer[des]^d_rx_xored_packet[i];
						}
						d_received_packet_number[idx] = d_xor_rx_pkt_no;
					}
				}
				d_check_end_packet_index = 0;
				d_check_end_packet_count = 0;
				d_rx_state = ST_IDLE;
			}
			break;
		}
		}
	}
	}
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	while(no<nOutputItems)
	{
		if(d_data_out==true)
		{
			if(d_data_out_index<=d_packet_size)
			{
				if(d_data_out_index==0)
				{
					out1[no] = d_end_node_id;
					d_data_out_index++;
				}
				else
				{
						int des = ((int)d_next_out_packet_no - 1)*d_packet_size+d_data_out_index-1;
						out1[no] = d_received_data_buffer[des];
						d_data_out_index++;
				}
			}
			else
			{
				d_next_out_packet_no++;
				d_number_of_packet_out++;
				d_data_out_index = 0;
				d_data_out = false;
				if(d_number_of_packet_out==d_number_of_session_packets)
				{
					if(d_session_no==0xFF)
					{
						d_session_no = 0x01;
					}
					else
					{
						d_session_no++;
					}
					d_number_of_packet_out = 0;
					d_next_out_packet_no = 0x01;
					if(d_number_of_session_packets==d_buffer_size)
					{
						for(int i = 0; i<d_buffer_size; i++)
						{
							d_loaded_packet_number[i] = 0x00;
							d_transmitted_packet_number[i] = 0x00;
							d_received_packet_number[i] = 0x00;
						}
						d_check_confirm = false;
						d_load_data = false;
						d_tx_state = ST_REQUEST_TRANS;
						d_rx_state = ST_IDLE;
						reset();
					}
					else
					{
						//std::cout<<"ST_ZERO_TRANS\n";
						d_tx_state = ST_ZERO_TRANS;
					}
				}
			}
		}
		else
		{
			out1[no] = 0x00;
		}
		switch(d_tx_state)
		{
		case ST_REQUEST_TRANS:
		{
			//std::cout<<"ST_REQUEST_TRANS\n";
			out0[no] = d_request[d_request_index];
			d_request_index++;
			no++;
			if(d_request_index==6)
			{
				d_request_index = 0;
				if(d_check_confirm==true)
				{
					d_tx_state = ST_BUFFER_MANAGEMENT;
				}
				else
				{
					d_tx_state = ST_SESSION_NUMBER_TRANS;
				}
			}
			break;
		}
		case ST_SESSION_NUMBER_TRANS:
		{
			out0[no] = d_session_no;
			d_session_trans_index++;
			no++;
			if(d_session_trans_index==6)
			{
				d_tx_state = ST_REQUEST_TRANS;
				d_session_trans_index = 0;
			}
			break;
		}
		case ST_BUFFER_MANAGEMENT:
		{
			//std::cout<<"ST_BUFFER_MANAGEMNET\n";
			int idx = (int)d_next_out_packet_no - 1;
			if(d_received_packet_number[idx]==d_next_out_packet_no && d_data_out==false)
			{
				d_data_out = true;
			}
			if(d_received_packet_number[d_tx_buffer_index]==0x00)
			{
				d_tx_state = ST_HEADER_TRANS;
			}
			else
			{
				d_tx_buffer_index++;
				if(d_tx_buffer_index==d_number_of_session_packets)
				{
					d_tx_buffer_index = 0;
				}
			}
			out0[no] = 0x00;
			no++;
			break;
		}
		case ST_HEADER_TRANS:
		{
			//std::cout<<"ST_HEADER_TRANS\n";
			if(d_trans_header_index<3)
			{
				out0[no] = d_end_node_id;
				d_trans_header_index++;
				no++;
				break;
			}
			if(d_trans_header_index>=3&&d_trans_header_index<6)
			{
				out0[no] = d_loaded_packet_number[d_tx_buffer_index];
				d_transmitted_packet_number[d_tx_buffer_index] = d_loaded_packet_number[d_tx_buffer_index];
				d_trans_header_index++;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					d_tx_state = ST_DATA_TRANS;
				}
			}
			break;
		}
		case ST_DATA_TRANS:
		{
			//std::cout<<"ST_DATA_TRANS\n";
			out0[no] = d_transmitted_data_buffer[d_tx_buffer_index*d_packet_size+d_trans_data_index];
			no++;
			d_trans_data_index++;
			if(d_trans_data_index==d_packet_size)
			{
				d_trans_data_index = 0;
				d_tx_state = ST_GUARD_INTERVAL_TRANS;
			}
			break;
		}
		case ST_GUARD_INTERVAL_TRANS:
		{
			//std::cout<<"ST_GUARD_INTERVAL_TRANS\n";
			out0[no] = 0x00;
			d_guard_index++;
			no++;
			if(d_guard_index==d_guard_interval)
			{
				d_guard_index = 0;
				d_tx_buffer_index++;
				if(d_tx_buffer_index==d_number_of_session_packets)
				{
					d_tx_buffer_index = 0;
				}
				d_tx_state = ST_BUFFER_MANAGEMENT;
			}
			break;
		}
		case ST_ZERO_TRANS:
		{
			out0[no] = 0x00;
			no++;
			d_zero_trans_index++;
			if(d_zero_trans_index==(d_buffer_size*d_packet_size))
			{
				d_tx_state = ST_SLEEP;
			}
		}
		case ST_SLEEP:
		{
			/* just do nothing */
			break;
		}
		}
	}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume (0, ni0);
        consume (1, ni1);

        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace CogNC */
} /* namespace gr */

