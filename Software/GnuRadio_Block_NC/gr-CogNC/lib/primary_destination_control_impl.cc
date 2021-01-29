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
#include "primary_destination_control_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <math.h>
#include <time.h>

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace CogNC {

    primary_destination_control::sptr
    primary_destination_control::make(int packet_size, unsigned char source_id, unsigned char destination_id, int time_out)
    {
      return gnuradio::get_initial_sptr
        (new primary_destination_control_impl(packet_size, source_id, destination_id, time_out));
    }

    /*
     * The private constructor
     */
    primary_destination_control_impl::primary_destination_control_impl(int packet_size, unsigned char source_id, unsigned char destination_id, int time_out)
      : gr::block("primary_destination_control",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
    	      d_packet_size(packet_size), d_source_id(source_id), d_destination_id(destination_id),
	      d_time_out(time_out), d_beacon_index(0), d_req_data_retrans_index(0), d_beacon_id(0x00),
	      d_data_index(0), d_header_index(0), d_packet_number_index(0),
	      d_recved_data(false), d_reach_data(false), d_reach_packet_number(false), d_beacon_trans(false), 	      d_next_packet_number(0x01), d_trans_packet_number(0x01), d_packet_number_count(0), d_packet_no(1), d_check_id_index(0), d_check_source_id_count(0), d_check_destination_id_count(0), 
	      d_state(ST_BEACON_TRANS)//, d_state_trans(ST_DO_BEACON_TRANS)
    {
	d_clock = clock();
	d_clock_diff = float(0.0);

	d_data.resize(d_packet_size);
	std::fill(d_data.begin(), d_data.end(), 0x00);
    }

    /*
     * Our virtual destructor.
     */
    primary_destination_control_impl::~primary_destination_control_impl()
    {
    }

    void
    primary_destination_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    primary_destination_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems = ninput_items[0];
	int bound = 2*d_packet_size;
	int nOutputItems = noutput_items;
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out0 = (unsigned char *) output_items[0];
        unsigned char *out1 = (unsigned char *) output_items[1];

	int ni = 0;
	int no = 0;

	while(ni<std::min(bound,nInputItems) && no<std::min(bound,nOutputItems))
	//while(ni<nInputItems && no<nOutputItems)
	//while(ni<bound && no<bound)
	{
		switch (d_state)
		{
		case ST_WAIT:
		{
			d_clock_diff = ((float)(clock() - d_clock) / CLOCKS_PER_MSEC);
			if (d_clock_diff > (float)d_time_out)
			{
				if(d_beacon_trans==false)
				{
					std::cout<<"timeout: retrans beacon "<<std::endl;
					d_state = ST_BEACON_TRANS;
				}
				else
				{
					std::cout<<"timeout: retrans ack number "<<(int)d_trans_packet_number<<std::endl;
					d_state = ST_ACK_TRANS;
				}
				break;
			}
			if(in[ni]==d_source_id)
			{
				d_check_id_index++;				
				d_check_source_id_count++;
				d_state = ST_CHECK_ID;
			}
			//out0[no] = 0x00;
			//out1[no] = 0x00;
			//no++;
			ni++;
			break;
		}
		case ST_CHECK_ID:
		{
			if(in[ni]==d_source_id)
			{
				d_check_source_id_count++;
			}
			if(in[ni]==d_destination_id)
			{
				d_check_destination_id_count++;
			}
			d_check_id_index++;
			if(d_check_id_index==3)
			{
				if(d_check_source_id_count==3)
				{
					/*std::cout<<"detect source ID: ";
					for (int i = 0;i<6;i++)
					{
						std::cout<<(int)in[ni-2+i]<<" ";
					}
					std::cout<<"\n";*/
					//printf("%#02X \n", (unsigned char) d_check_node_id);
					//out[no] = d_source_id;
					d_state = ST_CHECK_PACKET_NO;
					d_reach_packet_number=true;
					//d_clock = clock();
				}
				else
				{
					if(d_check_destination_id_count==3)
						std::cout<<"detect destination ID \n";
					d_state = ST_WAIT;
					d_clock = clock();
				}
				d_check_id_index = 0;				
				d_check_source_id_count = 0;
				d_check_destination_id_count = 0;
				//out0[no] = 0x00;
				//out1[no] = 0x00;
				//no++;
				//ni++;
				//break;
			}
			//out0[no] = 0x00;
			//out1[no] = 0x00;
			//no++;
			ni++;
			break;
		}
		case ST_CHECK_PACKET_NO:
		{
			if(d_reach_data==true)
			{
				if(d_data_index<d_packet_size)
				{
					d_data[d_data_index] = in[ni]; //store data into buffer
					//printf("%#02X, ", (unsigned char) in0[ni]);
					d_data_index++;
					if(d_data_index==d_packet_size)
					{
						//d_recved_data = true;
						//std::cout<<"\n";
						d_state = ST_DATA_OUT;
						std::cout<<"recv packets from source: "<<d_packet_no<<std::endl;
						d_reach_packet_number = false;
						d_reach_data = false;
						//d_recved_data = false;
						d_beacon_trans = true;
						d_packet_number_index = 0;
						d_packet_number_count = 0;
						d_data_index = 0;
						ni++;
						out1[no] = d_destination_id;
						out0[no] = 0x00;
						no++;
						break;
					}
				}
			}
			if(d_reach_packet_number==true)
			{
				if(d_packet_number_index<3)
				{
					//printf("%#02X, ", (unsigned char) in0[ni]);
					if(in[ni]==d_next_packet_number)
					{
						d_packet_number_count++;
					}
					d_packet_number_index++;
					if(d_packet_number_index==3)
					{
						if(d_packet_number_count>=2)
						{
							d_reach_data = true;
							//std::cout<<"get correct packet number\n";
						}
						else
						{
							d_reach_packet_number = false;
							//std::cout<<"\nd_reach_packet_number = false;"<<std::endl;
							d_packet_number_index = 0;
							d_packet_number_count = 0;
							d_state = ST_WAIT;
							d_clock = clock();
						}
					}
				}
			}
			//out0[no] = 0x00;
			//out1[no] = 0x00;
			ni++;
			//no++;
			break;
		}
		case ST_DATA_OUT:
		{
			if(d_data_index<d_packet_size)
			{
				out1[no] = d_data[d_data_index];
				//std::cout<<(int)d_data[d_data_index]<<" ";
				d_data_index++;
			}
			else
			{
				/*print rx data*/
				//std::cout<<"rx data: \n";
				/*for(int i = 0; i<d_packet_size; i++)
				{
				std::cout<<(int)d_data[i];
				//printf("%#02X, ", (unsigned char) d_data[i]);
				}*/
				//std::cout<<"\n";
				
				d_trans_packet_number = d_next_packet_number;
				if(d_packet_no==255)
				{
					d_next_packet_number = 0X01;
					d_packet_no = 1;
				}
				else
				{
					d_packet_no++;
					d_next_packet_number++;
				}
				d_state = ST_ACK_TRANS;
				d_data_index = 0;
				//out0[no] = 0x00;
				//out1[no] = 0x00;
				//no++;
				break;
			}
			out0[no] = 0x00;
			no++;
			break;
		}
		case ST_BEACON_TRANS:
		{
			if(d_beacon_index<4)
			{	
				out0[no] = d_destination_id;
				out1[no] = 0x00;
				d_beacon_index++;
				no++;
				break;
			}
			else if(d_beacon_index<7)
			{
				out0[no] = d_beacon_id;
				out1[no] = 0x00;
				d_beacon_index++;
				no++;
				break;
			}
			if(d_beacon_index == 7)
			{
				std::cout<<"trans beacon! "<<(int)d_destination_id<<"\n";
				d_beacon_index = 0;
				d_state = ST_WAIT;
				d_clock = clock();
				break;
			}
		}
		case ST_ACK_TRANS:
		{
			if(d_header_index<4)
			{
				out0[no] = d_destination_id;
				out1[no] = 0x00;
				d_header_index++;
				no++;
				break;
			}
			else if(d_header_index<7)
			{
				out0[no] = d_trans_packet_number;
				out1[no] = 0x00;
				d_header_index++;
				no++;
				break;
			}
			if(d_header_index == 7)
			{
				//printf("ack transmitted \n");
				d_header_index = 0;
				d_state = ST_WAIT;
				d_clock = clock();
				break;
			}
		}
		}
	}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each(ni);
	//std::cout<<"consume "<<ni<<std::endl;
	//std::cout<<"return "<<no<<std::endl;
        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace CogNC */
} /* namespace gr */

