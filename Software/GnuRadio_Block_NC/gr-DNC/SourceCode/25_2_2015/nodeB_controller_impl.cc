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
#include "nodeB_controller_impl.h"

namespace gr {
  namespace DNC {

    nodeB_controller::sptr
    nodeB_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id)
    {
      return gnuradio::get_initial_sptr
        (new nodeB_controller_impl(buffer_size, packet_size, guard_interval, nodeA_id, nodeB_id, nodeC_id, nodeR_id));
    }

    /*
     * The private constructor
     */
    nodeB_controller_impl::nodeB_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char nodeA_id, unsigned char nodeB_id, unsigned char nodeC_id, unsigned char nodeR_id)
      : gr::block("nodeB_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), 
	      d_nodeA_id(nodeA_id), d_nodeB_id(nodeB_id), d_nodeC_id(nodeC_id), d_nodeR_id(nodeR_id),
	      d_load_data(false), d_transmit(false), d_rx_node_id(0x00), d_check_rx_id_index(0), d_check_rx_id_count(0),
	      d_check_rx_packet_number_index(0), d_check_rx_node_req_id_index(0), d_end_data(false),
	      d_load_data_index(0), d_number_of_session_packets(0), d_load_packet_number(0x01), d_load_packet_index(0),
	      d_tx_buffer_index(0), d_tx_header_index(0), d_tx_data_index(0), d_tx_guard_index(0), 	
	      d_tx_ended_packet_number_index(0),
	      d_tx_state(ST_TX_BUFFER_MANAGEMENT), d_rx_state(ST_RX_IDLE)
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
		d_rx_packet_number[i] = 0x00;
		d_rx_node_req_id[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    nodeB_controller_impl::~nodeB_controller_impl()
    {
    }

    void
    nodeB_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    void
    nodeB_controller_impl::reset()
    {
	d_tx_buffer_index = 0;
	d_tx_header_index = 0;
	d_tx_data_index = 0;
	d_tx_guard_index = 0;
        d_tx_state = ST_TX_BUFFER_MANAGEMENT;
	for(int i = 0; i<d_buffer_size; i++)
	{
		d_transmitted_packet_number[i] = 0x00;
		d_confirm_packet_number[i] = 0x00;
	}
    }

    int
    nodeB_controller_impl::general_work (int noutput_items,
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
		if(d_load_data==true)
		{
			if(ni1<nInputItems1)
			{
				if(d_load_data_index==0)
				{
					if(in1[ni1]==0x01)
					{
						std::cout<<"End Data"<<std::endl;
						std::cout<<"Number of Loaded Packets = "<<d_number_of_session_packets<<"\n";
						d_ended_packet_number = (unsigned char) d_number_of_session_packets;
						d_load_packet_number = 0x01;
						d_load_packet_index = 0;
						d_load_data_index = 0;
						d_load_data = false;
						d_end_data = true;
						d_transmit = true;
						reset();
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
						if(d_load_packet_index==d_buffer_size)
						{
							d_load_packet_number = 0x01;
							d_load_packet_index = 0;
							std::cout<<"Number of Loaded Packets = "<<d_number_of_session_packets<<"\n";
							d_load_data = false;
							d_transmit = true;
							reset();
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
			switch(d_rx_state)
			{
				case ST_RX_IDLE:
				{
					if(in0[ni0]==d_nodeR_id)
					{
						d_rx_node_id = d_nodeR_id;
						d_rx_state = ST_RX_CHECK_NODE_ID;
						d_check_rx_id_index++;
						d_check_rx_id_count++;
					}
					if(in0[ni0]==d_nodeB_id)
					{
						d_rx_node_id = d_nodeB_id;
						d_rx_state = ST_RX_CHECK_NODE_ID;
						d_check_rx_id_index++;
						d_check_rx_id_count++;
					}
					ni0++;
					break;
				}
				case ST_RX_CHECK_NODE_ID:
				{
					if(in0[ni0]==d_rx_node_id) { d_check_rx_id_count++; }
					d_check_rx_id_index++;
					ni0++;
					if(d_check_rx_id_index==3)
					{
						if(d_check_rx_id_count==3)
						{
							d_rx_state = ST_RX_CHECK_PACKET_NUMBER;
						}
						else
						{
							d_rx_state = ST_RX_IDLE;
							ni0 = ni0 - 1;
						}
						d_check_rx_id_index = 0;
						d_check_rx_id_count = 0;
					}
					break;
				}
				case ST_RX_CHECK_PACKET_NUMBER:
				{
					d_rx_packet_number[d_check_rx_packet_number_index] = in0[ni0];
					d_check_rx_packet_number_index++;
					ni0++;
					if(d_check_rx_packet_number_index==3)
					{
						d_check_rx_packet_number_index = 0;
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
							if(rx_pkt_no==0x00 && d_rx_node_id==d_nodeR_id)
							{
								d_rx_state = ST_RX_CHECK_NODE_REQUEST;
							}
							if(d_rx_node_id==d_nodeB_id && rx_pkt_no!=0x00 && d_transmit==true)
							{
								// confirm sucessfully transmitted packets
								int idx = (int) rx_pkt_no - 1;
								d_confirm_packet_number[idx] = 0x01;
							}
						}
						else
						{
							d_rx_state = ST_RX_IDLE;
						}
					}
					break;
				}
				case ST_RX_CHECK_NODE_REQUEST:
				{
					d_rx_node_req_id[d_check_rx_node_req_id_index] = in0[ni0];
					d_check_rx_node_req_id_index++;
					ni0++;
					if(d_check_rx_node_req_id_index==3)
					{
						d_check_rx_node_req_id_index = 0;
						unsigned char a, b, c;
						unsigned char rx_node_req_id = 0x00;
						bool recv_node_req = false;
						a = d_rx_node_req_id[0];
						b = d_rx_node_req_id[1];
						c = d_rx_node_req_id[2];
						if(a==b && b==c) { rx_node_req_id = a; recv_node_req = true;}
						if(recv_node_req == true)
						{
							if(rx_node_req_id==d_nodeB_id&&d_transmit==false)
							{
								std::cout<<"received request for node B\n";
								d_load_data = true;
							}
							if(((rx_node_req_id==d_nodeA_id)||(rx_node_req_id==d_nodeC_id))&&d_transmit==true)
							{
								for(int i = 0; i<d_buffer_size; i++)
								{
									d_loaded_packet_number[i] = 0x00;
									d_transmitted_packet_number[i] = 0x00;
									d_confirm_packet_number[i] = 0x00;
								}
								if(rx_node_req_id==d_nodeA_id)
								{
									std::cout<<"received request for node A\n";
								}
								if(rx_node_req_id==d_nodeC_id)
								{
									std::cout<<"received request for node C\n";
								}
								d_transmit = false;
							}
						}
						d_rx_state = ST_RX_IDLE;
					}
					break;	
				}
			}
		}
	}

	if(d_transmit==true)
	{
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
							d_tx_buffer_index = 0;
							if(d_number_of_session_packets<d_buffer_size)
							{
								d_tx_state = ST_TX_END_PACKET_HEADER_TRANS;
							}
							else
							{
								out[no] = 0x00;
								no++;
							}
						}
					}
					break;
				}
				case ST_TX_HEADER_TRANS:
				{
					if(d_tx_header_index<3)
					{
						out[no] = d_nodeB_id;
						d_tx_header_index++;
						no++;
						break;
					}
					if(d_tx_header_index>=3)
					{
						out[no] = d_loaded_packet_number[d_tx_buffer_index];
						d_transmitted_packet_number[d_tx_buffer_index] = d_loaded_packet_number[d_tx_buffer_index];
						d_tx_header_index++;
						no++;
						if(d_tx_header_index==6)
						{
							d_tx_header_index = 0;
							d_tx_state = ST_TX_DATA_TRANS;
						}
					}
					break;
				}
				case ST_TX_DATA_TRANS:
				{
					out[no] = d_transmitted_data_buffer[d_tx_buffer_index*d_packet_size+d_tx_data_index];
					no++;
					d_tx_data_index++;
					if(d_tx_data_index==d_packet_size)
					{
						d_tx_data_index = 0;
						d_tx_state = ST_TX_GUARD_INTERVAL_TRANS;
					}
					break;
				}
				case ST_TX_GUARD_INTERVAL_TRANS:
				{
					out[no] = 0x00;
					d_tx_guard_index++;
					no++;
					if(d_tx_guard_index==d_guard_interval)
					{
						d_tx_guard_index = 0;
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
						else
						{
							d_tx_state = ST_TX_BUFFER_MANAGEMENT;
						}
					}
					break;
				}
				case ST_TX_END_PACKET_HEADER_TRANS:
				{
					if(d_tx_header_index<3)
					{
						out[no] = d_nodeB_id;
						d_tx_header_index++;
						no++;
						break;
					}
					if(d_tx_header_index>=3)
					{
						out[no] = 0xFF;
						d_tx_header_index++;
						no++;
						if(d_tx_header_index==6)
						{
							d_tx_header_index = 0;
							d_tx_state = ST_TX_END_PACKET_NUMBER_TRANS;
						}
					}
					break;
				}
				case ST_TX_END_PACKET_NUMBER_TRANS:
				{
					out[no] = d_ended_packet_number;
					d_tx_ended_packet_number_index++;
					no++;
					if(d_tx_ended_packet_number_index==3)
					{
						d_tx_ended_packet_number_index = 0;
						d_tx_state = ST_TX_BUFFER_MANAGEMENT;
					}
				}
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
