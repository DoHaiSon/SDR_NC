
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
#include "nodeC_controller_impl.h"

namespace gr {
  namespace DNC {

    nodeC_controller::sptr
    nodeC_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding)
    {
      return gnuradio::get_initial_sptr
        (new nodeC_controller_impl(buffer_size, packet_size, guard_interval, nodeA_id, nodeB_id, nodeC_id, nodeR_id, network_coding));
    }

    /*
     * The private constructor
     */
    nodeC_controller_impl::nodeC_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding)
      : gr::block("nodeC_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make3(3, 3, sizeof(unsigned char), sizeof(unsigned char), sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), d_nodeA_id(nodeA_id),
	      d_nodeB_id(nodeB_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id), d_network_coding(network_coding), 
              
	      d_check_rx0_id_index(0), d_check_rx0_id_count(0), d_rx0_packet_number_index(0), d_rx0_check_ended_packet_index(0),
	      d_rx0_data_index(0), d_rx0_check_end_packet_count(0), d_rx0_check_end_packet_index(0), d_rx0_pkt_no(0x00),

	      d_check_rx1_id_index(0), d_check_rx1_id_count(0), d_rx1_packet_number_index(0), d_rx1_check_ended_packet_index(0),
	      d_rx1_data_index(0), d_rx1_check_end_packet_count(0), d_rx1_check_end_packet_index(0), d_rx1_pkt_no(0x00),
	      d_rx1_node_id(0x00),

	      d_nodeA_ended_packet_number(0x00), d_nodeB_ended_packet_number(0x00), d_next_session_number(0x01),
	      d_trans_header_index(0), d_packet_out_nodeA_index(0), d_packet_out_nodeB_index(0), d_packet_nodeA_out(false),
	      d_packet_nodeB_out(false), d_data_out_index(0), d_session_trans_index(0), 
	      d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_HEADER_TRANS)
    {
	if (buffer_size>254)
		throw std::runtime_error("Invalid parameter! Please let buffer size be less than 255...!\n");

	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please let guard interval be greater than or equal to 6...!\n");

	if (nodeA_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeA ID must be different from 0...!!!\n");

	if (nodeB_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeB ID must be different from 0...!!!\n");

	if (nodeC_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeC ID must be different from 0...!!!\n");

	if (nodeR_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeR ID must be different from 0...!!!\n");

	if (nodeA_id==nodeB_id||nodeA_id==nodeC_id||nodeA_id==nodeR_id||nodeB_id==nodeC_id||nodeB_id==nodeR_id||nodeC_id==nodeR_id)
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

	d_packet_out_nodeA_buffer.resize(buffer_size);
	std::fill(d_packet_out_nodeA_buffer.begin(), d_packet_out_nodeA_buffer.end(), 0x00);

	d_packet_out_nodeB_buffer.resize(buffer_size);
	std::fill(d_packet_out_nodeB_buffer.begin(), d_packet_out_nodeB_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx0_check_ended_packet_number[i] = 0x00;
		d_rx0_check_ended_packet_number[i+3] = 0x00;
		d_rx1_check_ended_packet_number[i] = 0x00;
		d_rx1_check_ended_packet_number[i+3] = 0x00;
		d_rx0_packet_number[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    nodeC_controller_impl::~nodeC_controller_impl()
    {
    }

    void
    nodeC_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    nodeC_controller_impl::general_work (int noutput_items,
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
        unsigned char *out2 = (unsigned char *) output_items[2];

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
							d_rx0_state  = ST_RX0_GET_DATA;
						}
					}
					else
					{
						d_rx0_state = ST_RX0_IDLE;
					}
				}
				else
				{
					d_rx0_state = ST_RX0_IDLE;
				}
			}
			break;
		}
		case ST_RX0_CHECK_ENDED_PACKET_NUMBER:
		{
			d_rx0_check_ended_packet_number[d_rx0_check_ended_packet_index] = in0[ni0];
			d_rx0_check_ended_packet_index++;
			ni0++;
			if(d_rx0_check_ended_packet_index==6)
			{
				d_rx0_check_ended_packet_index = 0;
				int check_count;
				unsigned char temp_pkt_no;
				for(int i = 0; i<6; i++)
				{
					temp_pkt_no = d_rx0_check_ended_packet_number[i];
					check_count = 0;
					for(int j = 0; j<6; j++)
					{
						if(temp_pkt_no==d_rx0_check_ended_packet_number[j])
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
			if(in1[ni1]==d_nodeR_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODE_ID;
				d_rx1_node_id = d_nodeR_id;
				d_check_rx1_id_index++;
				d_check_rx1_id_count++;
			}
			if(in1[ni1]==d_nodeA_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODE_ID;
				d_rx1_node_id = d_nodeA_id;
				d_check_rx1_id_index++;
				d_check_rx1_id_count++;
			}
			if(in1[ni1]==d_nodeB_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODE_ID;
				d_rx1_node_id = d_nodeB_id;
				d_check_rx1_id_index++;
				d_check_rx1_id_count++;
			}
			ni1++;
			break;
		}
		case ST_RX1_CHECK_NODE_ID:
		{
			if(in1[ni1]==d_rx1_node_id)
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
					if(rx1_pkt_no!=0x00) 
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
						d_rx1_state = ST_RX1_IDLE;
					}
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
				}
			}
			break;
		}
		case ST_RX1_CHECK_ENDED_PACKET_NUMBER:
		{
			d_rx1_check_ended_packet_number[d_rx1_check_ended_packet_index] = in1[ni1];
			d_rx1_check_ended_packet_index++;
			ni1++;
			if(d_rx1_check_ended_packet_index==6)
			{
				d_rx1_check_ended_packet_index = 0;
				int check_count;
				unsigned char temp_pkt_no;
				for(int i = 0; i<6; i++)
				{
					temp_pkt_no = d_rx1_check_ended_packet_number[i];
					check_count = 0;
					for(int j = 0; j<6; j++)
					{
						if(temp_pkt_no==d_rx1_check_ended_packet_number[j])
						check_count++;
					}
					if(check_count>4)
					{
						if(d_rx1_node_id==d_nodeA_id) { d_nodeA_ended_packet_number = temp_pkt_no; }
						if(d_rx1_node_id==d_nodeB_id) { d_nodeB_ended_packet_number = temp_pkt_no; }
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
					if(d_rx1_node_id==d_nodeR_id)
					{
						if(d_xored_packet_no_buffer[idx]==0x00)
						{
							d_xored_packet_no_buffer[idx] = d_rx1_pkt_no;
							if(d_packet_no_nodeA_buffer[idx]!=0x00&&d_packet_no_nodeB_buffer[idx]==0x00)
							{
								d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
								for(int k = 0; k<d_packet_size; k++)
								{
									int data_idx = idx*d_packet_size+k;
									d_data_nodeB_buffer[data_idx] = d_data_nodeA_buffer[data_idx]^
													d_rx1_packet[k];
								}
							}
						}
					}
					if(d_rx1_node_id==d_nodeA_id)
					{
						if(d_packet_no_nodeA_buffer[idx]==0x00)
						{
							d_packet_no_nodeA_buffer[idx] = d_rx1_pkt_no;
							for(int k = 0; k<d_packet_size; k++)
							{
								int data_idx = idx*d_packet_size+k;
								d_data_nodeA_buffer[data_idx] = d_rx1_packet[k];
							}
						}
					}
					if(d_rx1_node_id==d_nodeB_id)
					{
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
		case ST_CHECK_PACKET_OUT:
		{
			for(int i = 0; i<d_buffer_size; i++)
			{
				if(d_packet_out_nodeA_buffer[i]==0x00&&d_packet_no_nodeA_buffer[i]!=0x00)
				{
					d_packet_out_nodeA_index = i;
					d_packet_nodeA_out = true;
					d_tx_state = ST_PACKET_OUT;
					break;
				}
			}
			for(int j = 0; j<d_buffer_size; j++)
			{
				if(d_packet_out_nodeB_buffer[j]==0x00&&d_packet_no_nodeB_buffer[j]!=0x00)
				{
					d_packet_out_nodeB_index = j;
					d_packet_nodeA_out = true;
					d_tx_state = ST_PACKET_OUT;
					break;
				}
			}
			out0[no] = 0x00;
			if(d_packet_nodeA_out==true) { out1[no] = d_packet_no_nodeA_buffer[d_packet_out_nodeA_index]; }
			else			     { out1[no] = 0x00; }
			if(d_packet_nodeB_out==true) { out2[no] = d_packet_no_nodeB_buffer[d_packet_out_nodeB_index]; }
			else			     { out2[no] = 0x00; }
			no++;
			break;
		}
		case ST_PACKET_OUT:
		{
			out0[no] = 0x00;
			if(d_packet_nodeA_out==true)
			{
				out1[no] = d_data_nodeA_buffer[d_packet_out_nodeA_index*d_packet_size+d_data_out_index];
			}
			else
			{
				out1[no] = 0x00;
			}
			if(d_packet_nodeB_out==true)
			{
				out2[no] = d_data_nodeB_buffer[d_packet_out_nodeB_index*d_packet_size+d_data_out_index];
			}
			else
			{
				out2[no] = 0x00;
			}
			d_data_out_index++;
			no++;
			if(d_data_out_index==d_packet_size)
			{
				d_data_out_index = 0;
				if(d_packet_nodeA_out==true) { d_packet_out_nodeA_buffer[d_packet_out_nodeA_index] = 0x01; }
				if(d_packet_nodeB_out==true) { d_packet_out_nodeB_buffer[d_packet_out_nodeB_index] = 0x01; }
				d_packet_nodeA_out = false;
				d_packet_nodeB_out = false;
				int check_count = 0;
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_out_nodeA_buffer[d_packet_out_nodeA_index]!=0x00 &&
					   d_packet_out_nodeB_buffer[d_packet_out_nodeB_index]!=0x00)
					{
						check_count++;
					}
				}
				if(check_count==d_buffer_size)
				{
					for(int i = 0; i<d_buffer_size; i++)
					{
						d_packet_no_nodeA_buffer[i] = 0x00;
						d_packet_no_nodeB_buffer[i] = 0x00;
						d_xored_packet_no_buffer[i] = 0x00;
						d_packet_out_nodeA_buffer[i] = 0x00;
						d_packet_out_nodeB_buffer[i] = 0x00;
					}
					if(d_next_session_number==0xFF) { d_next_session_number = 0x01; }
					else			        { d_next_session_number++;	}
					d_tx_state = ST_HEADER_TRANS;
				}
			}
			break;
		}
		case ST_HEADER_TRANS:
		{
			if(d_trans_header_index<3)
			{
				out0[no] = d_nodeC_id;
				out1[no] = 0x00;
				out2[no] = 0x00;
				d_trans_header_index++;
				no++;
				break;
			}
			if(d_trans_header_index>=3)
			{
				out0[no] = 0x00;
				out1[no] = 0x00;
				out2[no] = 0x00;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					d_tx_state = ST_SESSION_NUMBER_TRANS;
				}
			}
			break;
		}
		case ST_SESSION_NUMBER_TRANS:
		{
			out0[no] = d_next_session_number;
			out1[no] = 0x00;
			out2[no] = 0x00;
			d_session_trans_index++;
			no++;
			if(d_session_trans_index==6)
			{
				d_session_trans_index = 0;
				d_tx_state = ST_HEADER_TRANS;
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_no_nodeA_buffer[i]!=0x00||d_packet_no_nodeB_buffer[i]!=0x00)
					{
						d_tx_state = ST_CHECK_PACKET_OUT;
						break;
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

