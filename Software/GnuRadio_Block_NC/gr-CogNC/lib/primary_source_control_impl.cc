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
#include "primary_source_control_impl.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <math.h>
#include <time.h>

namespace gr {
  namespace CogNC {

    primary_source_control::sptr
    primary_source_control::make(int packet_size, unsigned char source_id, unsigned char destination_id, int ack_burst_size, int channel_size)
    {
      return gnuradio::get_initial_sptr
        (new primary_source_control_impl(packet_size, source_id, destination_id, ack_burst_size, channel_size));
    }

    /*
     * The private constructor
     */
    primary_source_control_impl::primary_source_control_impl(int packet_size, unsigned char source_id, unsigned char destination_id, int ack_burst_size, int channel_size)
      : gr::block("primary_source_control",
              gr::io_signature::make3(3, 3, sizeof(unsigned char)*ack_burst_size, sizeof(gr_complex)*channel_size, sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char)*(packet_size+6), sizeof(gr_complex)*channel_size)),
    	      d_packet_size(packet_size), d_source_id(source_id), d_destination_id(destination_id), d_beacon_id(0x00), d_ack_burst_size(ack_burst_size), d_channel_size(channel_size),
	      d_check_index(0), d_beacon_count(0), d_check_next_packet_number_count(0), d_check_last_packet_number_count(0), d_check_recv_beacon(false), 
	      d_origin_data_index(0), d_header_index(0), d_data_trans_index(0), d_xored_data_index(0), d_data_out_index(0), 
	      d_next_packet_number(0x01), d_trans_packet_number(0x01), d_check_recv_ack(false), d_check_recv_this_ack(false), d_check_destination_id_count(0), d_check_destination_id_index(0),
	      d_packet_number_count(1), d_state(ST_RECEIVE), count(0)
    {
	d_origin_data.resize(d_packet_size);
	std::fill(d_origin_data.begin(), d_origin_data.end(), 0x00);
	d_data_out.resize(d_packet_size);
	std::fill(d_data_out.begin(), d_data_out.end(), 0x00);
	d_channel_buffer.resize(d_channel_size);
	std::fill(d_channel_buffer.begin(), d_channel_buffer.end(), 0x00);
    }

    /*
     * Our virtual destructor.
     */
    primary_source_control_impl::~primary_source_control_impl()
    {
    }

    void
    primary_source_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    primary_source_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
	int nInputItems1 = ninput_items[1];
	int nInputItems2 = ninput_items[2];
	int nOutputItems = noutput_items;
        const unsigned char *in_ack = (const unsigned char *) input_items[0];
        const gr_complex *in_h = (const gr_complex *) input_items[1];
        const unsigned char *in_data = (const unsigned char *) input_items[2];
        unsigned char *out = (unsigned char *) output_items[0];
	gr_complex *out_h = (gr_complex *) output_items[1];
	
	int ni0 = 0;
	int ni1 = 0;
	int ni2 = 0;
	int no = 0;
	
	int nidx_ack = 0;
	int nidx_h = 0;

	int nodx_h = 0;
	int nodx_data = 0;

	int i = 0;
	int j = 0;

