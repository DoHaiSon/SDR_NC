
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
#include "nodeR_controller_impl.h"

namespace gr {
  namespace DNC {

    nodeR_controller::sptr
    nodeR_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeR_id, int network_coding )
    {
      return gnuradio::get_initial_sptr
        (new nodeR_controller_impl(buffer_size, packet_size, guard_interval, nodeA_id, nodeB_id, nodeR_id, network_coding));
    }

    /*
     * The private constructor
     */
    nodeR_controller_impl::nodeR_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeR_id, int network_coding)
      : gr::block("nodeR_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), d_nodeA_id(nodeA_id),
	      d_nodeB_id(nodeB_id), d_nodeR_id(nodeR_id), d_network_coding(network_coding),
	      d_check_rx1_id_count(0), d_check_rx1_id_index(0), d_rx1_packet_number_index(0),
	      d_next_session_number(0x01), d_check_rx1_session_index(0), d_check_rx1_session_count(0),
	      d_trans_header_index(0), d_node_id(0x00), d_tx_packet_number(0x00), d_session_trans_index(0),
	      d_buffer_index(0), d_check_rx0_id_index(0), d_check_rx0_id_count(0), d_rx0_packet_number_index(0),
	      d_rx0_pkt_no(0x00), d_rx0_data_index(0), d_rx0_check_end_packet_count(0), d_rx0_check_end_packet_index(0),
	      d_nodeA_ended_packet_number(0x00), d_nodeB_ended_packet_number(0x00), d_check_ended_packet_idx(0),
	      d_rx1_node_id(0x00), d_check_rx0_session_count(0), d_check_rx0_session_index(0), d_count(0),
	      d_tx_node_id(0x00), d_data_trans_index(0), d_rx1_pkt_no(0x00), d_rx1_check_end_packet_count(0),
	      d_rx1_check_end_packet_index(0), d_rx1_data_index(0), 
	      d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_INIT)
    {
	if (buffer_size>254)
		throw std::runtime_error("Invalid parameter! Please let buffer size be less than 255...!\n");

	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please let guard interval be greater than or equal to 6...!\n");

	if (nodeA_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeA ID must be different from 0...!!!\n");

	if (nodeB_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeB ID must be different from 0...!!!\n");

	if (nodeR_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeR ID must be different from 0...!!!\n");

	if (nodeA_id==nodeB_id||nodeB_id==nodeR_id||nodeA_id==nodeR_id)
		throw std::runtime_error("Invalid parameter! Node IDs must be different from each other...!!!\n");

	d_rx0_packet.resize(packet_size);
	std::fill(d_rx0_packet.begin(), d_rx0_packet.end(), 0x00);

	d_rx1_packet.resize(packet_size);
	std::fill(d_rx1_packet.begin(), d_rx1_packet.end(), 0x00);

	d_packet_no_nodeA_buffer.resize(buffer_size);
	std::fill(d_packet_no_nodeA_buffer.begin(), d_packet_no_nodeA_buffer.end(), 0x00);

	d_data_nodeA_buffer.resize(buffer_size*packet_size);
	std::fill(d_data_nodeA_buffer.begin(), d_data_nodeA_buffer.end(), 0x00);

	d_packet_no_nodeB_buffer.resize(buffer_size);
	std::fill(d_packet_no_nodeB_buffer.begin(), d_packet_no_nodeB_buffer.end(), 0x00);

	d_data_nodeB_buffer.resize(buffer_size*packet_size);
	std::fill(d_data_nodeB_buffer.begin(), d_data_nodeB_buffer.end(), 0x00);

	d_xored_packet_no_buffer.resize(buffer_size);
	std::fill(d_xored_packet_no_buffer.begin(), d_xored_packet_no_buffer.end(), 0x00);

	d_xored_data_buffer.resize(buffer_size*packet_size);
	std::fill(d_xored_data_buffer.begin(), d_xored_data_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_check_ended_packet_number[i] = 0x00;
		d_check_ended_packet_number[i+3] = 0x00;
		d_rx0_packet_number[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    nodeR_controller_impl::~nodeR_controller_impl()
    {
    }

    void
    nodeR_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    nodeR_controller_impl::general_work (int noutput_items,
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

	while(ni0<nInputItems0)
	{
		switch(d_rx0_state)
		{
		case ST_RX0_IDLE:
		{
			if(in0[ni0]==d_nodeA_id)
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
			if(in0[ni0]==d_nodeA_id)
			{
				d_check_rx0_id_count++;
			}
			ni0++;
			d_check_rx0_id_index++;
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
			d_rx0_packet_number[d_rx0_packet_number_index] = in0[ni0];
			d_rx0_packet_number_index++;
			ni0++;
			if(d_rx0_packet_number_index==3)
			{
				d_rx0_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx0_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx0_packet_number[0];
				b = d_rx0_packet_number[1];
				c = d_rx0_packet_number[2];
				if(a==b) { rx0_pkt_no = a; recv_pkt = true; }
				else 
				{
					if(b==c) { rx0_pkt_no = b; recv_pkt = true; }
					else
					{
						if(c==a) { rx0_pkt_no = c; recv_pkt = true; }
					}
				}
				if(recv_pkt == true)
				{
					if(rx0_pkt_no!=0)
					{
						if(rx0_pkt_no==0xFF)
						{
							d_rx0_state = ST_RX0_CHECK_ENDED_PACKET_NUMBER;
						}
						else
						{
							d_rx0_pkt_no = rx0_pkt_no;
							d_rx0_state = ST_RX0_GET_DATA;
						}
					}
					else
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
				d_check_rx0_session_count++;
			}
			d_check_rx0_session_index++;
			ni0++;
			if(d_check_rx0_session_index==6)
			{
				if(d_check_rx0_session_count==6)
				{
					std::cout<<"Received Request for Node R\n";
					if(d_next_session_number==0xFF)
					{
						d_next_session_number = 0x01;
					}
					else
					{
						d_next_session_number++;
					}
					d_tx_node_id = d_nodeR_id;
					d_tx_packet_number = 0x00;
					d_tx_state = ST_HEADER_TRANS;
					for(int i = 0; i<d_buffer_size; i++)
					{
						d_packet_no_nodeA_buffer[i] = 0x00;
						d_packet_no_nodeB_buffer[i] = 0x00;
						d_xored_packet_no_buffer[i] = 0x00;
					}
					//need to reset here
					d_trans_header_index = 0;
					d_session_trans_index = 0;
					d_data_trans_index = 0;
					d_buffer_index = 0;
				}
				d_rx1_state = ST_RX1_IDLE;
				d_check_rx1_session_index = 0;
				d_check_rx1_session_count = 0;
			}
			break;
		}
		case ST_RX0_CHECK_ENDED_PACKET_NUMBER:
		{
			d_check_ended_packet_number[d_check_ended_packet_idx] = in0[ni0];
			d_check_ended_packet_idx++;
			ni0++;
			if(d_check_ended_packet_idx==6)
			{
				d_check_ended_packet_idx = 0;
				int check_count;
				unsigned char temp_pkt_no;
				for(int i = 0; i<6; i++)
				{
					temp_pkt_no = d_check_ended_packet_number[i];
					check_count = 0;
					for(int j = 0; j<6; j++)
					{
						if(temp_pkt_no==d_check_ended_packet_number[j])
						check_count++;
					}
					if(check_count>4)
					{
						d_nodeA_ended_packet_number = temp_pkt_no;
						break;
					}
				}
				d_rx0_state = ST_RX0_IDLE;
			}
			break;
		}
		case ST_RX0_GET_DATA:
		{
			d_rx0_packet[d_rx0_data_index] = in0[ni0];
			d_rx0_data_index++;
			ni0++;
			if(d_rx0_data_index==d_packet_size)
			{
				d_rx0_data_index = 0;
				d_rx0_state = ST_RX0_CHECK_END_PACKET;
			}
			break;
		}
		case ST_RX0_CHECK_END_PACKET:
		{
			if(in0[ni0]==0x00)
			{
				d_rx0_check_end_packet_count++;
			}
			d_rx0_check_end_packet_index++;
			ni0++;
			if(d_rx0_check_end_packet_index==6)
			{
				if(d_rx0_check_end_packet_count>4)
				{
					int idx = (int)d_rx0_pkt_no-1;
					// just changed here. remember this
					if(d_packet_no_nodeA_buffer[idx]==0x00)
					{
						d_packet_no_nodeA_buffer[idx] = d_rx0_pkt_no;
						for(int k = 0; k<d_packet_size; k++)
						{
							int data_idx = idx*d_packet_size+k;
							d_data_nodeA_buffer[data_idx] = d_rx0_packet[k];
						}
					}
				}
				d_rx0_state = ST_RX0_IDLE;
				d_rx0_check_end_packet_count = 0;
				d_rx0_check_end_packet_index = 0;				
			}
			break;
		}
		}
	}

	while(ni1<nInputItems1)
	{
		switch(d_rx1_state)
		{
		case ST_RX1_IDLE:
		{
			if(in1[ni1]==d_nodeB_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODE_ID;
				d_check_rx1_id_index++;
				d_check_rx1_id_count++;
			}
			ni1++;
			break;
		}
		case ST_RX1_CHECK_NODE_ID:
		{
			if(in1[ni1]==d_nodeA_id)
			{
				d_check_rx1_id_count++;
			}
			ni1++;
			d_check_rx1_id_index++;
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
			d_rx1_packet_number[d_rx1_packet_number_index] = in1[ni1];
			d_rx1_packet_number_index++;
			ni1++;
			if(d_rx1_packet_number_index==3)
			{
				d_rx1_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx1_pkt_no = 0x00;
				bool recv_pkt = false;
				a = d_rx1_packet_number[0];
				b = d_rx1_packet_number[1];
				c = d_rx1_packet_number[2];
				if(a==b) { rx1_pkt_no = a; recv_pkt = true; }
				else 
				{
					if(b==c) { rx1_pkt_no = b; recv_pkt = true; }
					else
					{
						if(c==a) { rx1_pkt_no = c; recv_pkt = true; }
					}
				}
				if(recv_pkt == true)
				{
					if(rx1_pkt_no!=0)
					{
						if(rx1_pkt_no==0xFF)
						{
							d_rx1_state = ST_RX1_CHECK_ENDED_PACKET_NUMBER;
						}
						else
						{
							d_rx1_pkt_no = rx1_pkt_no;
							d_rx1_state = ST_RX1_GET_DATA;
						}
					}
					else
					{
						d_rx1_state = ST_RX1_CHECK_SESSION_NUMBER;
					}
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
				}
			}
			break;
		}
		case ST_RX1_CHECK_SESSION_NUMBER:
		{
			if(in1[ni1]==d_next_session_number)
			{
				d_check_rx1_session_count++;
			}
			d_check_rx1_session_index++;
			ni1++;
			if(d_check_rx1_session_index==6)
			{
				if(d_check_rx1_session_count==6)
				{
					std::cout<<"Received Request for Node R\n";
					if(d_next_session_number==0xFF)
					{
						d_next_session_number = 0x01;
					}
					else
					{
						d_next_session_number++;
					}
					d_tx_node_id = d_nodeR_id;
					d_tx_packet_number = 0x00;
					d_tx_state = ST_HEADER_TRANS;
					for(int i = 0; i<d_buffer_size; i++)
					{
						d_packet_no_nodeA_buffer[i] = 0x00;
						d_packet_no_nodeB_buffer[i] = 0x00;
						d_xored_packet_no_buffer[i] = 0x00;
					}
					//need to reset here
					d_trans_header_index = 0;
					d_session_trans_index = 0;
					d_data_trans_index = 0;
					d_buffer_index = 0;
				}
				d_rx1_state = ST_RX1_IDLE;
				d_check_rx1_session_index = 0;
				d_check_rx1_session_count = 0;
			}
			break;
		}
		case ST_RX1_CHECK_ENDED_PACKET_NUMBER:
		{
			d_check_ended_packet_number[d_check_ended_packet_idx] = in1[ni1];
			d_check_ended_packet_idx++;
			ni1++;
			if(d_check_ended_packet_idx==6)
			{
				d_check_ended_packet_idx = 0;
				int check_count;
				unsigned char temp_pkt_no;
				for(int i = 0; i<6; i++)
				{
					temp_pkt_no = d_check_ended_packet_number[i];
					check_count = 0;
					for(int j = 0; j<6; j++)
					{
						if(temp_pkt_no==d_check_ended_packet_number[j])
						check_count++;
					}
					if(check_count>4)
					{
						d_nodeB_ended_packet_number = temp_pkt_no;
						break;
					}
				}
				d_rx1_state = ST_RX1_IDLE;
			}
			break;
		}
		case ST_RX1_GET_DATA:
		{
			d_rx1_packet[d_rx1_data_index] = in1[ni1];
			d_rx1_data_index++;
			ni1++;
			if(d_rx1_data_index==d_packet_size)
			{
				d_rx1_data_index = 0;
				d_rx1_state = ST_RX1_CHECK_END_PACKET;
			}
			break;
		}
		case ST_RX1_CHECK_END_PACKET:
		{
			if(in1[ni1]==0x00)
			{
				d_rx1_check_end_packet_count++;
			}
			d_rx1_check_end_packet_index++;
			ni1++;
			if(d_rx1_check_end_packet_index==6)
			{
				if(d_rx1_check_end_packet_count>4)
				{
					int idx = (int)d_rx1_pkt_no-1;
					// just changed here. remember this
					if(d_packet_no_nodeB_buffer[idx]==0x00)
					{
						d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
						for(int k = 0; k<d_packet_size; k++)
						{
							int data_idx = idx*d_packet_size+k;
							d_data_nodeB_buffer[data_idx] = d_rx1_packet[k];
						}
					}
				}
				d_rx1_state = ST_RX1_IDLE;
				d_rx1_check_end_packet_count = 0;
				d_rx1_check_end_packet_index = 0;				
			}
			break;
		}
		}
	}

	while(no<nOutputItems)
	{
		switch(d_tx_state)
		{
		case ST_INIT:
		{
			out[no] = 0x00;
			no++;
			break;
		}
		case ST_MANAGE_DATA_TRANS:
		{
			if(d_network_coding==1)
			{
				/* Using network coding method */
				if(d_xored_packet_no_buffer[d_buffer_index]!=0x00)
				{
						d_tx_node_id = d_nodeR_id;
						d_tx_packet_number = d_xored_packet_no_buffer[d_buffer_index];
						d_tx_state = ST_HEADER_TRANS;
				}
				else
				{
					if(d_packet_no_nodeA_buffer[d_buffer_index]!=0x00&&
					   d_packet_no_nodeB_buffer[d_buffer_index]!=0x00)
					{
						for(int i = 0; i<d_packet_size; i++)
						{
							d_xored_data_buffer[d_buffer_index*d_packet_size+i] = 
							d_data_nodeA_buffer[d_buffer_index*d_packet_size+i]^
							d_data_nodeB_buffer[d_buffer_index*d_packet_size+i];
						}
						d_xored_packet_no_buffer[d_buffer_index] = d_packet_no_nodeA_buffer[d_buffer_index];
						d_tx_node_id = d_nodeR_id;
						d_tx_packet_number = d_xored_packet_no_buffer[d_buffer_index];
						d_tx_state = ST_HEADER_TRANS;
					}
					else
					{
						d_buffer_index++;
						if(d_buffer_index==d_buffer_size) { d_buffer_index = 0; }
						out[no] = 0x00;
						no++;
					}
				}
			}
			else
			{
				/* Without using network coding method */
				if(d_count==0)
				{
					if(d_packet_no_nodeA_buffer[d_buffer_index]!=0x00)
					{
						d_tx_node_id = d_nodeA_id;
						d_tx_packet_number = d_packet_no_nodeA_buffer[d_buffer_index];
						d_tx_state = ST_HEADER_TRANS;
					}
					else
					{
						d_count = 1;
						out[no] = 0x00;
						no++;
					}
				}
				else
				{
					if(d_packet_no_nodeB_buffer[d_buffer_index]!=0x00)
					{
						d_tx_node_id = d_nodeB_id;
						d_tx_packet_number = d_packet_no_nodeB_buffer[d_buffer_index];
						d_tx_state = ST_HEADER_TRANS;
					}
					else
					{
						d_count = 0;
						d_buffer_index++;
						if(d_buffer_index==d_buffer_size) { d_buffer_index = 0; }
						out[no] = 0x00;
						no++;
					}
				}
			}
			break;
		}
		case ST_HEADER_TRANS:
		{
			if(d_trans_header_index<3)
			{
				out[no] = d_tx_node_id;
				d_trans_header_index++;
				no++;
				break;
			}
			if(d_trans_header_index>=3)
			{
				out[no] = d_tx_packet_number;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					if(d_tx_packet_number==0x00) 
					{ 
						d_tx_state = ST_SESSION_NUMBER_TRANS; 
					}
					else
					{
						d_tx_state = ST_DATA_TRANS;
					}
				}
			}
			break;
		}
		case ST_SESSION_NUMBER_TRANS:
		{
			unsigned char d_curr_session_no = (unsigned char) ((int)d_next_session_number-1);
			out[no] = d_curr_session_no;
			d_session_trans_index++;
			no++;
			if(d_session_trans_index==6)
			{
				d_session_trans_index = 0;
				bool c1 = false;
				bool c2 = false;
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_no_nodeA_buffer[i]!=0x00)
					{
						c1 = true;
						break;
					}
				}
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_no_nodeB_buffer[i]!=0x00)
					{
						c2 = true;
						break;
					}
				}
				if((c1==true&&c2==true)||(c1==true&&d_nodeB_ended_packet_number!=0x00)||
							 (c2==true&&d_nodeA_ended_packet_number!=0x00))
				{
					d_tx_state = ST_MANAGE_DATA_TRANS;
				}
				else
				{
					d_tx_state = ST_HEADER_TRANS;
				}
			}
			break;
		}
		case ST_DATA_TRANS:
		{
			if(d_network_coding==1)
			{
				/* Using network coding method */
				out[no] = d_xored_data_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
				d_data_trans_index++;
				no++;
				if(d_data_trans_index==d_packet_size)
				{
					d_data_trans_index = 0;
					d_buffer_index++;
					if(d_buffer_index==d_buffer_size) { d_buffer_index = 0; }
					d_tx_state = ST_MANAGE_DATA_TRANS;
				}
			}
			else
			{
				/* Without using network coding method */
				if(d_count==0)
				{
					// forward nodeA's packet
					out[no] = d_data_nodeA_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
					d_data_trans_index++;
					no++;
					if(d_data_trans_index==d_packet_size)
					{
						d_data_trans_index = 0;
						d_count = 1;
						d_tx_state = ST_MANAGE_DATA_TRANS;
					}
				}
				else
				{
					// forward nodeB's packet
					out[no] = d_data_nodeB_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
					d_data_trans_index++;
					no++;
					if(d_data_trans_index==d_packet_size)
					{
						d_data_trans_index = 0;
						d_count = 0;
						d_buffer_index++;
						if(d_buffer_index==d_buffer_size) { d_buffer_index = 0; }
						d_tx_state = ST_MANAGE_DATA_TRANS;
					}
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

        // Tell runtime system how many output items we produced.
        return no;
    }
  } /* namespace DNC */
} /* namespace gr */

