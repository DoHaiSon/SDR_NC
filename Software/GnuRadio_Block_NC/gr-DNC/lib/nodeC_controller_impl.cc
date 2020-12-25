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
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), 
	      d_nodeA_id(nodeA_id), d_nodeB_id(nodeB_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id),
	      d_network_coding(network_coding),

	      d_tx_node_id(0xFF), d_tx_packet_number(0xFF), d_shift(0), d_max_xor_packet_number(d_buffer_size),
	      d_full_A(false), d_full_B(false), d_trans_header_index(0), d_rcvd_xor_data(false), d_rcvd_nodeA_data(false), 
	      d_rcvd_nodeB_data(false), d_request_to_start_new_session(false),
	      d_tx_session_number_index(0), d_next_session_number(0x01), d_nodeA_ended_packet_number(0x00), 
	      d_nodeB_ended_packet_number(0x00), d_packet_out(false), d_data_out_index(0), d_packet_out_index(0),
              
	      d_check_rx0_id_index(0), d_check_rx0_id_count(0), d_rx0_packet_number_index(0), d_rx0_check_ended_packet_index(0),
	      d_rx0_data_index(0), d_rx0_check_end_packet_count(0), d_rx0_check_end_packet_index(0), d_rx0_pkt_no(0x00),
	      d_rx0_node_id(0x00),

	      d_check_rx1_id_index(0), d_check_rx1_id_count(0), d_rx1_packet_number_index(0), d_rx1_check_ended_packet_index(0),
	      d_rx1_data_index(0), d_rx1_check_end_packet_count(0), d_rx1_check_end_packet_index(0), d_rx1_pkt_no(0x00),
	      d_rx1_node_id(0x00), d_rx1_ended_packet_number_index(0),

	      d_ctrl(0),
	      d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_HEADER_TRANS), 

              d_nodeA_no_rcvd_pkts(0), d_nodeB_no_rcvd_pkts(0), d_xor_no_rcvd_pkts(0)
    {
	if(network_coding==1)
	{
		std::cout<<"Using Network Coding method\n";
	}
	else
	{
		std::cout<<"Not Using Network Coding method\n";
	}
	if (buffer_size>127)
		throw std::runtime_error("Invalid parameter! Please let buffer size be less than 127...!\n");

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

	d_packet_no_nodeA_buffer.resize(2*buffer_size);
	std::fill(d_packet_no_nodeA_buffer.begin(), d_packet_no_nodeA_buffer.end(), 0x00);

	d_data_nodeA_buffer.resize(2*buffer_size*packet_size);
	std::fill(d_data_nodeA_buffer.begin(), d_data_nodeA_buffer.end(), 0x00);

	d_packet_no_nodeB_buffer.resize(2*buffer_size);
	std::fill(d_packet_no_nodeB_buffer.begin(), d_packet_no_nodeB_buffer.end(), 0x00);

	d_data_nodeB_buffer.resize(2*buffer_size*packet_size);
	std::fill(d_data_nodeB_buffer.begin(), d_data_nodeB_buffer.end(), 0x00);

	d_xored_packet_no_buffer.resize(2*buffer_size);
	std::fill(d_xored_packet_no_buffer.begin(), d_xored_packet_no_buffer.end(), 0x00);

	d_xored_data_buffer.resize(2*buffer_size*packet_size);
	std::fill(d_xored_data_buffer.begin(), d_xored_data_buffer.end(), 0x00);	

	d_packet_out_nodeA_buffer.resize(2*buffer_size);
	std::fill(d_packet_out_nodeA_buffer.begin(), d_packet_out_nodeA_buffer.end(), 0x00);

	d_packet_out_nodeB_buffer.resize(2*buffer_size);
	std::fill(d_packet_out_nodeB_buffer.begin(), d_packet_out_nodeB_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx0_packet_number[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
		d_rx1_ended_packet_number[i] = 0x00;
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
	d_rx1_ended_packet_number_index = 0;
	d_rx1_data_index = 0;
	d_rx1_check_end_packet_index = 0;
	d_rx1_check_end_packet_count = 0;
	
	for(int i = d_ctrl*d_buffer_size; i<d_ctrl*d_buffer_size+d_buffer_size; i++)
	{
		d_packet_no_nodeA_buffer[i]  = 0x00;
		d_packet_no_nodeB_buffer[i]  = 0x00;
		d_xored_packet_no_buffer[i]  = 0x00;
		d_packet_out_nodeA_buffer[i] = 0x00;
		d_packet_out_nodeB_buffer[i] = 0x00;
	}

	d_tx_state = ST_HEADER_TRANS;
	d_tx_node_id = 0xFF;
	d_tx_packet_number  = 0xFF;
	d_trans_header_index = 0;
	d_tx_session_number_index = 0;

	if(d_ctrl==0)
	{
		d_ctrl = 1;
		if(d_max_xor_packet_number<d_buffer_size)
		{
			d_max_xor_packet_number = d_buffer_size;
		}
		else
		{
			d_max_xor_packet_number = 2*d_buffer_size;			
		}
	}
	else
	{
		d_ctrl = 0;
		if(d_max_xor_packet_number<(2*d_buffer_size))
		{
			d_max_xor_packet_number = 0;
		}
		else
		{
			d_max_xor_packet_number = d_buffer_size;
		}
	}

	if(d_next_session_number==0xFF)
	{
		d_next_session_number = 0x01;
	}
	else
	{
		d_next_session_number++;
	}

	d_packet_out = false;
	d_rcvd_xor_data = false;
	d_rcvd_nodeA_data = false;
	d_rcvd_nodeB_data = false;
	d_nodeA_no_rcvd_pkts = 0;
	d_nodeB_no_rcvd_pkts = 0;
	d_xor_no_rcvd_pkts = 0;
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

	int ni0 = 0;
	int ni1 = 0;
	int no = 0;

	while(ni0<nInputItems0)
	{
		switch(d_rx0_state)
		{
		case ST_RX0_IDLE:
		{
			if(in0[ni0]==0x0A)
			{
				d_rx0_node_id = 0x0A;
				d_rx0_state = ST_RX0_CHECK_NODE_ID;
				d_check_rx0_id_index++;
				d_check_rx0_id_count++;
			}
			if(in0[ni0]==0x1A)
			{
				d_rx0_node_id = 0x1A;
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
					if((int)rx0_pkt_no>0 && (int)rx0_pkt_no<=(2*d_buffer_size))
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
					if(idx>=(d_ctrl*d_buffer_size)              &&
					   idx<(d_ctrl*d_buffer_size+d_buffer_size))
					{
						int data_idx = 0;
						for(int k = 0; k<d_packet_size; k++)
						{
							data_idx = idx*d_packet_size+k;
							d_data_nodeA_buffer[data_idx] = d_rx0_packet[k];
						}
						if(d_packet_no_nodeA_buffer[idx]==0x00)
						{
							d_rcvd_nodeA_data = true;
							d_nodeA_no_rcvd_pkts++;
							std::cout<<d_nodeA_no_rcvd_pkts<<": received packet number "
								 <<(int)d_rx0_pkt_no<<" from nodeA\n";
							d_packet_no_nodeA_buffer[idx] = d_rx0_pkt_no;
							if(d_network_coding==1)
							{
								if(d_xored_packet_no_buffer[idx]!=0x00 && d_packet_no_nodeB_buffer[idx]==0x00)
								{
									d_packet_no_nodeB_buffer[idx] = d_rx0_pkt_no;
									int data_idx = 0;
									for(int k = 0; k<d_packet_size; k++)
									{
										data_idx = idx*d_packet_size+k;
										d_data_nodeB_buffer[data_idx] = 
										d_xored_data_buffer[data_idx]^d_rx0_packet[k];
									}
								}
							}
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
					d_rx1_state = ST_RX1_IDLE;
					if((int)rx1_pkt_no>0 && (int)rx1_pkt_no<=(2*d_buffer_size)) 
					{
						d_rx1_pkt_no = rx1_pkt_no;
						d_rx1_state = ST_RX1_GET_DATA;
					}
					if(rx1_pkt_no==0xFF)
					{
						d_rx1_state = ST_RX1_CHECK_ENDED_PACKET;
					}
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
				}
			}
			break;
		}
		case ST_RX1_CHECK_ENDED_PACKET:
		{
			d_rx1_ended_packet_number[d_rx1_ended_packet_number_index] = in1[ni1];
			d_rx1_ended_packet_number_index++;
			ni1++;
			if(d_rx1_ended_packet_number_index==3)
			{
				d_rx1_ended_packet_number_index = 0;
				unsigned char a, b, c;
				unsigned char rx1_ended_packet_number = 0x00;
				bool recv_ended_packet_number = false;
				a = d_rx1_ended_packet_number[0];
				b = d_rx1_ended_packet_number[1];
				c = d_rx1_ended_packet_number[2];
				if(a==b && b==c) { rx1_ended_packet_number = a; recv_ended_packet_number = true;}
				if(recv_ended_packet_number == true)
				{
					if((int)rx1_ended_packet_number>0 && (int)rx1_ended_packet_number<=(2*d_buffer_size))
					{
						if(d_rx1_node_id==d_nodeA_id && d_nodeA_ended_packet_number==0x00)
						{
							d_nodeA_ended_packet_number = rx1_ended_packet_number;
							std::cout<<"d_nodeA_ended_packet_number = "<<(int)d_nodeA_ended_packet_number<<"\n";
						}
						if(d_rx1_node_id==d_nodeB_id && d_nodeB_ended_packet_number==0x00)
						{
							d_nodeB_ended_packet_number = rx1_ended_packet_number;
							std::cout<<"d_nodeB_ended_packet_number = "<<(int)d_nodeB_ended_packet_number<<"\n";
						}
						if(d_rx1_node_id==d_nodeR_id && 
						  (d_max_xor_packet_number==d_buffer_size || d_max_xor_packet_number==2*d_buffer_size))
						{
							d_max_xor_packet_number = (int) rx1_ended_packet_number;
							std::cout<<"d_max_xor_packet_number = "<<d_max_xor_packet_number<<"\n";
						}
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
					if(idx>=(d_ctrl*d_buffer_size)              &&
					   idx<(d_ctrl*d_buffer_size+d_buffer_size) &&
					   d_rx1_node_id==d_nodeR_id)
					{
						int data_idx = 0;	
						for(int j = 0; j<d_packet_size; j++)
						{
							data_idx = idx*d_packet_size+j;
							d_xored_data_buffer[data_idx] = d_rx1_packet[j];
						}
						data_idx = 0;
						if(d_packet_no_nodeA_buffer[idx]!=0x00 && d_packet_no_nodeB_buffer[idx]==0x00)
						{
							d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
							for(int k = 0; k<d_packet_size; k++)
							{
								data_idx = idx*d_packet_size+k;
								d_data_nodeB_buffer[data_idx] = 
								d_data_nodeA_buffer[data_idx]^d_rx1_packet[k];
							}
						}
						if(d_xored_packet_no_buffer[idx]==0x00)
						{
							d_rcvd_xor_data = true;
							d_xor_no_rcvd_pkts++;
							std::cout<<d_xor_no_rcvd_pkts<<": Received XOR packet number "
								 <<(int)d_rx1_pkt_no<<" from node R\n";

							d_xored_packet_no_buffer[idx] = d_rx1_pkt_no;
						}
					}
					if(idx>=(d_ctrl*d_buffer_size)              &&
					   idx<(d_ctrl*d_buffer_size+d_buffer_size) &&
					   d_rx1_node_id==d_nodeA_id)
					{
						int data_idx = 0;
						for(int k = 0; k<d_packet_size; k++)
						{
							data_idx = idx*d_packet_size+k;
							d_data_nodeA_buffer[data_idx] = d_rx1_packet[k];
						}
						if(d_network_coding==1)
						{
							if(d_xored_packet_no_buffer[idx]!=0x00 && d_packet_no_nodeB_buffer[idx]==0x00)
							{
								d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
								int data_idx = 0;
								for(int k = 0; k<d_packet_size; k++)
								{
									data_idx = idx*d_packet_size+k;
									d_data_nodeB_buffer[data_idx] = 
									d_xored_data_buffer[data_idx]^d_rx1_packet[k];
								}
							}
						}
						if(d_packet_no_nodeA_buffer[idx]==0x00)
						{
							d_rcvd_nodeA_data = true;
							d_nodeA_no_rcvd_pkts++;
							std::cout<<d_nodeA_no_rcvd_pkts<<": Received packet number "
								 <<(int)d_rx1_pkt_no<<" of nodeA sent from node R\n";
							d_packet_no_nodeA_buffer[idx] = d_rx1_pkt_no;
						}
					}
					if(idx>=(d_ctrl*d_buffer_size)              &&
					   idx<(d_ctrl*d_buffer_size+d_buffer_size) &&
					   d_rx1_node_id==d_nodeB_id)
					{
						if(d_packet_no_nodeB_buffer[idx]==0x00)
						{
							d_rcvd_nodeB_data = true;
							d_nodeB_no_rcvd_pkts++;
							std::cout<<d_nodeB_no_rcvd_pkts<<": Received packet number "
								 <<(int)d_rx1_pkt_no<<" of nodeB sent from node R\n";
							d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
						}
						int data_idx = 0;
						for(int k = 0; k<d_packet_size; k++)
						{
							data_idx = idx*d_packet_size+k;
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
		if(d_packet_out==false)
		{
			int begin_idx = 0;
			int end_idx = 0;
			if(d_ctrl==0)
			{
				begin_idx = 0;
				end_idx = d_buffer_size;
			}
			else
			{
				begin_idx = d_buffer_size;
				end_idx = 2*d_buffer_size;
			}

			for(int i = begin_idx; i<end_idx; i++)
			{
				if((d_packet_out_nodeA_buffer[i]==0x00) && (d_packet_no_nodeA_buffer[i]!=0x00))
				{
					d_packet_out_index = i;
					d_shift = 0;
					d_packet_out = true;
					break;
				}
				if((d_packet_out_nodeB_buffer[i]==0x00) && (d_packet_no_nodeB_buffer[i]!=0x00))
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
			out1[no] = 0x00;
		}
		else
		{
			if(d_data_out_index==0) 
			{
				if(d_shift==0) { out1[no] = d_nodeA_id; }
				if(d_shift==1) { out1[no] = d_nodeB_id; }
				d_data_out_index++;
			}
			else
			{
				if (d_data_out_index==1)
				{
					if(d_shift==0)
					{
						out1[no] = d_packet_no_nodeA_buffer[d_packet_out_index];
					}
					if(d_shift==1)
					{
						out1[no] = d_packet_no_nodeB_buffer[d_packet_out_index];
					}
					d_data_out_index++;
				}
				else
				{
					int idx = d_packet_out_index*d_packet_size+d_data_out_index-2;
					if(d_shift==0)
					{
						out1[no] = d_data_nodeA_buffer[idx];
					}
					if(d_shift==1)
					{
						out1[no] = d_data_nodeB_buffer[idx];
					}
					d_data_out_index++;
					if(d_data_out_index==(d_packet_size+2))
					{
						if(d_shift==0)
						{
							d_packet_out_nodeA_buffer[d_packet_out_index] = 0x01;
						}
						if(d_shift==1)
						{
							d_packet_out_nodeB_buffer[d_packet_out_index] = 0x01;
						}
						d_data_out_index = 0;
						d_packet_out = false;
					}
				}
			}
			if(d_packet_out==false)
			{
				if(d_network_coding==1) // With Network Coding
				{
					int begin_idx = 0;
					if(d_ctrl==0)
					{
						begin_idx = 0;
					}
					else
					{
						begin_idx = d_buffer_size;
					}

					int check_count = 0;
					if(d_ctrl==1) check_count = d_buffer_size;
					for(int i = begin_idx; i<d_max_xor_packet_number; i++)
					{
						if(d_packet_out_nodeA_buffer[i]!=0x00 &&
						   d_packet_out_nodeB_buffer[i]!=0x00)
						{
							check_count++;
						}
					}

					if(check_count==d_max_xor_packet_number)
					{
						int diff = 0;
						if(d_ctrl==0)
						{
							diff = d_buffer_size-d_max_xor_packet_number;
						}
						else
						{
							diff = 2*d_buffer_size-d_max_xor_packet_number;
						}
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
							d_request_to_start_new_session = true;
						}
					}
				}
				else // Without Network Coding
				{
					bool full_A = false;
					bool full_B = false;
					int begin_idx = 0;
					int end_idx = 0;
					if(d_ctrl==0)
					{
						begin_idx = 0;
						end_idx = d_buffer_size;
					}
					else
					{
						begin_idx = d_buffer_size;
						end_idx = 2*d_buffer_size;
					}
					for(int run = 0 ; run<2; run++)
					{
						unsigned char d_ended_packet_number = 0x00;

						if(run==0) { d_ended_packet_number = d_nodeA_ended_packet_number; }
						if(run==1) { d_ended_packet_number = d_nodeB_ended_packet_number; }

						if(d_ended_packet_number==0x00)
						{
							int check_count = 0;
							for(int i = begin_idx; i<end_idx; i++)
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
							if(d_ctrl==1)
							{
								check_count = d_buffer_size;
							}
							int upper_bound = (int)d_ended_packet_number;
							for(int i = begin_idx; i<upper_bound; i++)
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
						std::cout<<"Need to start a new session without NC :-)\n";
						d_request_to_start_new_session = true;
					}
				}
			}
		}

		switch(d_tx_state)
		{
			case ST_HEADER_TRANS:
			{
				if(d_trans_header_index<3)
				{
					out0[no] = d_tx_node_id;
					d_trans_header_index++;
					no++;
					break;
				}
				if(d_trans_header_index>=3)
				{
					out0[no] = d_tx_packet_number;
					d_trans_header_index++;
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
				d_tx_session_number_index++;
				no++;
				if(d_tx_session_number_index==6)
				{
					d_tx_state = ST_HEADER_TRANS;
					d_tx_session_number_index = 0;
					if(d_network_coding==1)
					{
						if(d_rcvd_xor_data==true)
						{
							d_tx_state = ST_FREE;
						}
						if(d_ctrl==0 && d_max_xor_packet_number<d_buffer_size &&
						   (d_rcvd_nodeA_data==true||d_rcvd_nodeB_data==true))
						{
							d_tx_state = ST_FREE;
						}
						if(d_ctrl==1 && d_max_xor_packet_number<(2*d_buffer_size) &&
						   (d_rcvd_nodeA_data==true||d_rcvd_nodeB_data==true))
						{
							d_tx_state = ST_FREE;
						}
					}
					else
					{
						if (d_rcvd_nodeA_data==true&&d_rcvd_nodeB_data==true)
						d_tx_state = ST_FREE;
					}
				}
				break;
			}
			case ST_FREE:
			{
				out0[no] = 0x00;
				no++;
				if(d_request_to_start_new_session==true)
				{
					std::cout<<"Reset to start a new session"<<(int)d_next_session_number<<"\n";
					reset();
					d_request_to_start_new_session = false;
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