	while(ni0<nInputItems0 && ni1<nInputItems1 && no<nOutputItems)
	{
		switch(d_state)
		{
		case ST_RECEIVE:
		{
			/* CHECK NODE ID */
			for (j=0; j<3; j++)
			{	
				if(in_ack[nidx_ack+j]==d_destination_id)
				{
					d_check_destination_id_count++;
				}
			}
			if(d_check_destination_id_count==3)
			{
				//printf("detect destination ID ");
				/*for (i = 0;i<9;i++)
				{
					std::cout<<(int)in_ack[nidx_ack - 3 + i]<<" ";
				}*/
				//std::cout<<"\n";
				/* CHECK PACKET NUMBER */
				for (i = 0; i < d_channel_size; i++)
				{
					d_channel_buffer[i] = in_h[nidx_h+i];
				}
				//d_channel_buffer_full = true;
				for (j=3; j<6; j++)
				{
					if(in_ack[nidx_ack+j]==d_beacon_id&&d_check_recv_ack==false)
					{
						d_beacon_count++;
					}
					if(in_ack[nidx_ack+j]==(d_next_packet_number-1))
					{
						d_check_last_packet_number_count++;
					}
					if(in_ack[nidx_ack+j]==d_next_packet_number)
					{
						d_check_next_packet_number_count++;
					}
				}
				if(d_check_next_packet_number_count>=2) // if we get ack packet
				{
					std::cout<<"recv ack number "<<(int) d_next_packet_number<<"\n";
					d_check_recv_ack = true;
					if(d_packet_number_count==255)
					{
						d_next_packet_number = 0x01;
						d_packet_number_count = 1;
					}
					else
					{
						d_next_packet_number++;
						d_packet_number_count++;
					}
					d_trans_packet_number = d_next_packet_number;
					d_state = ST_GET_ORIGIN_DATA;
					//std::cout<<"RECEIVE ----> LOAD DATA\n";
				}
				else
				{
					if(d_check_recv_ack==true && d_check_last_packet_number_count>=2)
					{
						std::cout<<"recv ack number "<<(int) d_next_packet_number-1<<"\n";
						d_state = ST_TRANSMIT;
						//std::cout<<"RECEIVE ----> PACKET TRANS\n";
					}
				}
				if(d_check_recv_ack==false)
				{
					if(d_beacon_count>=2) // if we get beacon
					{
						std::cout<<"recv beacon \n";
						if(d_check_recv_beacon==false)
						{
							d_state = ST_GET_ORIGIN_DATA;
							//std::cout<<"RECEIVE ----> LOAD DATA\n";
							d_check_recv_beacon = true;
						}
						else
						{
							d_state = ST_TRANSMIT;
							//std::cout<<"RECEIVE ----> PACKET TRANS\n";
						}
					} else 	{
						d_state = ST_RECEIVE;
					}
					d_beacon_count = 0;
				}
				d_check_next_packet_number_count = 0;
				d_check_last_packet_number_count = 0;
			} else 	{
				d_state = ST_RECEIVE;
			}
			d_check_destination_id_count = 0;
			nidx_ack += d_ack_burst_size;
			nidx_h += d_channel_size;
			ni0 ++;
			ni1 ++;
			break;
		}
		case ST_GET_ORIGIN_DATA:
		{
			if(d_origin_data_index<d_packet_size)
			{
				d_origin_data[d_origin_data_index] = in_data[ni2];
				//std::cout<<(int)d_origin_data[d_origin_data_index]<<" ";
				d_origin_data_index++;
				ni2++;
				if (d_origin_data_index == d_packet_size)
				{
					//std::cout<<"\n";
					//std::cout<<"Data loaded \n";
					d_origin_data_index = 0;
					d_state = ST_TRANSMIT;
					//std::cout<<"LOAD DATA ----> PACKET TRANS\n";
					//if (d_trans_packet_number > 0) 
						//std::cout<<"Packet "<<(int)d_trans_packet_number-1<<" transmitted "<<count<<" times \n";	
					count = 0;
				}
			}
			break;
		}
		case ST_TRANSMIT:
		{
			for (int i = 0;i<d_packet_size+6;i++)
			{
				if (i<3 && i>=0)
				{
					out[nodx_data+i] = d_source_id;
				}
				if (i<6 && i>=3)
				{
					out[nodx_data+i] = d_trans_packet_number;
				}
				if (i<d_packet_size+6 && i>=6)
				{
					out[nodx_data+i] = d_origin_data[i-6];
				}
			}
			for (int i = 0; i < d_channel_size; i++)
			{
				out_h[nodx_h+i] = d_channel_buffer[i];
				//printf("%.4f+i*%.4f, ", d_channel_buffer[i].real(), d_channel_buffer[i].imag());
			}
			nodx_data+= (d_packet_size+6);
			nodx_h+= d_channel_size;
			no++;
			std::cout<<"Packet Transmitted Number "<<(int) d_trans_packet_number<<"\n";
			count++;
			d_state = ST_RECEIVE;
			break;
		}
		/*case ST_SOURCE_ID_TRANS:
		{
			if(d_header_index<3)
			{
				out[no] = d_source_id;
				d_header_index++;
				for (i = 0; i < d_channel_size; i++)
				{
					out_h[nodx_h+i] = d_channel_buffer[i];
					//printf("%.4f+i*%.4f, ", d_channel_buffer[i].real(), d_channel_buffer[i].imag());
				}
				//std::cout<<std::endl;
				//printf("--> channel out\n");
				//std::cout<<nOutputItems<<"\n";
				//d_channel_buffer_full = false;
				nodx_h += d_channel_size;
				no++;
			}
			else
			{
				d_header_index = 0;
				d_state = ST_PACKET_NUMBER_TRANS;
			}
			break;
		}
		case ST_PACKET_NUMBER_TRANS:
		{
			if(d_header_index<3)
			{
				out[no] = d_trans_packet_number;
				d_header_index++;
				for (i = 0; i < d_channel_size; i++)
				{
					out_h[nodx_h+i] = gr_complex(0,0);
				}
				//d_channel_buffer_full = false;
				nodx_h += d_channel_size;
				no++;
			}
			else
			{
				d_header_index = 0;
				d_state = ST_DATA_TRANS;
			}
			break;
		}
		case ST_DATA_TRANS:
		{
			if(d_data_trans_index<d_packet_size)
			{
				out[no] = d_origin_data[d_data_trans_index];
				d_data_trans_index++;
				for (i = 0; i < d_channel_size; i++)
				{
					out_h[nodx_h+i] = gr_complex(0,0);
				}
				//d_channel_buffer_full = false;
				nodx_h += d_channel_size;
				no++;
			}
			else
			{
				std::cout<<"Packet Transmitted Number "<<(int) d_trans_packet_number<<"\n";
				d_data_trans_index = 0;
				count++;
				d_state = ST_RECEIVE;
				//std::cout<<"PACKET TRANS -----> RECEIVE \n";
			}
			break;
		}*/
		}
	}
        consume (0, ni0);
	consume (1, ni1);
	consume (2, ni2);
	return no;
    }

  } /* namespace CogNC */
} /* namespace gr */

