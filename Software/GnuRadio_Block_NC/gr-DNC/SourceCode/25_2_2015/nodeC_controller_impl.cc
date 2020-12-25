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
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), 
	      d_nodeA_id(nodeA_id), d_nodeB_id(nodeB_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id),
	      d_network_coding(network_coding),

	      d_send(true), d_tx_packet_number(0xFF), d_control_info_trans_index(0), d_shift(0), d_branch(1),
	      d_request_for_nodeA(false), d_request_for_nodeB(false), d_request_for_nodeC(false), 
	      d_max_xor_packet_number(buffer_size), d_min_sing_packet_number(buffer_size),
	      d_full_A(false), d_full_B(false), d_send_request_new_session(true),
              
	      d_check_rx0_id_index(0), d_check_rx0_id_count(0), d_rx0_packet_number_index(0), d_rx0_check_ended_packet_index(0),
	      d_rx0_data_index(0), d_rx0_check_end_packet_count(0), d_rx0_check_end_packet_index(0), d_rx0_pkt_no(0x00),
	      d_rx0_node_id(0x00),

	      d_check_rx1_id_index(0), d_check_rx1_id_count(0), d_rx1_packet_number_index(0), d_rx1_check_ended_packet_index(0),
	      d_rx1_data_index(0), d_rx1_check_end_packet_count(0), d_rx1_check_end_packet_index(0), d_rx1_pkt_no(0x00),
	      d_rx1_node_id(0x00), d_rx1_control_info_index(0),

	      d_nodeA_ended_packet_number(0x00), d_nodeB_ended_packet_number(0x00), d_trans_header_index(0), 
	      d_packet_out(false), d_data_out_index(0), d_packet_out_index(0),
	      d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_HEADER_TRANS), d_prt(0), d_no_rcvd_pkts(0)
    {
	std::cout<<"d_network_coding = "<<network_coding<<"\n";
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

	d_xored_data_buffer.resize(buffer_size*packet_size);
	std::fill(d_xored_data_buffer.begin(), d_xored_data_buffer.end(), 0x00);	

	d_packet_out_nodeA_buffer.resize(buffer_size);
	std::fill(d_packet_out_nodeA_buffer.begin(), d_packet_out_nodeA_buffer.end(), 0x00);

	d_packet_out_nodeB_buffer.resize(buffer_size);
	std::fill(d_packet_out_nodeB_buffer.begin(), d_packet_out_nodeB_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx0_packet_number[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
		d_rx1_control_info[i] = 0x00;
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

    void
    nodeC_controller_impl::reset()
    {
	d_rx0_state = ST_RX0_IDLE;
	d_check_rx0_id_index = 0;
	d_check_rx0_id_count = 0;
	d_rx0_packet_number_index = 0;
	d_rx0_data_index = 0;
	d_rx0_check_end_packet_index = 0;
	d_rx0_check_end_packet_count = 0;

	d_rx1_state = ST_RX1_IDLE;
	d_check_rx1_id_index = 0;
	d_check_rx1_id_count = 0;
	d_rx1_packet_number_index = 0;
	d_rx1_control_info_index = 0;
	d_rx1_data_index = 0;
	d_rx1_check_end_packet_index = 0;
	d_rx1_check_end_packet_count = 0;

	for(int i = 0; i<d_buffer_size; i++)
	{
		d_packet_no_nodeA_buffer[i]  = 0x00;
		d_packet_no_nodeB_buffer[i]  = 0x00;
		d_xored_packet_no_buffer[i]  = 0x00;
		d_packet_out_nodeA_buffer[i] = 0x00;
		d_packet_out_nodeB_buffer[i] = 0x00;
	}
	d_tx_packet_number  = 0xFF;
	d_request_for_nodeA = false;
	d_request_for_nodeC = false;
	d_packet_out = false;
	d_send = true;
	d_prt = 0;
	return;
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
				d_rx0_node_id = d_nodeA_id;
				d_rx0_state = ST_RX0_CHECK_NODE_ID;
				d_check_rx0_id_index++;
				d_check_rx0_id_count++;
			}
			if(in0[ni0]==d_nodeB_id)
			{
				d_rx0_node_id = d_nodeB_id;
				d_rx0_state = ST_RX0_CHECK_NODE_ID;
				d_check_rx0_id_index++;
				d_check_rx0_id_count++;
			}
			ni0++;
			break;
		}
		case ST_RX0_CHECK_NODE_ID:
		{
			if(in0[ni0]==d_rx0_node_id)
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
					if(rx0_pkt_no!=0x00&&rx0_pkt_no!=0xFF)
					{
						d_rx0_pkt_no = rx0_pkt_no;
						d_rx0_state  = ST_RX0_GET_DATA;
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
					if(idx>=d_buffer_size)
					{
						d_rx0_state = ST_RX0_IDLE;
						d_rx0_check_end_packet_count = 0;
						d_rx0_check_end_packet_index = 0;
						break;
					}
					if(d_rx0_node_id==d_nodeB_id)
					{
						std::cout<<"NodeB starts sending\n";
						d_branch = 1;
						d_send = false;
						d_rx0_state = ST_RX0_IDLE;
						break;
					}
					if(d_packet_no_nodeA_buffer[idx]==0x00)
					{
						d_no_rcvd_pkts++;
						std::cout<<d_no_rcvd_pkts<<": received packet number "<<(int)d_rx0_pkt_no<<" from nodeA\n";
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
					if(rx1_pkt_no!=0x00 && rx1_pkt_no!=0xFF && d_request_for_nodeC==true) 
					{
						d_rx1_pkt_no = rx1_pkt_no;
						d_rx1_state = ST_RX1_GET_DATA;
						//d_send = false; Let C transmit while receiving data from R.
					}
					else
					{
						d_rx1_pkt_no = rx1_pkt_no;
						d_rx1_state = ST_RX1_CHECK_CONTROL_INFO;
					}
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
				}
			}
			break;
		}
		case ST_RX1_CHECK_CONTROL_INFO:
		{
			d_rx1_control_info[d_rx1_control_info_index] = in1[ni1];
			d_rx1_control_info_index++;
			ni1++;
			if(d_rx1_control_info_index==3)
			{
				d_rx1_control_info_index = 0;
				unsigned char a, b, c;
				unsigned char rx1_control_info = 0x00;
				bool recv_control_info = false;
				a = d_rx1_control_info[0];
				b = d_rx1_control_info[1];
				c = d_rx1_control_info[2];
				if(a==b && b==c) { rx1_control_info = a; recv_control_info = true;}
				if(recv_control_info == true)
				{
					if(d_rx1_node_id==d_nodeR_id && d_rx1_pkt_no==0x00 && rx1_control_info==d_nodeA_id)
					{
						d_request_for_nodeA = true;
						ni1 = nInputItems1;
					}
					if(d_rx1_node_id==d_nodeR_id && d_rx1_pkt_no==0x00 && rx1_control_info==d_nodeB_id)
					{
						d_request_for_nodeB = true;
					}
					if(d_rx1_node_id==d_nodeR_id && d_rx1_pkt_no==0x00 && rx1_control_info==d_nodeC_id)
					{
						if(d_prt==0)
						{
							std::cout<<"received request for node C\n";
							d_prt++;
						}
						d_request_for_nodeC = true;
						d_tx_state = ST_HEADER_TRANS;
						d_tx_packet_number = 0x00;
						d_branch = 1;
						d_send = true;
					}
					if(d_rx1_node_id==d_nodeA_id && d_rx1_pkt_no==0xFF)
					{
						d_nodeA_ended_packet_number = rx1_control_info;
					}
					if(d_rx1_node_id==d_nodeB_id && d_rx1_pkt_no==0xFF)
					{
						d_nodeB_ended_packet_number = rx1_control_info;
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
							for(int j = 0; j<d_packet_size; j++)
							{
								int data_idx = idx*d_packet_size+j;
								d_xored_data_buffer[data_idx] = d_rx1_packet[j];
							}
							if(d_packet_no_nodeA_buffer[idx]!=0x00 && d_packet_no_nodeB_buffer[idx]==0x00)
							{
								d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
								for(int k = 0; k<d_packet_size; k++)
								{
									int data_idx = idx*d_packet_size+k;
									d_data_nodeB_buffer[data_idx] = 
									d_data_nodeA_buffer[data_idx]^d_rx1_packet[k];
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
						if(d_network_coding==1)
						{
							if((int)d_rx1_pkt_no<=d_min_sing_packet_number)
							{
								d_min_sing_packet_number = (int)d_rx1_pkt_no;
							}
							int check_idx = d_min_sing_packet_number-2;
							if(d_xored_packet_no_buffer[check_idx]!=0x00)
							{
								d_max_xor_packet_number = d_min_sing_packet_number-1;
							}
							if(d_xored_packet_no_buffer[idx]!=0x00 && d_packet_no_nodeB_buffer[idx]==0x00)
							{
								d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
								for(int k = 0; k<d_packet_size; k++)
								{
									int data_idx = idx*d_packet_size+k;
									d_data_nodeB_buffer[data_idx] = 
									d_xored_data_buffer[data_idx]^d_rx1_packet[k];
								}
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
						if(d_network_coding==1)
						{
							if((int)d_rx1_pkt_no<=d_min_sing_packet_number)
							{
								d_min_sing_packet_number = (int)d_rx1_pkt_no;
							}
							int check_idx = d_min_sing_packet_number-2;
							if(d_xored_packet_no_buffer[check_idx]!=0x00)
							{
								d_max_xor_packet_number = d_min_sing_packet_number-1;
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
		if(d_request_for_nodeC==true)
		{
			if(d_packet_out==false)
			{
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_out_nodeA_buffer[i]==0x00 && d_packet_no_nodeA_buffer[i]!=0x00)
					{
						d_packet_out_index = i;
						d_shift = 0;
						d_packet_out = true;
						break;
					}
					if(d_packet_out_nodeB_buffer[i]==0x00 && d_packet_no_nodeB_buffer[i]!=0x00)
					{
						d_packet_out_index = i;
						d_shift = 1;
						d_packet_out = true;
						break;
					}
				}
			}

			if(d_packet_out==false)
			{ 
				out2[no] = 0x00;
			}
			else
			{
				if(d_data_out_index==0) 
				{
					if(d_shift==0) { out2[no] = d_nodeA_id; }
					if(d_shift==1) { out2[no] = d_nodeB_id; }
					d_data_out_index++;
				}
				else 
				{
					if (d_data_out_index==1)
					{
						if(d_shift==0)
						{
							out2[no] = d_packet_no_nodeA_buffer[d_packet_out_index];
						}
						if(d_shift==1)
						{
							out2[no] = d_packet_no_nodeB_buffer[d_packet_out_index];
						}
						d_data_out_index++;
					}
					else
					{
						int idx = d_packet_out_index*d_packet_size+d_data_out_index-2;
						if(d_shift==0)
						{
							out2[no] = d_data_nodeA_buffer[idx];
						}
						if(d_shift==1)
						{
							out2[no] = d_data_nodeB_buffer[idx];
						}
						d_data_out_index++;
						if(d_data_out_index==(d_buffer_size+2))
						{
							d_data_out_index = 0;
							if(d_shift==0)
							{
								d_packet_out_nodeA_buffer[d_packet_out_index]==0x01;
							}
							if(d_shift==1)
							{
								d_packet_out_nodeB_buffer[d_packet_out_index]==0x01;
							}
							d_packet_out = false;
						}
					}
				}

				if(d_packet_out==false)
				{
					if(d_network_coding==1)
					{
						// With Network Coding
						int check_count = 0;
						for(int i = 0; i<d_max_xor_packet_number; i++)
						{
							if(d_packet_out_nodeA_buffer[i]!=0x00 &&
							   d_packet_out_nodeB_buffer[i]!=0x00)
							{
								check_count++;
							}
						}
						if(check_count==d_max_xor_packet_number)
						{
							int diff = d_buffer_size-d_max_xor_packet_number;
							int c = 0;
							for(int j = 0; j<diff; j++)
							{
								int index = j+d_max_xor_packet_number;
								if(d_packet_out_nodeA_buffer[index]!=0x00 || 
								   d_packet_out_nodeB_buffer[index]!=0x00)
								{
									c++;
								}
							}
							if(c==diff)
							{
								// Need to start a new session :-)
								d_send_request_new_session = true;
							}
						}
					}
					else
					{
						// Without Network Coding
						bool full_A = false;
						bool full_B = false;
						for(int run = 0 ; run<2; run++)
						{
							unsigned char d_ended_packet_number = 0x00;

							if(run==0) { d_ended_packet_number = d_nodeA_ended_packet_number; }
							if(run==1) { d_ended_packet_number = d_nodeB_ended_packet_number; }

							if(d_ended_packet_number==0x00)
							{
								int check_count = 0;
								for(int i = 0; i<d_buffer_size; i++)
								{
									if((run==0 && d_packet_out_nodeA_buffer[i]!=0x00) ||
									   (run==1 && d_packet_out_nodeB_buffer[i]!=0x00))
									{
										check_count++;
									}
								}
								if(check_count==d_buffer_size)
								{
									if(run==0) { full_A = true; }
									if(run==1) { full_B = true; }
								}
								if(d_full_A==true) { full_A = true; }
								if(d_full_B==true) { full_B = true; }
							}
							else
							{
								int check_count = 0;
								int upper_bound = (int)d_ended_packet_number;
								for(int i = 0; i<upper_bound; i++)
								{
									if((run==0 && d_packet_out_nodeA_buffer[i]!=0x00) ||
									   (run==1 && d_packet_out_nodeB_buffer[i]!=0x00))
									{
										check_count++;
									}
								}
								if(check_count==upper_bound)
								{
									if(run==0) { full_A = true; }
									if(run==1) { full_B = true; }
								}
								if(d_full_A==true) { full_A = true; }
								if(d_full_B==true) { full_B = true; }
							}
						}

						if(full_A==true && full_B==true)
						{
							if(d_nodeA_ended_packet_number!=0x00) { d_full_A = true; }
							if(d_nodeB_ended_packet_number!=0x00) { d_full_B = true; }
							// Need to start a new session :-)
							d_send_request_new_session = true;
						}
					}
				}
			}
		}
		else
		{
			out2[no] = 0x00;
		}

		switch(d_tx_state)
		{
			case ST_HEADER_TRANS:
			{
				if(d_trans_header_index<3)
				{
					out0[no] = d_nodeC_id;
					if(d_branch==1)
					{
						if(d_send==true) { out1[no] = 0x01; }
						else		 { out1[no] = 0x02; }
					}
					else
					{
						out1[no] = 0x00;
					}
					d_trans_header_index++;
					no++;
					break;
				}
				if(d_trans_header_index>=3)
				{
					out0[no] = d_tx_packet_number;
					if(d_branch==1)
					{
						if(d_send==true) { out1[no] = 0x01; }
						else		 { out1[no] = 0x02; }
					}
					else
					{
						out1[no] = 0x00;
					}
					d_trans_header_index++;
					no++;
					if(d_trans_header_index==6)
					{
						d_trans_header_index = 0;
						if(d_tx_packet_number==0x00||d_tx_packet_number==0xFF)
						{
							d_tx_state = ST_CONTROL_INFO_TRANS;
						}
						else
						{
							if(d_send_request_new_session==true)
							{
								// Start a new session (RESET)
								if(d_max_xor_packet_number<d_buffer_size)
								{
									d_max_xor_packet_number = 0;
								}
								reset();
								break;
							}
							for(int j = 0; j<d_max_xor_packet_number; j++)
							{
								int idx = j+(int)d_tx_packet_number;
								if(d_packet_no_nodeA_buffer[idx]==0x00)
								{
									d_tx_packet_number = d_xored_packet_no_buffer[idx];
									d_tx_state = ST_HEADER_TRANS;
									break;
								}
							}
						}
					}
				}
				break;
			}
			case ST_CONTROL_INFO_TRANS:
			{
				out0[no] = d_nodeR_id;
				if(d_branch==1)
				{
					if(d_send==true) { out1[no] = 0x01; }
					else		 { out1[no] = 0x02; }
				}
				else
				{
					out1[no] = 0x00;
				}
				d_control_info_trans_index++;
				no++;
				if(d_control_info_trans_index==3)
				{
					d_control_info_trans_index = 0;
					d_tx_state = ST_HEADER_TRANS;
					if(d_tx_packet_number==0x00)
					{
						if(d_network_coding==1)
						{
							int check_count = 0;
							for(int i = 0; i<d_max_xor_packet_number; i++)
							{
								if(d_xored_packet_no_buffer[i]!=0x00)
								{
									check_count++;
								}
							}
							if(check_count==d_max_xor_packet_number)
							{
								int diff = d_buffer_size-d_max_xor_packet_number;
								int c = 0;
								for(int j = 0; j<diff; j++)
								{
									int idx = j + d_max_xor_packet_number;
									if(d_packet_no_nodeA_buffer[idx]!=0x00 || 
									   d_packet_no_nodeB_buffer[idx]!=0x00)
									{
										c++;
									}
								}
								if(c==diff)
								{
									// Request node R to REtransmit missed packets of node A
									if(d_max_xor_packet_number!=0)
									{
										d_send = true;
										d_tx_state = ST_HEADER_TRANS;
										for(int j = 0; j<d_max_xor_packet_number; j++)
										{
											if(d_packet_no_nodeA_buffer[j]==0x00)
											{
												d_tx_packet_number = 
												d_xored_packet_no_buffer[j];
												break;
											}
										}
									}
									else
									{
										// Remain data in one node only
										if(d_send_request_new_session==true)
										{
											// Start a new session (RESET)
											reset();
											d_tx_state = ST_HEADER_TRANS;
											break;
										}
									}
								}
							}
						}
						else
						{
							if(d_send_request_new_session==true)
							{
								// Start a new session (RESET)
								reset();
								d_tx_state = ST_HEADER_TRANS;
								break;
							}
						}
					}
					if(d_tx_packet_number==0xFF)
					{
						if(d_network_coding==1)
						{
							// remain data in both nodes A and B
							if(d_request_for_nodeA==true && d_max_xor_packet_number!=0)
							{
								std::cout<<"received request for node A\n";
								d_send_request_new_session = false;
								d_tx_state = ST_FREE;
								d_branch = 0;
							}
							// remain data in node A only
							if(d_request_for_nodeA==true && d_max_xor_packet_number==0)
							{
								std::cout<<"received request for node A\n";
								d_send_request_new_session = false;
								d_tx_state = ST_FREE;
								d_branch = 1;
								d_send = false;
							}
							// remain data in node B only
							if(d_request_for_nodeB==true && d_max_xor_packet_number==0)
							{
								std::cout<<"received request for node B\n";
								d_send_request_new_session = false;
								d_tx_state = ST_FREE;
								d_branch = 1;
								d_send = false;
							}
						}
						else
						{
							// remain data in both nodes A and B
							if(d_request_for_nodeA==true && d_full_B==false)
							{
								std::cout<<"received request for node A\n";
								d_send_request_new_session = false;
								d_tx_state = ST_FREE;
								d_branch = 0;
							}
							// remain data in node A only
							if(d_request_for_nodeA==true && d_full_B==true)
							{
								std::cout<<"received request for node A\n";
								d_send_request_new_session = false;
								d_tx_state = ST_FREE;
								d_branch = 1;
								d_send = false;
							}
							// remain data in one B only
							if(d_request_for_nodeB==true && d_full_A==true)
							{
								std::cout<<"received request for node B\n";
								d_send_request_new_session = false;
								d_tx_state = ST_FREE;
								d_branch = 1;
								d_send = false;
							}
						}
					}
				}
				break;
			}
			case ST_FREE:
			{
				out0[no] = 0x00;
				if(d_branch==1)
				{
					if(d_send==true) { out1[no] = 0x01; }
					else		 { out1[no] = 0x02; }
				}
				else
				{
					out1[no] = 0x00;
				}
				no++;
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

