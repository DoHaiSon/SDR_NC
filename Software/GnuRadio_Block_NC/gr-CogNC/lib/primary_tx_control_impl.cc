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
#include "primary_tx_control_impl.h"
#include <cstdio>

namespace gr {
  namespace CogNC {

    primary_tx_control::sptr
    primary_tx_control::make(int packet_sz, int guard_interval, unsigned char source_id, unsigned char destination_id, int ack_burst_size, int channel_size)
    {
      return gnuradio::get_initial_sptr
        (new primary_tx_control_impl(packet_sz, guard_interval, source_id, destination_id, ack_burst_size, channel_size));
    }

    /*
     * The private constructor
     */
    primary_tx_control_impl::primary_tx_control_impl(int packet_sz, int guard_interval, unsigned char source_id, unsigned char destination_id, int ack_burst_size, int channel_size)
      : gr::block("primary_tx_control",
              gr::io_signature::make3(3, 3, sizeof(unsigned char)*ack_burst_size, sizeof(gr_complex)*channel_size, sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(gr_complex)*channel_size)),
     	      d_packet_size(packet_sz), d_guard_interval(guard_interval), d_transmitter_id(source_id), d_receiver_id(destination_id), d_ack_burst_size(ack_burst_size), d_channel_size(channel_size),
	      d_buffer_size(255), d_load_packet_index(0), d_load_data_index(0), d_load_packet_number(0x01), d_load_data(false),
	      d_tx_buffer_index(0), d_header_index(0), d_data_index(0), d_guard_index(0), d_check_rx_id_count(0),
	      d_check_rx_id_index(0), d_check_recv_packet_number_index(0), d_number_of_session_packets(0),
	      d_expected_session_number(0x01), d_check_session_index(0), d_check_session_count(0),
	      d_loaded_data(false), d_channel_buffer_full(false), d_tx_state(ST_WAIT), d_rx_state(ST_RECEIVE)
    {
	if (source_id==0x00)
		throw std::runtime_error("Invalid parameter! source ID must be different from 0...!!!\n");
	if (destination_id==0x00)
		throw std::runtime_error("Invalid parameter! destination ID must be different from 0...!!!\n");
	if(ack_burst_size != 6)
		throw std::runtime_error("Invalid parameter! acknowlege burst size must be 6...!!!\n");

	d_confirm_packet_number.resize(d_buffer_size);
	std::fill(d_loaded_packet_number.begin(), d_loaded_packet_number.end(), 0x00);	

	d_loaded_packet_number.resize(d_buffer_size);
	std::fill(d_loaded_packet_number.begin(), d_loaded_packet_number.end(), 0x00);

	d_transmitted_packet_number.resize(d_buffer_size);
	std::fill(d_transmitted_packet_number.begin(), d_transmitted_packet_number.end(), 0x00);

	d_transmitted_data_buffer.resize(d_buffer_size*packet_sz);
	std::fill(d_transmitted_data_buffer.begin(), d_transmitted_data_buffer.end(), 0x00);

	d_channel_buffer.resize(d_channel_size);
	std::fill(d_channel_buffer.begin(), d_channel_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx_packet_number[i] = 0x00;
	}
    }


    /*
     * Our virtual destructor.
     */
    primary_tx_control_impl::~primary_tx_control_impl()
    {
    }

    void
    primary_tx_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    void  primary_tx_control_impl::reset()
    {
	for(int k = 0; k<d_buffer_size; k++)
	{
		d_loaded_packet_number[k] = 0x00;
		d_transmitted_packet_number[k] = 0x00;
		d_confirm_packet_number[k] = 0x00;
	}
	d_number_of_session_packets = 0;
	d_tx_buffer_index = 0;
	d_header_index = 0;
	d_data_index = 0;
	d_guard_index = 0;
	d_tx_state = ST_WAIT;
    }

    int
    primary_tx_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items_v,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	int nInputItems0 = ninput_items_v[0];
	int nInputItems1 = ninput_items_v[1];
	int nInputItems2 = ninput_items_v[2];
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

