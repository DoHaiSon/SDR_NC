/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "transmitter_control_p2p_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <math.h>
#include <time.h>

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace DNC {

    transmitter_control_p2p::sptr
    transmitter_control_p2p::make(int packet_size, int guard_interval, unsigned char transmitter_id, unsigned char receiver_id)
    {
      return gnuradio::get_initial_sptr
        (new transmitter_control_p2p_impl(packet_size, guard_interval, transmitter_id, receiver_id));
    }

    /*
     * The private constructor
     */
    transmitter_control_p2p_impl::transmitter_control_p2p_impl(int packet_size, int guard_interval, unsigned char transmitter_id, unsigned char receiver_id)
      : gr::block("transmitter_control_p2p",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_packet_size(packet_size), d_guard_interval(guard_interval), d_transmitter_id(transmitter_id), d_receiver_id(receiver_id),
	      d_buffer_size(255), d_load_packet_index(0), d_load_data_index(0), d_load_packet_number(0x01), d_load_data(false),
	      d_tx_buffer_index(0), d_header_index(0), d_data_index(0), d_guard_index(0), d_check_rx_id_count(0),
	      d_check_rx_id_index(0), d_check_recv_packet_number_index(0), d_number_of_session_packets(0),
	      d_expected_session_number(0x01), d_check_session_index(0), d_check_session_count(0),
	      d_loaded_data(false), d_tx_state(ST_WAIT), d_rx_state(ST_IDLE)
    {
	if (transmitter_id==0x00)
		throw std::runtime_error("Invalid parameter! Transmitter ID must be different from 0...!!!\n");
	if (receiver_id==0x00)
		throw std::runtime_error("Invalid parameter! Receiver ID must be different from 0...!!!\n");

	d_confirm_packet_number.resize(d_buffer_size);
	std::fill(d_loaded_packet_number.begin(), d_loaded_packet_number.end(), 0x00);	

	d_loaded_packet_number.resize(d_buffer_size);
	std::fill(d_loaded_packet_number.begin(), d_loaded_packet_number.end(), 0x00);

	d_transmitted_packet_number.resize(d_buffer_size);
	std::fill(d_transmitted_packet_number.begin(), d_transmitted_packet_number.end(), 0x00);

	d_transmitted_data_buffer.resize(d_buffer_size*packet_size);
	std::fill(d_transmitted_data_buffer.begin(), d_transmitted_data_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    transmitter_control_p2p_impl::~transmitter_control_p2p_impl()
    {
    }

    void
    transmitter_control_p2p_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    void
    transmitter_control_p2p_impl::reset()
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
    transmitter_control_p2p_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
	int nInputItems0 = ninput_items[0];
	int nInputItems1 = ninput_items[1];
	int nOutputItems = noutput_items;
        const unsigned char *in0 = (const unsigned char *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];
        unsigned char *out = (unsigned char *) output_items[0];
	
	int ni0 = 0;
	int ni1 = 0;
	int no = 0;
	while(no<nOutputItems)
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
			d_loaded_data = true;
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
						d_loaded_data = true;
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
				return no;
			}
		//}
	}
	else
	{
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
	}
	//=============================
	while(ni0<nInputItems0)
	{
		switch(d_rx_state)
		{
		case ST_IDLE:
		{
			if(in0[ni0]==d_receiver_id)
			{
				d_rx_state = ST_CHECK_RECEIVER_ID;
				d_check_rx_id_index++;
				d_check_rx_id_count++;
			}
			ni0++;
			break;
		}
		case ST_CHECK_RECEIVER_ID:
		{
			if(in0[ni0]==d_receiver_id)
			{
				d_check_rx_id_count++;
			}
			d_check_rx_id_index++;
			ni0++;
			if(d_check_rx_id_index==3)
			{
				if(d_check_rx_id_count==3)
				{
					d_rx_state = ST_CHECK_PACKET_NO_CONFIRM;
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
		case ST_CHECK_PACKET_NO_CONFIRM:
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
				if(a==b&&b==c) { rx_pkt_no = a; recv_pkt = true; }
				if(recv_pkt == true)
				{
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
		case ST_CHECK_SESSION_NUMBER:
		{
			if(in0[ni0]==d_expected_session_number)
			{
				d_check_session_count++;
			}
			d_check_session_index++;
			ni0++;
			if(d_check_session_index==6)
			{
				if(d_check_session_count==6)
				{
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
				d_check_session_index = 0;
				d_check_session_count = 0;
				d_rx_state = ST_IDLE;
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

  } /* namespace DNC */
} /* namespace gr */

