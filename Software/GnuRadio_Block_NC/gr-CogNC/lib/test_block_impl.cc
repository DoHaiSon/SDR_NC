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
#include "test_block_impl.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <math.h>
#include <time.h>
#include <fstream>

namespace gr {
  namespace CogNC {

    test_block::sptr
    test_block::make(int packet_size, unsigned char source_id, unsigned char destination_id)
    {
      return gnuradio::get_initial_sptr
        (new test_block_impl(packet_size, source_id, destination_id));
    }

    /*
     * The private constructor
     */
    test_block_impl::test_block_impl(int packet_size, unsigned char source_id, unsigned char destination_id)
      : gr::block("test_block",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
    	      d_packet_size(packet_size), d_source_id(source_id), d_destination_id(destination_id), d_beacon_id(0x00),
	      d_check_index(0), d_beacon_count(0), d_check_next_packet_number_count(0), d_check_last_packet_number_count(0), d_check_recv_beacon(false), 
	      d_origin_data_index(0), d_header_index(0), d_data_trans_index(0), d_xored_data_index(0), d_data_out_index(0), 
	      d_next_packet_number(0x01), d_trans_packet_number(0x01), d_check_recv_ack(false), d_check_recv_this_ack(false), d_check_destination_id_count(0), d_check_destination_id_index(0),
	      d_packet_number_count(1), d_state(ST_WAIT)
    {
	d_origin_data.resize(d_packet_size);
	std::fill(d_origin_data.begin(), d_origin_data.end(), 0x00);
	d_data_out.resize(d_packet_size);
	std::fill(d_data_out.begin(), d_data_out.end(), 0x00);
	
	//std::ofstream fw("/home/khachoang/testimage.txt");
    }


    /*
     * Our virtual destructor.
     */
    test_block_impl::~test_block_impl()
    {
    }

    void
    test_block_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    test_block_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
	int nOutputItems = noutput_items;
	//if(nInputItems0!=0&&nInputItems1!=0&&nOutputItems!=0)
	//std::cout<<nInputItems0<<"/ "<<nInputItems1<<"/ "<<nOutputItems<<"\n";
        const unsigned char *in0 = (const unsigned char *) input_items[0]; // rx beacon or ack
        const unsigned char *in1 = (const unsigned char *) input_items[1]; // data to be transmitted
        unsigned char *out = (unsigned char *) output_items[0];

	int ni0 = 0;
	int ni1 = 0;
	int no = 0;

	while(ni0<nInputItems0 && ni1<nInputItems1 && no<nOutputItems)
	{
		switch(d_state)
		{
		case ST_WAIT:
		{
			if(in0[ni0]==d_destination_id)
			{
				d_check_destination_id_count++;
				d_check_destination_id_index++;
				d_state = ST_CHECK_DESTINATION_ID;
			}
			//out[no] = 0x00;
			//no++;
			ni0++;
			break;
		}
		case ST_CHECK_DESTINATION_ID:
		{
			if(in0[ni0]==d_destination_id)
			{
				d_check_destination_id_count++;
			}
			d_check_destination_id_index++;
			if(d_check_destination_id_index==3)
			{
				if(d_check_destination_id_count==3)
				{
					std::cout<<"detect destination node ID: ";
					for (int i = 0;i<9;i++)
					{
						std::cout<<(int) in0[ni0 - 5 + i]<<" ";
					}
					std::cout<<"\n";
					d_state = ST_CHECK_PACKET_NO;
				}
				else
				{
					d_state = ST_WAIT;
				}
				d_check_destination_id_index = 0;				
				d_check_destination_id_count = 0;
				ni0++;
				break;
			}
			//out[no] = 0x00;
			//no++;
			ni0++;
			break;
		}
		case ST_CHECK_PACKET_NO:
		{
			d_check_index++;
			if(in0[ni0]==d_beacon_id&&d_check_recv_ack==false)
			{
				d_beacon_count++;
			}
			if(in0[ni0]==(d_next_packet_number-1))
			{
				d_check_last_packet_number_count++;
			}
			if(in0[ni0]==d_next_packet_number)
			{
				d_check_next_packet_number_count++;
			}
			if(d_check_index==3)
			{
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
				}
				else
				{
					if(d_check_recv_ack==true)// && d_check_last_packet_number_count>=2)
					{
						std::cout<<"recv ack number "<<(int) d_next_packet_number-1<<"\n";
						d_state = ST_SOURCE_ID_TRANS;
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
							d_check_recv_beacon = true;
						}
						else
						{
							d_state = ST_SOURCE_ID_TRANS;
						}
					}
					else
					{
						d_state = ST_WAIT;
					}
					d_beacon_count = 0;
				}
				d_check_next_packet_number_count = 0;
				d_check_last_packet_number_count = 0;
				d_check_index = 0;
			}
			//out[no] = 0x00;
			//no++;
			ni0++;
			break;
		}
		case ST_GET_ORIGIN_DATA:
		{
			if(d_origin_data_index<d_packet_size)
			{
				d_origin_data[d_origin_data_index] = in1[ni1];
				//std::cout<<(int)d_origin_data[d_origin_data_index]<<" ";
				//std::ofstream fw("/home/khachoang/testimage.txt");	
				//fw<<(int)d_origin_data[d_origin_data_index-1]<<" "; fw.close();
				d_origin_data_index++;
				ni1++;
			}
			else
			{
				//std::ofstream fw("/home/khachoang/testimage.txt");	
				//std::cout<<"\n"; //fw.close();
				std::cout<<"Data loaded \n";
				d_origin_data_index = 0;
				d_state = ST_SOURCE_ID_TRANS;
			}
			break;
		}
		case ST_SOURCE_ID_TRANS:
		{
			if(d_header_index<3)
			{
				out[no] = d_source_id;
				no++;
				d_header_index++;
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
				no++;
				d_data_trans_index++;
			}
			else
			{
				std::cout<<"Packet Transmitted Number "<<(int) d_trans_packet_number<<"\n";
				d_data_trans_index = 0;
				d_state = ST_WAIT;
			}
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