	int i = 0;
	int j = 0;
	
	//printf("ni0: %i, ni1: %i, ni2: %i, no: %i\n",nInputItems0,nInputItems1,nInputItems2,nOutputItems);	

	while(ni0<nInputItems0 && ni1<nInputItems1)
	{
	/* Loading native packets */
	if(d_load_data==true)
	{
		if(ni2<nInputItems2)
		{
			if(d_load_data_index==0)
			{
				if(in_data[ni2]==0x01)
				{
					std::cout<<"end data"<<std::endl;
					d_number_of_session_packets--;
					std::cout<<"d_number_of_session_packets = "<<d_number_of_session_packets<<"\n";
					d_load_packet_number = 0x01;
					d_load_packet_index = 0;
					d_load_data_index = 0;
					d_load_data = false;
					d_loaded_data = true;
				}
				d_load_data_index++;
				ni2++;
			}
			else
			{
				d_transmitted_data_buffer[d_load_packet_index*d_packet_size+d_load_data_index-1] = in_data[ni2];
				d_load_data_index++;
				ni2++;
				if(d_load_data_index==(d_packet_size+1))
				{
					d_number_of_session_packets++;
					d_loaded_packet_number[d_load_packet_index] = d_load_packet_number;
					d_load_packet_index++;
					d_load_packet_number++;
					d_load_data_index = 0;
					if(d_load_packet_index==d_buffer_size)
					{
						d_load_packet_number = 0x01;
						d_load_packet_index = 0;
						std::cout<<"d_number_of_session_packets = "<<d_number_of_session_packets<<"\n";
						d_load_data = false;
						d_loaded_data = true;
					}
				}
			}
		}
		else
		{
			consume (0, ni0);
			consume (1, ni1);
			consume (2, ni2);
			return no;
		}
	}
	else
	{
		switch(d_rx_state)
		{
		case ST_RECEIVE:
		{
			for (j=0; j<3; j++)
			{
				if(in_ack[nidx_ack+j]==d_receiver_id)
				{
					d_check_rx_id_count++;
				}
			}
			if(d_check_rx_id_count==3)
			{
				//printf("received beacon\n");
				for (i = 0; i < d_channel_size; i++)
				{
					d_channel_buffer[i] = in_h[nidx_h+i];
				}
				d_channel_buffer_full = true;
				for (j=3; j<6; j++)
				{
					d_rx_packet_number[j-3] = in_ack[nidx_ack+j];
				}
				unsigned char a, b, c;
				unsigned char rx_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx_packet_number[0];
				b = d_rx_packet_number[1];
				c = d_rx_packet_number[2];
				//if(a==b&&b==c) { rx_pkt_no = a; recv_pkt = true; }
				if(a==b) {rx_pkt_no = a; recv_pkt = true;}
				else
				{
					if(b==c) {rx_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) {rx_pkt_no = c; recv_pkt = true;}
					}
				}
				if(recv_pkt == true)
				{	//printf("recv packet no ack = %i \n",(int)rx_pkt_no);
					if(rx_pkt_no==0x00)
					{
						d_rx_state = ST_CHECK_SESSION_NUMBER;
					}
					else
					{
						int pkt_idx = (int)rx_pkt_no-1;
						if(d_transmitted_packet_number[pkt_idx]==rx_pkt_no)
						{
							d_confirm_packet_number[pkt_idx] = rx_pkt_no;
						}
					}
				}
			}
			d_check_rx_id_count = 0;
			nidx_ack += d_ack_burst_size;
			nidx_h += d_channel_size;
			ni0 ++;
			ni1 ++;
			break;
		}
		case ST_CHECK_SESSION_NUMBER:
		{
			for (i=0;i<6;i++)
			{			
				if(in_ack[nidx_ack+i]==d_expected_session_number)
				{
					d_check_session_count++;
				}
			}
			if(d_check_session_count>4)
			{
				for (i = 0; i < d_channel_size; i++)
				{
					d_channel_buffer[i] = in_h[nidx_h+i];
				}
				d_channel_buffer_full = true;
				if(d_expected_session_number==0xFF)
				{
					d_expected_session_number = 0x01;
				}
				else
				{
					d_expected_session_number++;
				}
				std::cout<<"Received session request!\n";
				d_load_data = true;
				d_loaded_data = false;
				reset();
			}
			d_check_session_count = 0;
			d_rx_state = ST_RECEIVE;
			nidx_ack += d_ack_burst_size;
			nidx_h += d_channel_size;
			ni0 ++;
			ni1 ++;
			break;
		}
		}
	}
	}
	//=============================
	while(no<nOutputItems)
	{
		//std::cout<<nOutputItems<<"\n";
		if (d_channel_buffer_full)
		{
			//std::cout<<"channel: ";
			for (i = 0; i < d_channel_size; i++)
			{
				out_h[nodx_h+i] = d_channel_buffer[i];
				//printf("%.4f+i*%.4f, ", d_channel_buffer[i].real(), d_channel_buffer[i].imag());
			}
			//std::cout<<std::endl;
			//printf("--> channel out\n");
			//std::cout<<nOutputItems<<"\n";
		}
		else
		{
			for (i = 0; i < d_channel_size; i++)
			{
				out_h[nodx_h+i] = gr_complex(0,0);
			}
		}
		d_channel_buffer_full = false;
		nodx_h += d_channel_size;

		switch(d_tx_state)
		{
		case ST_WAIT:
		{
			if(d_loaded_data==true)
			{
				d_tx_state = ST_BUFFER_MANAGEMENT;
			}
			out[no] = 0x00;
			no++;
			break;
		}
		case ST_BUFFER_MANAGEMENT:
		{
			if(d_confirm_packet_number[d_tx_buffer_index]==0x00)
			{
				d_tx_state = ST_TRANSMITTER_ID_TRANS;
			}
			else
			{
				d_tx_buffer_index++;
				if(d_tx_buffer_index==d_number_of_session_packets)
				{
					d_tx_buffer_index = 0;
				}
			}
			out[no] = 0x00;
			no++;
			break;
		}
		case ST_TRANSMITTER_ID_TRANS:
		{
			out[no] = d_transmitter_id;
			d_header_index++;
			no++;
			if(d_header_index==3)
			{
				d_tx_state = ST_PACKET_NO_TRANS;
				d_header_index = 0;
			}
			break;
		}
		case ST_PACKET_NO_TRANS:
		{
			out[no] = d_loaded_packet_number[d_tx_buffer_index];
			d_transmitted_packet_number[d_tx_buffer_index] = d_loaded_packet_number[d_tx_buffer_index];
			d_header_index++;
			no++;
			if(d_header_index==3)
			{
				printf("transmitted packet no %i \n", (int)d_loaded_packet_number[d_tx_buffer_index]);
				d_tx_state = ST_DATA_TRANS;
				d_header_index = 0;
			}
			break;
		}
		case ST_DATA_TRANS:
		{
			int idx = d_tx_buffer_index*d_packet_size + d_data_index;
			out[no] = d_transmitted_data_buffer[idx];
			d_data_index++;
			no++;
			if(d_data_index==d_packet_size)
			{
				d_tx_state = ST_GUARD_INTERVAL_TRANS;
				d_data_index = 0;
			}
			break;
		}
		case ST_GUARD_INTERVAL_TRANS:
		{
			out[no] = 0x00;
			d_guard_index++;
			no++;
			if(d_guard_index==d_guard_interval)
			{
				d_tx_state = ST_BUFFER_MANAGEMENT;
				d_guard_index = 0;
				d_tx_buffer_index++;
				if(d_tx_buffer_index==d_number_of_session_packets)
				{
					d_tx_buffer_index = 0;
				}
			}
			break;
		}
		}
	}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume (0, ni0);
	consume (1, ni1);
	consume (2, ni2);

        // Tell runtime system how many output items we produced.
        return no;    
     }

  } /* namespace CogNC */
} /* namespace gr */

