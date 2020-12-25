
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
#include <stdlib.h>

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace DNC {

    nodeR_controller::sptr
    nodeR_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding, float priority)
    {
      return gnuradio::get_initial_sptr
        (new nodeR_controller_impl(buffer_size, packet_size, guard_interval, nodeA_id, nodeB_id, nodeC_id, nodeR_id, network_coding, priority));
    }

    /*
     * The private constructor
     */
    nodeR_controller_impl::nodeR_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding, float priority)
      : gr::block("nodeR_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), 
	      d_nodeA_id(nodeA_id), d_nodeB_id(nodeB_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id),
	      d_network_coding(network_coding), d_priority(priority),

	      d_ctrl_rx0(0), d_rx0_node_id(0x00), d_check_rx0_id_count(0), d_check_rx0_id_index(0), d_rx0_packet_number_index(0),
	      d_rx0_check_ended_packet_index(0), d_rx0_data_index(0), d_rx0_check_guard_count(0), d_rx0_check_guard_index(0),
	      d_rx0_pkt_no(0x00), d_nodeA_ended_packet_number(0x00), d_erase_rx0(false), d_first_rcv_rx0(false),

	      d_ctrl_rx1(0), d_rx1_node_id(0x00), d_check_rx1_id_count(0), d_check_rx1_id_index(0), d_rx1_packet_number_index(0),
	      d_rx1_check_ended_packet_index(0), d_rx1_data_index(0), d_rx1_check_guard_count(0), d_rx1_check_guard_index(0),
	      d_rx1_pkt_no(0x00), d_nodeB_ended_packet_number(0x00), d_erase_rx1(false), d_first_rcv_rx1(false),

	      d_ctrl_tx(1), d_buffer_index(0), d_send_max_xor_packet_number(false), d_sent_max_xor_packet_number(false),
	      d_send_max_xor_pkt_no_index(0), d_tx_node_id(0x00), d_tx_packet_number(0x00), d_tx_guard_index(0),
	      d_trans_header_index(0), d_data_trans_index(0), d_send_max_xor_pkt_no_count(0), d_ctrl_shift(false),

	      d_shift(0), d_nodeA_send_ended_packet(false), d_nodeB_send_ended_packet(false), d_tx_ended_packet_number_index(0),
	      d_tx_ended_packet_number(0x00),

	      d_count_pkt_rx0(0), d_count_pkt_rx1(0), d_tx_ended_packet_nodeA(0), d_tx_ended_packet_nodeB(0),
	      d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_MANAGE_DATA_TRANS)
    {
	if (buffer_size>127)
		throw std::runtime_error("Invalid parameter! Please let buffer size be less than 127...!\n");

	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please let guard interval be greater than or equal to 6...!\n");

	if (nodeA_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeA ID must be different from 0...!!!\n");

	if (nodeB_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeB ID must be different from 0...!!!\n");

	if (nodeR_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeR ID must be different from 0...!!!\n");

	if (nodeC_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeC ID must be different from 0...!!!\n");

	if (nodeA_id==nodeB_id||nodeA_id==nodeC_id||nodeA_id==nodeR_id||nodeB_id==nodeC_id||nodeB_id==nodeR_id||nodeC_id==nodeR_id)
		throw std::runtime_error("Invalid parameter! Node IDs must be different from each other...!!!\n");

	d_max_xor_packet_number = (unsigned char) (2*buffer_size);

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

	for(int i = 0; i<3; i++)
	{
		d_rx0_packet_number[i] = 0x00;
		d_rx0_ended_packet[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
		d_rx1_ended_packet[i] = 0x00;
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
					if(recv_pkt==true)
					{
						d_rx0_state = ST_RX0_IDLE;
						if((int)rx0_pkt_no>0 && (int)rx0_pkt_no<=(2*d_buffer_size))
						{
							d_rx0_pkt_no = rx0_pkt_no;
							d_rx0_state = ST_RX0_GET_DATA;
						}
						if(rx0_pkt_no==0xFF)
						{
							d_rx0_state = ST_RX0_CHECK_ENDED_PACKET;
						}
					}
					else
					{
						d_rx0_state = ST_RX0_IDLE;
					}
				}
				break;
			}
			case ST_RX0_CHECK_ENDED_PACKET:
			{
				d_rx0_ended_packet[d_rx0_check_ended_packet_index] = in0[ni0];
				d_rx0_check_ended_packet_index++;
				ni0++;
				if(d_rx0_check_ended_packet_index==3)
				{
					d_rx0_check_ended_packet_index = 0;
					unsigned char a, b, c;
					unsigned char rx0_ended_packet = 0x00;
					bool recv_ended_packet = false;
					a = d_rx0_ended_packet[0];
					b = d_rx0_ended_packet[1];
					c = d_rx0_ended_packet[2];
					if(a==b && b==c) { rx0_ended_packet = a; recv_ended_packet = true;}
					if( recv_ended_packet == true         &&
					    d_nodeA_ended_packet_number==0x00 && 
					    (int)rx0_ended_packet>0           && 
					    (int)rx0_ended_packet<=(2*d_buffer_size) )
					{
						std::cout<<"d_nodeA_ended_packet_number = "<<(int)rx0_ended_packet<<"\n";
						d_nodeA_ended_packet_number = rx0_ended_packet;
						d_max_xor_packet_number = rx0_ended_packet;
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
					d_rx0_state = ST_RX0_CHECK_GUARD_INTERVAL;
				}
				break;
			}
			case ST_RX0_CHECK_GUARD_INTERVAL:
			{
				if(in0[ni0]==0x00)
				{
					d_rx0_check_guard_count++;
				}
				d_rx0_check_guard_index++;
				ni0++;
				if(d_rx0_check_guard_index==6)
				{
					if(d_rx0_check_guard_index>4)
					{
						int check_full = 0;
						if(d_ctrl_rx0==0)
						{
							int idx = (int)d_rx0_pkt_no-1;
							if(d_rx0_node_id==0x0A &&
							   (idx>=0)            &&
							   (idx<d_buffer_size) &&
							   d_packet_no_nodeA_buffer[idx]==0x00)
							{
								d_count_pkt_rx0++;
								std::cout<<d_count_pkt_rx0<<": Received packet number "
									 <<(int)d_rx0_pkt_no<<" from node A\n";
								d_packet_no_nodeA_buffer[idx] = d_rx0_pkt_no;
								int data_idx = 0;
								for(int i = 0; i<d_packet_size; i++)
								{
									data_idx = idx*d_packet_size+i;
									d_data_nodeA_buffer[data_idx] = d_rx0_packet[i];
								}

								if(d_count_pkt_rx0==1)
								{
									d_first_rcv_rx0 = true;
								}

								if((d_ctrl_rx1==0 && d_first_rcv_rx0==true && d_first_rcv_rx1==true)||
								   (d_first_rcv_rx0==true && d_max_xor_packet_number==0x00))
								{
									std::cout<<"shift control\n";
									d_ctrl_shift = true;
									d_first_rcv_rx0 = false;
									d_first_rcv_rx1 = false;
									for(int i = d_buffer_size; i<2*d_buffer_size; i++)
									{
										d_packet_no_nodeA_buffer[i] = 0x00;
										d_packet_no_nodeB_buffer[i] = 0x00;
										d_xored_packet_no_buffer[i] = 0x00;
									}
								}
							}
							
							int j = 0;
							for(j = 0; j<d_buffer_size; j++)
							{
								if(d_packet_no_nodeA_buffer[j]!=0x00)
								{
									check_full++;
								}
							}

							if(check_full==d_buffer_size )
							{
								std::cout<<d_ctrl_rx0<<": Get full buffer nodeA\n";
								if((int)d_max_xor_packet_number>0 &&
								   (int)d_max_xor_packet_number<(2*d_buffer_size))
								{
									d_max_xor_packet_number = 0x00;
									d_sent_max_xor_packet_number = true;
								}
								
								d_ctrl_rx0 = 1;
								d_count_pkt_rx0 = 0;
							}
						}
						else
						{
							int idx = (int)d_rx0_pkt_no-1;
							if( d_rx0_node_id==0x1A	    && 
							    (idx>=d_buffer_size)    && 
							    (idx<(2*d_buffer_size)) && 
							    d_packet_no_nodeA_buffer[idx]==0x00)
							{
								d_count_pkt_rx0++;
								std::cout<<d_count_pkt_rx0<<": Received packet number "
									 <<(int)d_rx0_pkt_no<<" from node A\n";
								d_packet_no_nodeA_buffer[idx] = d_rx0_pkt_no;
								int data_idx = 0;
								for(int i = 0; i<d_packet_size; i++)
								{
									data_idx = idx*d_packet_size+i;
									d_data_nodeA_buffer[data_idx] = d_rx0_packet[i];
								}

								if(d_count_pkt_rx0==1)
								{
									d_first_rcv_rx0 = true;
								}

								if((d_ctrl_rx1==1 && d_first_rcv_rx0==true && d_first_rcv_rx1==true)||
								   (d_first_rcv_rx0==true && d_max_xor_packet_number==0x00))
								{
									std::cout<<"shift control\n";
									d_ctrl_shift = true;
									d_first_rcv_rx0 = false;
									d_first_rcv_rx1 = false;
									for(int i = 0; i<d_buffer_size; i++)
									{
										d_packet_no_nodeA_buffer[i] = 0x00;
										d_packet_no_nodeB_buffer[i] = 0x00;
										d_xored_packet_no_buffer[i] = 0x00;
									}
								}
							}

							int j = 0;
							for(j = d_buffer_size; j<(2*d_buffer_size); j++)
							{
								if(d_packet_no_nodeA_buffer[j]!=0x00)
								{
									check_full++;
								}
							}

							if(check_full==d_buffer_size )
							{
								std::cout<<d_ctrl_rx0<<": Get full buffer nodeA\n";
								if((int)d_max_xor_packet_number>0 &&
								   (int)d_max_xor_packet_number<(2*d_buffer_size))
								{
									d_max_xor_packet_number = 0x00;
									d_sent_max_xor_packet_number = true;
								}
								
								d_ctrl_rx0 = 0;
								d_count_pkt_rx0 = 0;
							}
						}
					}
					d_rx0_state = ST_RX0_IDLE;
					d_rx0_check_guard_count = 0;
					d_rx0_check_guard_index = 0;				
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
				if(in1[ni1]==0x0B)
				{
					d_rx1_node_id = 0x0B;
					d_rx1_state = ST_RX1_CHECK_NODE_ID;
					d_check_rx1_id_index++;
					d_check_rx1_id_count++;
				}
				if(in1[ni1]==0xBB)
				{
					d_rx1_node_id = 0xBB;
					d_rx1_state = ST_RX1_CHECK_NODE_ID;
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
				d_rx1_ended_packet[d_rx1_check_ended_packet_index] = in1[ni1];
				d_rx1_check_ended_packet_index++;
				ni1++;
				if(d_rx1_check_ended_packet_index==3)
				{
					d_rx1_check_ended_packet_index = 0;
					unsigned char a, b, c;
					unsigned char rx1_ended_packet = 0x00;
					bool recv_ended_packet = false;
					a = d_rx1_ended_packet[0];
					b = d_rx1_ended_packet[1];
					c = d_rx1_ended_packet[2];
					if(a==b && b==c) { rx1_ended_packet = a; recv_ended_packet = true;}
					if( recv_ended_packet == true         &&
					    d_nodeB_ended_packet_number==0x00 &&
					    (int)rx1_ended_packet>0           &&
					    (int)rx1_ended_packet<=(2*d_buffer_size) )
					{
						std::cout<<"d_nodeB_ended_packet_number = "<<(int)rx1_ended_packet<<"\n";
						d_nodeB_ended_packet_number = rx1_ended_packet;	
						d_max_xor_packet_number = rx1_ended_packet;
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
					d_rx1_state = ST_RX1_CHECK_GUARD_INTERVAL;
				}
				break;
			}
			case ST_RX1_CHECK_GUARD_INTERVAL:
			{
				if(in1[ni1]==0x00)
				{
					d_rx1_check_guard_count++;
				}
				d_rx1_check_guard_index++;
				ni1++;
				if(d_rx1_check_guard_index==6)
				{
					if(d_rx1_check_guard_index>4)
					{
						int check_full = 0;
						if(d_ctrl_rx1==0)
						{
							int idx = (int)d_rx1_pkt_no-1;
							if( d_rx1_node_id==0x0B &&
							    (idx>=0)            && 
							    (idx<d_buffer_size) && 
							    d_packet_no_nodeB_buffer[idx]==0x00)
							{
								d_count_pkt_rx1++;
								std::cout<<d_count_pkt_rx1<<": Received packet number "
									 <<(int)d_rx1_pkt_no<<" from node B\n";
								d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
								int data_idx = 0;
								for(int i = 0; i<d_packet_size; i++)
								{
									data_idx = idx*d_packet_size+i;
									d_data_nodeB_buffer[data_idx] = d_rx1_packet[i];
								}

								if(d_count_pkt_rx1==1)
								{
									d_first_rcv_rx1 = true;
								}

								if((d_ctrl_rx0==0 && d_first_rcv_rx0==true && d_first_rcv_rx1==true)||
								    (d_first_rcv_rx1==true && d_max_xor_packet_number==0x00))
								{
									std::cout<<"shift control\n";
									d_ctrl_shift = true;
									d_first_rcv_rx0 = false;
									d_first_rcv_rx1 = false;
									for(int i = d_buffer_size; i<2*d_buffer_size; i++)
									{
										d_packet_no_nodeA_buffer[i] = 0x00;
										d_packet_no_nodeB_buffer[i] = 0x00;
										d_xored_packet_no_buffer[i] = 0x00;
									}
								}
							}

							int j = 0;
							for(j = 0; j<d_buffer_size; j++)
							{
								if(d_packet_no_nodeB_buffer[j]!=0x00)
								{
									check_full++;
								}
							}

							if(check_full==d_buffer_size )
							{
								std::cout<<d_ctrl_rx1<<": Get full buffer nodeB\n";
								if((int)d_max_xor_packet_number>0 &&
								   (int)d_max_xor_packet_number<(2*d_buffer_size))
								{
									d_max_xor_packet_number = 0x00;
									d_sent_max_xor_packet_number = true;
								}
								
								d_ctrl_rx1 = 1;
								d_count_pkt_rx1 = 0;
							}
						}
						else
						{
							int idx = (int)d_rx1_pkt_no-1;
							if( d_rx1_node_id==0xBB    &&
							    (idx>=d_buffer_size)   && 
							    (idx<(2*d_buffer_size)) && 
							    d_packet_no_nodeB_buffer[idx]==0x00)
							{
								d_count_pkt_rx1++;
								std::cout<<d_count_pkt_rx1<<": Received packet number "
									 <<(int)d_rx1_pkt_no<<" from node B\n";
								d_packet_no_nodeB_buffer[idx] = d_rx1_pkt_no;
								int data_idx = 0;
								for(int i = 0; i<d_packet_size; i++)
								{
									data_idx = idx*d_packet_size+i;
									d_data_nodeB_buffer[data_idx] = d_rx1_packet[i];
								}

								if(d_count_pkt_rx1==1)
								{
									d_first_rcv_rx1 = true;
								}

								if((d_ctrl_rx0==1 && d_first_rcv_rx0==true && d_first_rcv_rx1==true)||
								   (d_first_rcv_rx1==true && d_max_xor_packet_number==0x00))
								{
									std::cout<<"shift control\n";
									d_ctrl_shift = true;
									d_first_rcv_rx0 = false;
									d_first_rcv_rx1 = false;
									for(int i = 0; i<d_buffer_size; i++)
									{
										d_packet_no_nodeA_buffer[i] = 0x00;
										d_packet_no_nodeB_buffer[i] = 0x00;
										d_xored_packet_no_buffer[i] = 0x00;
									}
								}
							}

							int j = 0;
							for(j = d_buffer_size; j<(2*d_buffer_size); j++)
							{
								if(d_packet_no_nodeB_buffer[j]!=0x00)
								{
									check_full++;
								}
							}

							if(check_full==d_buffer_size )
							{
								std::cout<<d_ctrl_rx1<<": Get full buffer nodeB\n";
								if((int)d_max_xor_packet_number>0 &&
								   (int)d_max_xor_packet_number<(2*d_buffer_size))
								{
									d_max_xor_packet_number = 0x00;
									d_sent_max_xor_packet_number = true;
								}
								
								d_ctrl_rx1 = 0;
								d_count_pkt_rx1 = 0;
							}
						}
					}
					d_rx1_state = ST_RX1_IDLE;
					d_rx1_check_guard_count = 0;
					d_rx1_check_guard_index = 0;				
				}
				break;
			}
		}
	}

	while(no<nOutputItems)
	{
		switch(d_tx_state)
		{
		case ST_MANAGE_DATA_TRANS:
		{
			out[no] = 0x00;
			no++;
			if(d_ctrl_shift==true)
			{
				if(d_ctrl_tx==0) 
				{
					d_ctrl_tx = 1;
					std::cout<<"d_ctrl_tx = "<<d_ctrl_tx<<"\n";
					d_buffer_index = d_buffer_size;
				}
				else
				{
					d_ctrl_tx = 0;
					std::cout<<"d_ctrl_tx = "<<d_ctrl_tx<<"\n";
					d_buffer_index = 0;
				}
				d_ctrl_shift = false;
			}
			if(d_network_coding==1) // Using network coding method
			{
				if(d_buffer_index==0 || d_buffer_index==d_buffer_size) { d_send_max_xor_packet_number = false; }
				if(d_xored_packet_no_buffer[d_buffer_index]!=0x00)
				{
					d_tx_node_id = d_nodeR_id;
					d_tx_packet_number = d_xored_packet_no_buffer[d_buffer_index];
					d_tx_state = ST_HEADER_TRANS;
				}
				else
				{
					if(d_packet_no_nodeA_buffer[d_buffer_index]!=0x00 &&
					   d_packet_no_nodeB_buffer[d_buffer_index]!=0x00)
					{
						/* Dealing with priority */
						float val = (float)rand() / RAND_MAX;
						int coeff = 0;
						if (val < d_priority)
						{
						    coeff = 1;
						}
						for(int i = 0; i<d_packet_size; i++)
						{
							d_data_nodeB_buffer[d_buffer_index*d_packet_size+i] = (unsigned char)(coeff*(int)d_data_nodeB_buffer[d_buffer_index*d_packet_size+i]);
						}

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
						if(d_buffer_index<(int)d_max_xor_packet_number)
						{
							d_buffer_index++;
							if(d_ctrl_tx==0)
							{
								if(d_buffer_index==d_buffer_size)
								{
									d_buffer_index = 0;
								}
							}
							else
							{
								if(d_buffer_index==(2*d_buffer_size))
								{
									d_buffer_index = d_buffer_size;
								}
							}
						}
						else
						{
							if(d_send_max_xor_packet_number==false && d_sent_max_xor_packet_number==false)
							{
								d_send_max_xor_packet_number = true;
								d_tx_node_id = d_nodeR_id;
								d_tx_packet_number = 0xFF;
								d_tx_state = ST_HEADER_TRANS;
								if(d_ctrl_tx==1) 
								{
									d_buffer_index = d_buffer_size;
								}
								else
								{
									d_buffer_index = 0;
								}

								break;
							}
							if(d_packet_no_nodeA_buffer[d_buffer_index]!=0x00)
							{
								d_tx_node_id = d_nodeA_id;
								d_tx_packet_number = d_packet_no_nodeA_buffer[d_buffer_index];
								d_tx_state = ST_HEADER_TRANS;
							}
							if(d_packet_no_nodeB_buffer[d_buffer_index]!=0x00)
							{
								d_tx_node_id = d_nodeB_id;
								d_tx_packet_number = d_packet_no_nodeB_buffer[d_buffer_index];
								d_tx_state = ST_HEADER_TRANS;
							}
						}
					}
				}
			}
			if(d_network_coding==0) // Without using network coding method
			{
				if(d_shift==0)
				{
					if(d_packet_no_nodeA_buffer[d_buffer_index]!=0x00)
					{
						d_tx_node_id = d_nodeA_id;
						d_tx_packet_number = d_packet_no_nodeA_buffer[d_buffer_index];
						d_tx_state = ST_HEADER_TRANS;
					}
					else
					{
						if(d_nodeA_send_ended_packet==false && d_nodeA_ended_packet_number!=0x00)
						{
							d_tx_node_id = d_nodeA_id;
							d_tx_ended_packet_number = d_nodeA_ended_packet_number;
							d_tx_state = ST_END_PACKET_HEADER_TRANS;
						}
						d_shift = 1;
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
						if(d_nodeB_send_ended_packet==false && d_nodeB_ended_packet_number!=0x00)
						{
							d_tx_node_id = d_nodeB_id;
							d_tx_ended_packet_number = d_nodeB_ended_packet_number;
							d_tx_state = ST_END_PACKET_HEADER_TRANS;
						}
						d_shift = 0;
						d_buffer_index++;
						if(d_ctrl_tx==0)
						{
							if(d_buffer_index==d_buffer_size)
							{
								d_buffer_index = 0;
							}
						}
						else
						{
							if(d_buffer_index==(2*d_buffer_size))
							{
								d_buffer_index = d_buffer_size;
							}
						}
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
				d_trans_header_index++;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					if(d_tx_node_id==d_nodeR_id && d_tx_packet_number==0xFF)
					{
						d_tx_state = ST_MAX_XOR_PACKET_NUMBER_TRANS;
						break;
					}
					else
					{
						d_tx_state = ST_DATA_TRANS;
						break;
					}
				}
			}
			break;
		}
		case ST_MAX_XOR_PACKET_NUMBER_TRANS:
		{
			out[no] = d_max_xor_packet_number;
			no++;
			d_send_max_xor_pkt_no_index++;
			if(d_send_max_xor_pkt_no_index==3)
			{
				d_send_max_xor_pkt_no_index = 0;
				d_send_max_xor_pkt_no_count++;
				if(d_send_max_xor_pkt_no_count==50)
				{
					d_send_max_xor_pkt_no_count = 0;
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
			if(d_network_coding==1) // Using network coding method
			{
				if(d_tx_node_id==d_nodeA_id)
				{
					out[no] = d_data_nodeA_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
				}
				if(d_tx_node_id==d_nodeB_id)
				{
					out[no] = d_data_nodeB_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
				}
				if(d_tx_node_id==d_nodeR_id)
				{
					out[no] = d_xored_data_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
				}
				d_data_trans_index++;
				no++;
				if(d_data_trans_index==d_packet_size)
				{
					d_data_trans_index = 0;
					d_tx_state = ST_GUARD_INTERVAL_TRANS;
				}
			}
			else // Without using network coding method
			{
				if(d_shift==0)
				{
					// forward nodeA's packets
					out[no] = d_data_nodeA_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
					d_data_trans_index++;
					no++;
					if(d_data_trans_index==d_packet_size)
					{
						d_data_trans_index = 0;
						d_shift = 1;
						d_tx_state = ST_GUARD_INTERVAL_TRANS;
					}
				}
				else
				{
					// forward nodeB's packets
					out[no] = d_data_nodeB_buffer[d_buffer_index*d_packet_size+d_data_trans_index];
					d_data_trans_index++;
					no++;
					if(d_data_trans_index==d_packet_size)
					{
						d_data_trans_index = 0;
						d_shift = 0;
						d_tx_state = ST_GUARD_INTERVAL_TRANS;
					}
				}
			}
			break;
		}
		case ST_GUARD_INTERVAL_TRANS:
		{
			out[no] = 0x00;
			d_tx_guard_index++;
			no++;
			if(d_tx_guard_index==d_guard_interval)
			{
				d_tx_guard_index = 0;
				if(d_network_coding==1)
				{
					d_buffer_index++;
					if(d_ctrl_tx==0)
					{
						if(d_buffer_index==d_buffer_size)
						{
							d_buffer_index = 0;
						}
					}
					else
					{
						if(d_buffer_index==(2*d_buffer_size))
						{
							d_buffer_index = d_buffer_size;
						}
					}
					d_tx_state = ST_MANAGE_DATA_TRANS;
				}
				else
				{
					if(d_shift==0)
					{
						d_buffer_index++;
						if(d_ctrl_tx==0)
						{
							if(d_buffer_index==d_buffer_size)
							{
								d_buffer_index = 0;
							}
						}
						else
						{
							if(d_buffer_index==(2*d_buffer_size))
							{
								d_buffer_index = d_buffer_size;
							}
						}
					}
					d_tx_state = ST_MANAGE_DATA_TRANS;
				}
			}
			break;
		}
		case ST_END_PACKET_HEADER_TRANS:
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
				out[no] = 0xFF;
				d_trans_header_index++;
				no++;
				if(d_trans_header_index==6)
				{
					d_trans_header_index = 0;
					d_tx_state = ST_END_PACKET_NUMBER_TRANS;
				}
			}
			break;
		}
		case ST_END_PACKET_NUMBER_TRANS:
		{
			if(d_tx_ended_packet_number_index<3)
			{
				out[no] = d_tx_ended_packet_number;
				d_tx_ended_packet_number_index++;
				no++;
				break;
			}
			else
			{
				out[no] = 0x00;
				no++;
				d_tx_ended_packet_number_index++;
				if(d_tx_ended_packet_number_index==6)
				{
					d_tx_state = ST_END_PACKET_HEADER_TRANS;
					d_tx_ended_packet_number_index = 0;
					if(d_shift==1)
					{
						d_tx_ended_packet_nodeA++;
						if(d_tx_ended_packet_nodeA==50)
						{
							d_shift = 1;
							d_tx_ended_packet_nodeA = 0;
							d_tx_state = ST_MANAGE_DATA_TRANS;
						}
					}
					else
					{
						d_tx_ended_packet_nodeB++;
						if(d_tx_ended_packet_nodeB==50)
						{
							d_shift = 0;
							d_tx_ended_packet_nodeB = 0;
							d_tx_state = ST_MANAGE_DATA_TRANS;		
						}
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

