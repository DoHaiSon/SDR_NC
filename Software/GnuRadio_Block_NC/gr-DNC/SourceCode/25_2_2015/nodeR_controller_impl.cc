
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

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace DNC {

    nodeR_controller::sptr
    nodeR_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding)
    {
      return gnuradio::get_initial_sptr
        (new nodeR_controller_impl(buffer_size, packet_size, guard_interval, nodeA_id, nodeB_id, nodeC_id, nodeR_id, network_coding));
    }

    /*
     * The private constructor
     */
    nodeR_controller_impl::nodeR_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id, int network_coding)
      : gr::block("nodeR_controller",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), 
	      d_nodeA_id(nodeA_id), d_nodeB_id(nodeB_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id),
	      d_network_coding(network_coding), d_send(false), d_rx_node_id(0x00), d_check_rx_id_count(0), d_check_rx_id_index(0), 
	      d_rx_packet_number_index(0), d_rx_control_info_index(0), d_rx_data_index(0), d_rx_check_guard_count(0),
	      d_rx_check_guard_index(0), d_rx_pkt_no(0x00), d_nodeA_ended_packet_number(0x00), d_trans_data_to_nodeC(false),
	      d_nodeB_ended_packet_number(0x00), d_tx_node_id(0x00), d_tx_packet_number(0x00), d_tx_control_info(0x00),
	      d_tx_control_info_index(0), d_tx_guard_index(0), d_trans_header_index(0), d_data_trans_index(0),
	      d_buffer_index(0), d_shift(0), d_nodeA_send_ended_packet(false), d_nodeB_send_ended_packet(false),
	      d_tx_ended_packet_number_index(0), d_tx_ended_packet_number(0x00), d_timeout_ms(1000),
	      d_rx_state(ST_RX_IDLE), d_tx_state(ST_INIT)
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

	if (nodeC_id==0x00)
		throw std::runtime_error("Invalid parameter! NodeC ID must be different from 0...!!!\n");

	if (nodeA_id==nodeB_id||nodeA_id==nodeC_id||nodeA_id==nodeR_id||nodeB_id==nodeC_id||nodeB_id==nodeR_id||nodeC_id==nodeR_id)
		throw std::runtime_error("Invalid parameter! Node IDs must be different from each other...!!!\n");

	d_rx_packet.resize(packet_size);
	std::fill(d_rx_packet.begin(), d_rx_packet.end(), 0x00);

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

	d_request_retrans_packet_no_buffer.resize(buffer_size);
	std::fill(d_request_retrans_packet_no_buffer.begin(), d_request_retrans_packet_no_buffer.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_rx_packet_number[i] = 0x00;
		d_rx_control_info[i] = 0x00;
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
        int nInputItems = ninput_items[0];
	int nOutputItems = noutput_items;
	
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

	int ni = 0;
	int no = 0;

	while(ni<nInputItems)
	{
		switch(d_rx_state)
		{
			case ST_RX_IDLE:
			{
				if(in[ni]==d_nodeA_id)
				{
					d_rx_node_id = d_nodeA_id;
					d_rx_state = ST_RX_CHECK_NODE_ID;
					d_check_rx_id_index++;
					d_check_rx_id_count++;
				}
				if(in[ni]==d_nodeB_id)
				{
					d_rx_node_id = d_nodeB_id;
					d_rx_state = ST_RX_CHECK_NODE_ID;
					d_check_rx_id_index++;
					d_check_rx_id_count++;
				}
				if(in[ni]==d_nodeC_id)
				{
					d_rx_node_id = d_nodeC_id;
					d_rx_state = ST_RX_CHECK_NODE_ID;
					d_check_rx_id_index++;
					d_check_rx_id_count++;
				}
				ni++;
				break;
			}
			case ST_RX_CHECK_NODE_ID:
			{
				if(in[ni]==d_rx_node_id)
				{
					d_check_rx_id_count++;
				}
				ni++;
				d_check_rx_id_index++;
				if(d_check_rx_id_index==3)
				{
					if(d_check_rx_id_count==3)
					{
						d_rx_state = ST_RX_CHECK_PACKET_NUMBER;
					}
					else
					{
						d_rx_state = ST_RX_IDLE;
						ni = ni - 1;
					}
					d_check_rx_id_index = 0;
					d_check_rx_id_count = 0;
				}
				break;
			}
			case ST_RX_CHECK_PACKET_NUMBER:
			{
				d_rx_packet_number[d_rx_packet_number_index] = in[ni];
				d_rx_packet_number_index++;
				ni++;
				if(d_rx_packet_number_index==3)
				{
					d_rx_packet_number_index = 0;
					unsigned char a, b, c;
					unsigned char rx_pkt_no = 0x00;
					bool recv_pkt = false;
					a = d_rx_packet_number[0];
					b = d_rx_packet_number[1];
					c = d_rx_packet_number[2];
					if(a==b) { rx_pkt_no = a; recv_pkt = true; }
					else 
					{
						if(b==c) { rx_pkt_no = b; recv_pkt = true; }
						else
						{
							if(c==a) { rx_pkt_no = c; recv_pkt = true; }
						}
					}
					if(recv_pkt == true)
					{
						if(rx_pkt_no==0xFF||rx_pkt_no==0x00)
						{
							d_rx_pkt_no = rx_pkt_no;
							d_rx_state = ST_RX_CHECK_CONTROL_INFO;
						}
						else
						{
							d_rx_pkt_no = rx_pkt_no;
							if(d_rx_node_id==d_nodeA_id||d_rx_node_id==d_nodeB_id)
							{
								// receive data from nodes A or B 
								d_rx_state = ST_RX_GET_DATA;
							}
							if(d_rx_node_id==d_nodeC_id)
							{
								// request to REtrans packet from node C
								int idx = (int)d_rx_pkt_no-1;
								d_request_retrans_packet_no_buffer[idx] = d_rx_pkt_no;
								d_rx_state = ST_RX_IDLE;
							}
						}
					}
					else
					{
						d_rx_state = ST_RX_IDLE;
					}
				}
				break;
			}
			case ST_RX_CHECK_CONTROL_INFO:
			{
				d_rx_control_info[d_rx_control_info_index] = in[ni];
				d_rx_control_info_index++;
				ni++;
				if(d_rx_control_info_index==3)
				{
					d_rx_control_info_index = 0;
					unsigned char a, b, c;
					unsigned char rx_control_info = 0x00;
					bool recv_control_info = false;
					a = d_rx_control_info[0];
					b = d_rx_control_info[1];
					c = d_rx_control_info[2];
					if(a==b && b==c) { rx_control_info = a; recv_control_info = true;}
					if(recv_control_info == true)
					{
						if(d_rx_node_id==d_nodeC_id && rx_control_info==d_nodeR_id)
						{
							if(d_rx_pkt_no==0xFF)
							{
								// need to transmit request for node A
								d_tx_state = ST_HEADER_TRANS;
								d_tx_node_id = d_nodeR_id;
								d_tx_packet_number = 0x00;
								if(d_nodeA_ended_packet_number==0x00)
								{
									d_tx_control_info = d_nodeA_id;
								}
								else
								{
									if(d_nodeB_ended_packet_number!=0x00)
									{
										d_tx_control_info = d_nodeB_id;
									}
									else
									{
										d_send = false;
									}
								}
								// reset
								for(int i = 0; i<d_buffer_size; i++)
								{
									d_packet_no_nodeA_buffer[i] = 0x00;
									d_packet_no_nodeB_buffer[i] = 0x00;
									d_xored_packet_no_buffer[i] = 0x00;
									d_request_retrans_packet_no_buffer[i] = 0x00;
								}
								if(d_nodeA_send_ended_packet==false && d_nodeA_ended_packet_number!=0x00)
								{
									d_nodeA_send_ended_packet = true;
								}
								if(d_nodeB_send_ended_packet==false && d_nodeB_ended_packet_number!=0x00)
								{
									d_nodeB_send_ended_packet = true;
								}
								d_trans_data_to_nodeC = false;
								d_trans_header_index = 0;
								d_tx_control_info_index = 0;
								d_data_trans_index = 0;
								d_tx_guard_index = 0;
								d_clock = clock();
								d_send = true;
								std::cout<<"received request for node R\n";
							}
							if(d_rx_pkt_no==0x00)
							{
								// need to transmit data to node C
								d_trans_data_to_nodeC = true;
							}
						}
						if(d_rx_node_id==d_nodeA_id && d_rx_pkt_no==0xFF)
						{
							d_nodeA_ended_packet_number = rx_control_info;
						}
						if(d_rx_node_id==d_nodeB_id && d_rx_pkt_no==0xFF)
						{
							d_nodeB_ended_packet_number = rx_control_info;
						}
					}
					d_rx_state = ST_RX_IDLE;
				}
				break;
			}
			case ST_RX_GET_DATA:
			{
				d_rx_packet[d_rx_data_index] = in[ni];
				d_rx_data_index++;
				ni++;
				if(d_rx_data_index==d_packet_size)
				{
					d_rx_data_index = 0;
					d_rx_state = ST_RX_CHECK_GUARD_INTERVAL;
				}
				break;
			}
			case ST_RX_CHECK_GUARD_INTERVAL:
			{
				if(in[ni]==0x00)
				{
					d_rx_check_guard_count++;
				}
				d_rx_check_guard_index++;
				ni++;
				if(d_rx_check_guard_index==6)
				{
					if(d_rx_check_guard_index>4)
					{
						//------------------------ (added on January 16, 2015)
						// disable transmission
						d_send = false;
						//------------------------
						int idx = (int)d_rx_pkt_no-1;
						if(idx>=d_buffer_size)
						{
							d_rx_state = ST_RX_IDLE;
							d_rx_check_guard_count = 0;
							d_rx_check_guard_index = 0;
							break;
						}
						if(d_rx_node_id==d_nodeA_id && d_packet_no_nodeA_buffer[idx]==0x00)
						{
							d_packet_no_nodeA_buffer[idx] = d_rx_pkt_no;
							for(int k = 0; k<d_packet_size; k++)
							{
								int data_idx = idx*d_packet_size+k;
								d_data_nodeA_buffer[data_idx] = d_rx_packet[k];
							}
						}
						if(d_rx_node_id==d_nodeB_id&&d_packet_no_nodeB_buffer[idx]==0x00)
						{
							d_packet_no_nodeB_buffer[idx] = d_rx_pkt_no;
							for(int k = 0; k<d_packet_size; k++)
							{
								int data_idx = idx*d_packet_size+k;
								d_data_nodeB_buffer[data_idx] = d_rx_packet[k];
							}
						}
					}
					d_rx_state = ST_RX_IDLE;
					d_rx_check_guard_count = 0;
					d_rx_check_guard_index = 0;				
				}
				break;
			}
		}
	}

	if(d_send==true)
	{
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
				// Using network coding method 
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
						if(d_packet_no_nodeA_buffer[d_buffer_index]==0x00&&
					   	   d_packet_no_nodeB_buffer[d_buffer_index]==0x00)
						{
							d_buffer_index++;
							if(d_buffer_index==d_buffer_size) d_buffer_index = 0;
						}
					}
				}
			}
			else
			{
				// Without using network coding method
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
						if(d_buffer_index==d_buffer_size) { d_buffer_index = 0; }
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
					if(d_tx_packet_number==0x00) 
					{
						d_tx_state = ST_CONTROL_INFO_TRANS;
					}
					else
					{
						if(d_tx_control_info==d_nodeA_id)
						{
							int c = 0;
							bool c1 = false;
							for(int i = 0; i<d_buffer_size; i++)
							{
								if(d_packet_no_nodeA_buffer[i]!=0x00) 
								{ 
									c++;
									if(c==(int)d_nodeA_ended_packet_number)
									{
										// ended packet
										d_tx_node_id = d_nodeR_id;
										d_tx_packet_number = 0x00;
										if(d_nodeB_ended_packet_number==0x00)
										{
											d_tx_control_info = d_nodeB_id;
										}
										else
										{
											d_tx_control_info = d_nodeC_id;
										}
										//--------------------- (added on January 16, 2015)
										// enable transmission
										d_send = true;
										//---------------------
										c1 = true;
										break;
									}
								}
							}
							if(c1==true) { break; }
							if(c==d_buffer_size)
							{
								// receive full buffer
								d_tx_node_id = d_nodeR_id;
								d_tx_packet_number = 0x00;
								if(d_nodeB_ended_packet_number==0x00)
								{
									d_tx_control_info = d_nodeB_id;
								}
								else
								{
									d_tx_control_info = d_nodeC_id;
								}
								//--------------------- (added on January 16, 2015)
								// enable transmission
								d_send = true;
								//---------------------
								break;
							}
							// send confirm received packet to node A
							for(int i = 0; i<d_buffer_size; i++)
							{
								int idx = d_buffer_index+i+1;
								if(idx==d_buffer_size) { idx = 0; }
								if(d_packet_no_nodeA_buffer[idx]!=0x00)
								{
									d_tx_packet_number = d_packet_no_nodeA_buffer[i];
									d_buffer_index = idx;
									break;
								}
							}
						}
						if(d_tx_control_info==d_nodeB_id)
						{
							int c = 0;
							bool c1 = false;
							for(int i = 0; i<d_buffer_size; i++)
							{
								if(d_packet_no_nodeB_buffer[i]!=0x00) 
								{ 
									c++;
									if(c==(int)d_nodeB_ended_packet_number)
									{
										d_tx_node_id = d_nodeR_id;
										d_tx_packet_number = 0x00;
										d_tx_control_info = d_nodeC_id;
										c1 = true;
										break;
									}
								}
							}
							if(c1==true) { break; }
							if(c==d_buffer_size)
							{
								d_tx_node_id = d_nodeR_id;
								d_tx_packet_number = 0x00;
								d_tx_control_info = d_nodeC_id;
								break;
							}
							// send confirm received packet to node B
							for(int i = 0; i<d_buffer_size; i++)
							{
								int idx = d_buffer_index+i+1;
								if(idx==d_buffer_size) { idx = 0; }
								if(d_packet_no_nodeB_buffer[idx]!=0x00)
								{
									d_tx_packet_number = d_packet_no_nodeB_buffer[i];
									d_buffer_index = idx;
									break;
								}
							}
						}
						if(d_tx_control_info==d_nodeC_id)
						{
							//forward data to node C
							d_tx_state = ST_DATA_TRANS;
						}
					}
				}
			}
			break;
		}
		case ST_CONTROL_INFO_TRANS:
		{
			out[no] = d_tx_control_info;
			d_tx_control_info_index++;
			no++;
			if(d_tx_control_info_index==3)
			{
				d_tx_control_info_index = 0;
				if(d_tx_control_info==d_nodeA_id)
				{
					for(int i = 0; i<d_buffer_size; i++)
					{
						if(d_packet_no_nodeA_buffer[i]!=0x00)
						{
							d_tx_node_id = d_nodeA_id;
							d_tx_packet_number = d_packet_no_nodeA_buffer[i];
							d_buffer_index = i;
							break;
						}
					}
				}
				if(d_tx_control_info==d_nodeB_id)
				{
					for(int i = 0; i<d_buffer_size; i++)
					{
						if(d_packet_no_nodeB_buffer[i]!=0x00)
						{
							d_tx_node_id = d_nodeB_id;
							d_tx_packet_number = d_packet_no_nodeB_buffer[i];
							d_buffer_index = i;
							break;
						}
					}
				}
				if(d_tx_control_info==d_nodeC_id)
				{
					if(d_trans_data_to_nodeC==true)
					{
						d_tx_state = ST_MANAGE_DATA_TRANS;
						d_buffer_index = 0;
						break;
					}
				}
				/* Count if time is out, switch to ST_CHECK_TIMEOUT*/
				d_clock_diff = (clock() - d_clock) / CLOCKS_PER_MSEC;
				if (d_clock_diff > d_timeout_ms)
				{
					d_tx_state = ST_CHECK_TIMEOUT;
					d_send = false;
					d_clock = clock();
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
				// Using network coding method
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
			else
			{
				// Without using network coding method
				if(d_shift==0)
				{
					// forward nodeA's packet
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
					// forward nodeB's packet
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
					bool c = false;
					for(int i = 0; i<d_buffer_size; i++)
					{
						int idx = d_buffer_index+i+1;
						if(idx==d_buffer_size) { idx = 0; }
						if(d_request_retrans_packet_no_buffer[idx]!=0x00)
						{
							d_buffer_index = idx;
							d_tx_node_id = d_nodeA_id;
							d_tx_packet_number = d_packet_no_nodeA_buffer[idx];
							d_tx_state = ST_HEADER_TRANS;
							c = true;
							break;
						}
					}
					if(c==false)
					{
						d_buffer_index++;
						if(d_buffer_index==d_buffer_size)  { d_buffer_index = 0; }
						d_tx_state = ST_MANAGE_DATA_TRANS;

					}
				}
				else
				{
					d_buffer_index++;
					if(d_buffer_index==d_buffer_size) { d_buffer_index = 0; }
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
			out[no] = d_tx_ended_packet_number;
			d_tx_ended_packet_number_index++;
			no++;
			if(d_tx_ended_packet_number_index==3)
			{
				d_tx_ended_packet_number_index = 0;
				d_tx_state = ST_MANAGE_DATA_TRANS;
			}
		}
		case ST_CHECK_TIMEOUT:
		{
			d_clock_diff = (clock() - d_clock) / CLOCKS_PER_MSEC;
			if (d_clock_diff > d_timeout_ms)
			{
				d_tx_state = ST_HEADER_TRANS;
				d_send = true;
				d_clock = clock();
			}
			break;
		}
		}
	}
	}

        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (ni);

        // Tell runtime system how many output items we produced.
        return no;
    }
  } /* namespace DNC */
} /* namespace gr */

