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
#include "terminal_controller_impl.h"

namespace gr {
  namespace DNC {

    terminal_controller::sptr
    terminal_controller::make(int packet_size, int guard_interval, unsigned char end_node_id, unsigned char relay_node_id)
    {
      return gnuradio::get_initial_sptr
        (new terminal_controller_impl(packet_size, guard_interval, end_node_id, relay_node_id));
    }

    /*
     * The private constructor
     */
    terminal_controller_impl::terminal_controller_impl(int packet_size, int guard_interval, unsigned char end_node_id, unsigned char relay_node_id)
      : gr::block("terminal_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
	      d_packet_size(packet_size), d_guard_interval(guard_interval), d_end_node_id(end_node_id), d_relay_node_id(relay_node_id),
	      d_buffer_size(127), d_next_out_packet_no(0x01), d_number_of_session_packets(0), 
	      d_check_rx_id_index(0), d_check_rx_id_count(0), d_tx_buffer_index(0), d_load_data_index(0), d_check_confirm(false), 
	      d_load_data(false), d_data_loaded(false),
	      d_load_packet_number(0x01), d_trans_header_index(0), d_trans_data_index(0), d_guard_index(0), 
	      d_packets_out_index(0), d_data_out_index(0), d_xor_rx_pkt_no(0x00), d_rx_data_xored_index(0), 
	      d_check_recv_packet_number_index(0), d_data_out(false), d_zero_trans_index(0), 
	      d_check_end_packet_index(0), d_check_end_packet_count(0), d_number_of_packet_out(0), d_load_packet_index(0),
	      d_tx_state(ST_WAIT), d_rx_state(ST_IDLE)
    {
	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please make guard interval be greater than or equal to 6...!\n");

	if (end_node_id==0x00)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from 0...!!!\n");

	if (relay_node_id==0x00)
		throw std::runtime_error("Invalid parameter! Relay Node ID must be different from 0...!!!\n");

	if (end_node_id==relay_node_id)
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
    terminal_controller_impl::~terminal_controller_impl()
    {
    }

    void
    terminal_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    void
   terminal_controller_impl::reset()
    {
	d_number_of_session_packets = 0;
	d_tx_buffer_index = 0;
	d_check_rx_id_index = 0;
	d_check_rx_id_count = 0;
	d_trans_header_index = 0;
	d_trans_data_index = 0;
	d_guard_index = 0;
	d_rx_data_xored_index = 0;
	d_check_recv_packet_number_index = 0;
	d_zero_trans_index = 0;
	d_check_end_packet_index = 0;
	d_check_end_packet_count = 0;
    }

    int
    terminal_controller_impl::general_work (int noutput_items,
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
			d_data_loaded = true;
			d_rx_state = ST_IDLE;
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
					d_data_loaded = true;
					d_rx_state = ST_IDLE;
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
					if(d_load_packet_index==127)
					{
						if(d_load_packet_number==0xFF)
						{
							d_load_packet_number = 0x01;
						}
						d_load_packet_index = 0;
						std::cout<<"d_number_of_session_packets = "<<d_number_of_session_packets<<"\n";
						d_load_data = false;
						d_data_loaded = true;
						d_rx_state = ST_IDLE;
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
					if(rx_pkt_no==0x00&&d_check_confirm==false)
					{
						std::cout<<"Received Confirmation!\n";
						d_check_confirm = true;
						d_load_data = true;
						d_rx_state = ST_IDLE;
					}
					else
					{
						if(rx_pkt_no!=0x00&&d_check_confirm == true)
						{
							d_xor_rx_pkt_no = rx_pkt_no;
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
			if(in0[ni0]==0x00)
			{
				d_check_end_packet_count++;
			}
			d_check_end_packet_index++;
			ni0++;
			if(d_check_end_packet_index==6)
			{
				if(d_check_end_packet_count>4)
				{
					bool check = false;
					int idx = 0;
					for(int i = 0; i<127; i++)
					{
						if(d_transmitted_packet_number[i]==d_xor_rx_pkt_no)
						{
							check = true;
							idx = i;
							break;
						}
					}
					if(check==true&&d_received_packet_number[idx]==0x00)
					{
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
						int idx = (int)d_next_out_packet_no-1;
						if(idx>126)
						{
							idx = idx - 127;
						}
						int des = idx*d_packet_size+d_data_out_index-1;
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
					d_number_of_packet_out = 0;
					if(d_next_out_packet_no==0xFF)
					{
						d_next_out_packet_no = 0x01;
					}
					if(d_number_of_session_packets==127)
					{
						for(int i = 0; i<d_buffer_size; i++)
						{
							d_loaded_packet_number[i] = 0x00;
							d_transmitted_packet_number[i] = 0x00;
							d_received_packet_number[i] = 0x00;
						}
						d_load_data = true;
						d_data_loaded = false;
						d_tx_state = ST_WAIT;
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
		case ST_WAIT:
		{
			//std::cout<<"ST_WAIT\n";
			out0[no] = 0x00;
			no++;
			if(d_data_loaded==true)
			{
				d_tx_state = ST_BUFFER_MANAGEMENT;
			}
			break;
		}
		case ST_BUFFER_MANAGEMENT:
		{
			//std::cout<<"ST_BUFFER_MANAGEMNET\n";
			int idx = (int)d_next_out_packet_no-1;
			if(idx>126)
			{
				idx = idx - 127;
			}
			if(d_received_packet_number[idx]==d_next_out_packet_no&&d_data_out==false)
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

  } /* namespace DNC */
} /* namespace gr */

