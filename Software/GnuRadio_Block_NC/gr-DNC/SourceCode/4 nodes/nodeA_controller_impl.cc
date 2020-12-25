
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
#include "nodeA_controller_impl.h"

namespace gr {
  namespace DNC {

    nodeA_controller::sptr
    nodeA_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeC_id, unsigned char nodeR_id)
    {
      return gnuradio::get_initial_sptr
        (new nodeA_controller_impl(buffer_size, packet_size, guard_interval, nodeA_id, nodeC_id, nodeR_id));
    }

    /*
     * The private constructor
     */
    nodeA_controller_impl::nodeA_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeC_id, unsigned char nodeR_id)
      : gr::block("nodeA_controller",
              gr::io_signature::make3(3, 3, sizeof(unsigned char), sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), 
	      d_nodeA_id(nodeA_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id),
	      d_load_data(false), d_check_rx0_id_index(0), d_check_rx0_id_count(0),
	      d_check_rx0_packet_number_index(0), d_next_session_number(0x01), d_check_session_count(0),
	      d_check_session_index(0), d_rx1_node_id_check(0x00), d_check_rx1_id_index(0), d_check_rx1_id_count(0),
	      d_check_rx1_packet_number_index(0), d_send_data(false), d_end_data(false),
	      d_load_data_index(0), d_number_of_session_packets(0), d_load_packet_number(0x01), d_load_packet_index(0), 
	      d_tx_buffer_index(0), d_trans_header_index(0), d_trans_data_index(0), d_guard_index(0), 	
	      d_ended_packet_number_trans_index(0), d_session_trans_index(0), d_tx_node_id(0x00),
	      d_tx_state(ST_TX_BUFFER_MANAGEMENT), d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE)
    {
	if (buffer_size>254)
		throw std::runtime_error("Invalid parameter! Please let buffer size be less than 255...!\n");

	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please let guard interval be greater than or equal to 6...!\n");

	if (nodeA_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeA ID must be different from 0...!!!\n");

	if (nodeR_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeR ID must be different from 0...!!!\n");

	if (nodeC_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeC ID must be different from 0...!!!\n");

	if (nodeA_id==nodeC_id||nodeC_id==nodeR_id||nodeR_id==nodeA_id)
		throw std::runtime_error("Invalid parameter! Node IDs must be different from each other...!!!\n");


	d_loaded_packet_number.resize(buffer_size);
	std::fill(d_loaded_packet_number.begin(), d_loaded_packet_number.end(), 0x00);

	d_transmitted_packet_number.resize(buffer_size);
	std::fill(d_transmitted_packet_number.begin(), d_transmitted_packet_number.end(), 0x00);

	d_transmitted_data_buffer.resize(buffer_size*packet_size);
	std::fill(d_transmitted_data_buffer.begin(), d_transmitted_data_buffer.end(), 0x00);

	d_confirm_packet_number.resize(buffer_size);
	std::fill(d_confirm_packet_number.begin(), d_confirm_packet_number.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx0_packet_number[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    nodeA_controller_impl::~nodeA_controller_impl()
    {
    }
   void
    nodeA_controller_impl::reset()
    {
	d_tx_buffer_index = 0;
	d_trans_header_index = 0;
	d_trans_data_index = 0;
	d_guard_index = 0;
        d_tx_state = ST_TX_BUFFER_MANAGEMENT;
	d_send_data = false;
	for(int i = 0; i<d_buffer_size; i++)
	{
		d_transmitted_packet_number[i] = 0x00;
		d_confirm_packet_number[i] = 0x00;
	}
    }
    void
    nodeA_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    nodeA_controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
        int nInputItems2 = ninput_items[2];
	int nOutputItems = noutput_items;
	
        const unsigned char *in0 = (const unsigned char *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];
        const unsigned char *in2 = (const unsigned char *) input_items[2];
        unsigned char *out = (unsigned char *) output_items[0];

	int ni0 = 0;
	int ni1 = 0;
	int ni2 = 0;
	int no = 0;

	while(ni0<nInputItems0)
	{
	// Load Native Packets 
	if(d_load_data==true)
	{
		if(ni2<nInputItems2)
		{
			if(d_load_data_index==0)
			{
				if(in2[ni2]==0x01)
				{
					std::cout<<"End Data"<<std::endl;
					std::cout<<"Number of Loaded Packets = "<<d_number_of_session_packets<<"\n";
					d_ended_packet_number = (unsigned char) d_number_of_session_packets;
					d_load_packet_number = 0x01;
					d_load_packet_index = 0;
					d_load_data_index = 0;
					d_load_data = false;
					d_end_data = true;
					reset(); // reset for tx
				}
				d_load_data_index++;
				ni2++;
			}
			else
			{
				d_transmitted_data_buffer[d_load_packet_index*d_packet_size+d_load_data_index-1] = in2[ni2];
				d_load_data_index++;
				ni2++;
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
						std::cout<<"Number of Loaded Packets = "<<d_number_of_session_packets<<"\n";
						d_load_data = false;
						d_rx0_state = ST_RX0_IDLE;
						reset(); // reset for tx
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
		switch(d_rx0_state)
		{
		case ST_RX0_IDLE:
		{
			if(in0[ni0]==d_nodeC_id)
			{
				d_rx0_state = ST_RX0_CHECK_NODE_ID;
				d_check_rx0_id_index++;
				d_check_rx0_id_count++;
			}
			ni0++;
			break;
		}
		case ST_RX0_CHECK_NODE_ID:
		{
			if(in0[ni0]==d_nodeC_id)
			{
				d_check_rx0_id_count++;
			}
			d_check_rx0_id_index++;
			ni0++;
			if(d_check_rx0_id_index==3)
			{
				if(d_check_rx0_id_count==3)
				{
					d_rx0_state = ST_RX0_CHECK_PACKET_NUMBER;
				}
				else
				{
					d_rx0_state = ST_RX0_IDLE;
					ni0 = ni0 - 1;
				}
				d_check_rx0_id_index = 0;
				d_check_rx0_id_count = 0;
			}
			break;
		}
		case ST_RX0_CHECK_PACKET_NUMBER:
		{
			d_rx0_packet_number[d_check_rx0_packet_number_index] = in0[ni0];
			d_check_rx0_packet_number_index++;
			ni0++;
			if(d_check_rx0_packet_number_index==3)
			{
				d_check_rx0_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx0_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx0_packet_number[0];
				b = d_rx0_packet_number[1];
				c = d_rx0_packet_number[2];
				if(a==b&&b==c) { rx0_pkt_no = a; recv_pkt = true;}
				if(recv_pkt == true)
				{
					if(rx0_pkt_no==0x00)
					{
						d_rx0_state = ST_RX0_CHECK_SESSION_NUMBER;
					}
				}
				else
				{
					d_rx0_state = ST_RX0_IDLE;
				}
			}
			break;
		}
		case ST_RX0_CHECK_SESSION_NUMBER:
		{
			if(in0[ni0]==d_next_session_number)
			{
				d_check_session_count++;
			}
			d_check_session_index++;
			ni0++;
			if(d_check_session_index==6)
			{
				if(d_check_session_count>4)
				{
					std::cout<<"Received Request for Node A\n";
					d_load_data = true;
					d_number_of_session_packets = 0;
					if(d_next_session_number==0xFF)
					{
						d_next_session_number = 0x01;
					}
					else
					{
						d_next_session_number++;
					}
					for(int i = 0; i<d_buffer_size; i++)
					{
						d_loaded_packet_number[i] = 0x00;
						d_transmitted_packet_number[i] = 0x00;
						d_confirm_packet_number[i] = 0x00;
					}
				}
				d_rx0_state = ST_RX0_IDLE;
				d_check_session_index = 0;
				d_check_session_count = 0;
			}
			break;
					
		}
		}
	}
	}

	while(ni1<nInputItems1)
	{
		switch(d_rx1_state)
		{
		case ST_RX1_IDLE:
		{
			if(in1[ni1]==d_nodeA_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODE_ID;
				d_rx1_node_id_check = d_nodeA_id;
				d_check_rx1_id_index++;
				d_check_rx1_id_count++;
			}
			if(in1[ni1]==d_nodeR_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODE_ID;
				d_rx1_node_id_check = d_nodeR_id;
				d_check_rx1_id_index++;
				d_check_rx1_id_count++;
			}
			ni1++;
			break;
		}
		case ST_RX1_CHECK_NODE_ID:
		{
			if(in1[ni1]==d_rx1_node_id_check)
			{
				d_check_rx1_id_count++;
			}
			d_check_rx1_id_index++;
			ni1++;
			if(d_check_rx1_id_index==3)
			{
				if(d_check_rx1_id_count==3)
				{
					d_rx1_state = ST_RX1_CHECK_PACKET_NUMBER;
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
					ni1 = ni1 - 1;
				}
				d_check_rx1_id_index = 0;
				d_check_rx1_id_count = 0;
			}
			break;
		}
		case ST_RX1_CHECK_PACKET_NUMBER:
		{
			d_rx1_packet_number[d_check_rx1_packet_number_index] = in1[ni1];
			d_check_rx1_packet_number_index++;
			ni1++;
			if(d_check_rx1_packet_number_index==3)
			{
				d_check_rx1_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx1_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx1_packet_number[0];
				b = d_rx1_packet_number[1];
				c = d_rx1_packet_number[2];
				if(a==b) { rx1_pkt_no = a; recv_pkt = true;}
				else 
				{
					if(b==c) { rx1_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) { rx1_pkt_no = c; recv_pkt = true;}
					}
				}
				if(recv_pkt == true)
				{
					if(rx1_pkt_no==0x00&&d_rx1_node_id_check==d_nodeR_id&&d_send_data==false)
					{
						d_rx1_state = ST_RX1_CHECK_SESSION_NUMBER;
					}
					else
					{
						int idx = (int)rx1_pkt_no - 1;
						if(idx<d_number_of_session_packets&&d_transmitted_packet_number[idx]!=0x00)
						{
							d_confirm_packet_number[idx] = rx1_pkt_no;
						}
					}
				}
				d_rx1_state = ST_RX1_IDLE;
			}
			break;
		}
		case ST_RX1_CHECK_SESSION_NUMBER:
		{
			unsigned char d_curr_session_no = (unsigned char) ((int)d_next_session_number-1);
			if(in0[ni0]==d_curr_session_no)
			{
				d_check_session_count++;
			}
			d_check_session_index++;
			ni0++;
			if(d_check_session_index==6)
			{
				if(d_check_session_count==6)
				{
					d_send_data = true;
				}
				d_rx0_state = ST_RX0_IDLE;
				d_check_session_index = 0;
				d_check_session_count = 0;
			}
			break;
					
		}
		}
	}

	// **** transmission states here ***
	while(no<nOutputItems)
	{
		switch(d_tx_state)
		{
		case ST_TX_BUFFER_MANAGEMENT:
		{
			if(d_confirm_packet_number[d_tx_buffer_index]==0x00&&
			   d_loaded_packet_number[d_tx_buffer_index]!=0x00)
			{
				d_tx_state = ST_TX_HEADER_TRANS;
			}
			else
			{
				d_tx_buffer_index++;
				if(d_tx_buffer_index==d_number_of_session_packets)
				{
					if(d_number_of_session_packets<d_buffer_size)
					{
						d_tx_state = ST_TX_END_PACKET_HEADER_TRANS;
					}
					d_tx_buffer_index = 0;
				}
			}
			out[no] = 0x00;
			no++;
			break;
		}
		case ST_TX_HEADER_TRANS:
		{
			if(d_trans_header_index<3)
			{
				out[no] = d_nodeA_id;
				d_trans_header_index++;
				no++;
				break;
			}
			if(d_trans_header_index>=3)
			{
				if(d_send_data==true) { out[no] = d_loaded_packet_number[d_tx_buffer_index]; }
				else		      { out[no] = 0x00; }
				d_transmitted_packet_number[d_tx_buffer_index] = d_loaded_packet_number[d_tx_buffer_index];
				d_trans_header_index++;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					if(d_send_data==true) { d_tx_state = ST_TX_DATA_TRANS; }
 					else		      { d_tx_state = ST_TX_SESSION_NUMBER_TRANS; }
				}
			}
			break;
		}
		case ST_TX_DATA_TRANS:
		{
			out[no] = d_transmitted_data_buffer[d_tx_buffer_index*d_packet_size+d_trans_data_index];
			no++;
			d_trans_data_index++;
			if(d_trans_data_index==d_packet_size)
			{
				d_trans_data_index = 0;
				d_tx_state = ST_TX_GUARD_INTERVAL_TRANS;
			}
			break;
		}
		case ST_TX_GUARD_INTERVAL_TRANS:
		{
			out[no] = 0x00;
			d_guard_index++;
			no++;
			if(d_guard_index==d_guard_interval)
			{
				d_guard_index = 0;
				d_tx_buffer_index++;
				if(d_tx_buffer_index==d_number_of_session_packets)
				{
					d_tx_buffer_index = 0;
					if(d_number_of_session_packets==d_buffer_size)
					{
						d_tx_state = ST_TX_BUFFER_MANAGEMENT;
					}
					else
					{
						d_tx_state = ST_TX_END_PACKET_HEADER_TRANS;
					}
				}
			}
			break;
		}
		case ST_TX_END_PACKET_HEADER_TRANS:
		{
			if(d_trans_header_index<3)
			{
				out[no] = d_nodeA_id;
				d_trans_header_index++;
				no++;
				break;
			}
			if(d_trans_header_index>=3)
			{
				out[no] = 0xFF;
				d_trans_header_index++;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					d_tx_state = ST_TX_END_PACKET_NUMBER_TRANS;
				}
			}
			break;
		}
		case ST_TX_END_PACKET_NUMBER_TRANS:
		{
			out[no] = d_ended_packet_number;
			d_ended_packet_number_trans_index++;
			no++;
			if(d_ended_packet_number_trans_index==6)
			{
				d_ended_packet_number_trans_index = 0;
				d_tx_state = ST_TX_BUFFER_MANAGEMENT;
			}
		}
		case ST_TX_SESSION_NUMBER_TRANS:
		{
			unsigned char d_curr_session_no = (unsigned char) ((int)d_next_session_number-1);
			out[no] = d_curr_session_no;
			d_session_trans_index++;
			no++;
			if(d_session_trans_index==6)
			{
				d_session_trans_index = 0;
			}
			d_tx_state = ST_TX_HEADER_TRANS;
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
  } /* namespace DNC */
} /* namespace gr */

