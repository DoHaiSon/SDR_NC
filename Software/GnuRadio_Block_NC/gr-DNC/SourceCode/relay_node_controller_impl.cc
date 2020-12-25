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
#include "relay_node_controller_impl.h"

namespace gr {
  namespace DNC {

    relay_node_controller::sptr
    relay_node_controller::make(int packet_size, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id)
    {
      return gnuradio::get_initial_sptr
        (new relay_node_controller_impl(packet_size, guard_interval, end_nodeA_id, end_nodeB_id, relay_node_id));
    }

    /*
     * The private constructor
     */
    relay_node_controller_impl::relay_node_controller_impl(int packet_size, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id)
      : gr::block("relay_node_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_packet_size(packet_size), d_guard_interval(guard_interval), d_end_nodeA_id(end_nodeA_id), d_end_nodeB_id(end_nodeB_id), 
	      d_relay_node_id(relay_node_id),  d_buffer_size(127), d_check_nodeA_id_index(0), d_check_nodeA_id_count(0), 
	      d_check_nodeB_id_count(0), d_check_nodeB_id_index(0), d_rx0_packet_number_index(0), d_rx1_packet_number_index(0), 
	      d_check_rx0_req(false), d_check_rx1_req(false), d_rx0_pkt_no(0x00), d_rx1_pkt_no(0x00), d_rx0_data_index(0), 
	      d_rx1_data_index(0), d_confirm_index(0), d_packet_out_index(0), d_node_id_out_idx(0), d_packet_no_out_idx(0), 
	      d_xor_data_out_idx(0), d_guard_idx(0), d_rx0_check_end_packet_count(0), d_rx0_check_end_packet_index(0),
	      d_rx1_check_end_packet_count(0), d_rx1_check_end_packet_index(0), d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_TX_IDLE)
    {
	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please make guard interval be greater than or equal to 6...!\n");

	if (end_nodeA_id==0x00||end_nodeB_id==0x00)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from 0...!!!\n");

	if (relay_node_id==0x00)
		throw std::runtime_error("Invalid parameter! Relay Node ID must be different from 0...!!!\n");

	if (end_nodeA_id==relay_node_id||end_nodeB_id==relay_node_id)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from Relay Node ID...!!!\n");

	d_range_packet_no_nodeA.resize(d_buffer_size);
	std::fill(d_range_packet_no_nodeA.begin(), d_range_packet_no_nodeA.end(), 0x00);

	d_range_packet_no_nodeB.resize(d_buffer_size);
	std::fill(d_range_packet_no_nodeB.begin(), d_range_packet_no_nodeB.end(), 0x00);
	
	unsigned char pkt_no = 0x00;
	for(int i = 0; i<d_buffer_size; i++)
	{
		pkt_no++;
		d_range_packet_no_nodeA[i] = pkt_no;
		d_range_packet_no_nodeB[i] = pkt_no;
	}

	d_packet_no_nodeA.resize(d_buffer_size);
	std::fill(d_packet_no_nodeA.begin(), d_packet_no_nodeA.end(), 0x00);

	d_packet_no_nodeB.resize(d_buffer_size);
	std::fill(d_packet_no_nodeB.begin(), d_packet_no_nodeB.end(), 0x00);

	d_xored_packet_no.resize(d_buffer_size);
	std::fill(d_xored_packet_no.begin(), d_xored_packet_no.end(), 0x00);
	
	d_packet_nodeA.resize(d_buffer_size*packet_size);
	std::fill(d_packet_nodeA.begin(), d_packet_nodeA.end(), 0x00);

	d_packet_nodeB.resize(d_buffer_size*packet_size);
	std::fill(d_packet_nodeB.begin(), d_packet_nodeB.end(), 0x00);
	
	d_xored_packet.resize(d_buffer_size*packet_size);
	std::fill(d_xored_packet.begin(), d_xored_packet.end(), 0x00);
	
	d_rx0_packet.resize(packet_size);
	std::fill(d_rx0_packet.begin(), d_rx0_packet.end(), 0x00);

	d_rx1_packet.resize(packet_size);
	std::fill(d_rx1_packet.begin(), d_rx1_packet.end(), 0x00);

	d_packet_no_out.resize(d_buffer_size);
	std::fill(d_packet_no_out.begin(), d_packet_no_out.end(), 0x00);

	d_packet_out.resize(d_buffer_size*packet_size);
	std::fill(d_packet_out.begin(), d_packet_out.end(), 0x00);

	for(int i = 0; i<3; i++)
	{
		d_confirm_req[i] = d_relay_node_id;
		d_confirm_req[i+3] = 0x00;
		d_rx0_packet_number[i] = 0x00;
		d_rx1_packet_number[i] = 0x00;
	}
    }

    /*
     * Our virtual destructor.
     */
    relay_node_controller_impl::~relay_node_controller_impl()
    {
    }

    void
    relay_node_controller_impl::set_data_out(int xored_pos, unsigned char xored_pkt_no)
    {
	for(int i = 0; i<d_buffer_size; i++)
	{
		if(d_packet_no_out[i]==0x00)
		{
			d_packet_no_out[i] = xored_pkt_no;
			int idx_out = 0;
			for(int j = 0; j<d_packet_size; j++)
			{
				idx_out = i*d_packet_size+j;
				d_packet_out[idx_out] = d_xored_packet[xored_pos*d_packet_size+j];
			}
			break;
		}
	}
	return;
    }
    void
    relay_node_controller_impl::shift_xored_data_buffer()
    {
	for(int i = 0; i<d_buffer_size; i++)
	{
		if(d_xored_packet_no[i]!=0x00)
		{
			bool c1 = false;
			bool c2 = false;
			for(int j = 0; j<d_buffer_size; j++)
			{
				if(d_packet_no_nodeA[j]==d_xored_packet_no[i])
				{
					c1 = true;
				}
				if(d_packet_no_nodeB[j]==d_xored_packet_no[i])
				{
					c2 = true;
				}
			}
			if(c1==false&&c2==false)
			{
				// shift xor data buffer
				d_xored_packet_no[i] = 0x00;
			}
		}
	}
	return;
    }
    void
    relay_node_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    relay_node_controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
	int nOutputItems = noutput_items;
	int bound = 2*d_packet_size;
        const unsigned char *in0 = (const unsigned char *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];
        unsigned char *out = (unsigned char *) output_items[0];

	int ni0 = 0;
	int ni1 = 0;
	int no = 0;

	while(ni0<bound && ni1<bound && no<bound)
	{
		/* Receiving states from node A */
		switch(d_rx0_state)
		{
		case ST_RX0_IDLE:
		{
			if(in0[ni0]==d_end_nodeA_id)
			{
				d_rx0_state = ST_RX0_CHECK_NODEA_ID;
				d_check_nodeA_id_index++;
				d_check_nodeA_id_count++;
			}
			ni0++;
			break;
		}
		case ST_RX0_CHECK_NODEA_ID:
		{
			if(in0[ni0]==d_end_nodeA_id)
			{
				d_check_nodeA_id_count++;
			}
			ni0++;
			d_check_nodeA_id_index++;
			if(d_check_nodeA_id_index==3)
			{
				if(d_check_nodeA_id_count==3)
				{
					d_rx0_state = ST_RX0_CHECK_PACKET_NUMBER;
				}
				else
				{
					d_rx0_state = ST_RX0_IDLE;
					ni0 = ni0 - 1;
				}
				d_check_nodeA_id_index = 0;
				d_check_nodeA_id_count = 0;
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
				if(a==b&&b==c) { rx0_pkt_no = a; recv_pkt = true;}
				/*
				if(a==b) { rx0_pkt_no = a; recv_pkt = true;}
				else 
				{
					if(b==c) { rx0_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) { rx0_pkt_no = c; recv_pkt = true;}
					}
				}*/
				if(recv_pkt == true)
				{
					if(rx0_pkt_no==0x00&&d_check_rx0_req==false)
					{
						d_check_rx0_req = true;
						d_rx0_state = ST_RX0_IDLE;
						//d_tx_state = ST_BUFFER_MANAGEMENT; ?? what state of transmission
					}
					else 
					{
						if(rx0_pkt_no!=0)
						{
							d_rx0_pkt_no = rx0_pkt_no;
							//std::cout<<"branch A recved packet no "<<(int)rx0_pkt_no<<"\n ";
							d_rx0_state = ST_RX0_GET_DATA;
						}
						else
						{
							d_rx0_state = ST_RX0_IDLE;
						}
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
					//std::cout<<"d_rx0_pkt_no = "<<(int)d_rx0_pkt_no<<"\n";

	for(int i = 0; i<d_buffer_size; i++)
	{
		//std::cout<<(int)d_packet_no_nodeA[i]<<", ";
	}
	//std::cout<<"\n------------------------------------------\n";
	for(int i = 0; i<d_buffer_size; i++)
	{
		//std::cout<<(int)d_packet_no_nodeB[i]<<", ";
	}
	//std::cout<<"\n------------------------------------------\n";
	for(int i = 0; i<d_buffer_size; i++)
	{
		std::cout<<(int)d_xored_packet_no[i]<<", ";
	}
	std::cout<<"\n=====================================\n";


					bool check_pkt_exist = false;
					for(int i = 0; i<d_buffer_size; i++)
					{
						if(d_packet_no_nodeA[i]==d_rx0_pkt_no)
						{
							check_pkt_exist = true;
							break;
						}
					}
					if(check_pkt_exist==false)
					{
						bool check_shift_recv_data_buffer = true;
						int idx = 0;
						for(int j = 0; j<d_buffer_size; j++)
						{
							if(d_range_packet_no_nodeA[j]==d_rx0_pkt_no)
							{
								check_shift_recv_data_buffer = false;
								idx = j;
								break;
							}	
						}
						if(check_shift_recv_data_buffer==true)
						{
							//std::cout<<"branch A check_shift_recv_data_buffer=true "<<(int)d_rx0_pkt_no<<std::endl;
							// shift expected range packet no nodeA
							unsigned char range_begin = 0x00;
							if((int)d_rx0_pkt_no<=127)
							{
								int temp = (int)d_rx0_pkt_no+128;
								range_begin = (unsigned char) temp;
								if(range_begin==0xFF)
								{
									range_begin = 0x01;
								}
							}
							else
							{
								int temp = (int)d_rx0_pkt_no-126;
								range_begin = (unsigned char) temp;
							}
							for(int j = 0; j<d_buffer_size; j++)
							{
								d_range_packet_no_nodeA[j] = range_begin;
								range_begin++;
								if(range_begin==0xFF)
								{
									range_begin = 0x01;
								}
							}
							// shift recv data buffer
							for(int j = 0; j<d_buffer_size; j++)
							{
								bool c = false;
								for(int k = 0; k<d_buffer_size; k++)
								{
									if(d_packet_no_nodeA[j]==d_range_packet_no_nodeA[k])
									{
										c = true;
										break;
									}
								}
								if(c==false)
								{
									d_packet_no_nodeA[j] = 0x00;
								}
							}
							// check shift xor data buffer
							std::cout<<"\npacket no range from: "<<(int)d_range_packet_no_nodeA[0]<<" to "<<(int)d_range_packet_no_nodeA[126]<<"\n";
							std::cout<<"received packet not in range: "<<(int)d_rx0_pkt_no<<"\n";


							shift_xored_data_buffer();
						}
						// put packet in recv data buffer by finding zero pkt no
						for(int j = 0; j<d_buffer_size; j++)
						{
							if(d_packet_no_nodeA[j]==0x00)
							{
								d_packet_no_nodeA[j] = d_rx0_pkt_no;
								for(int k = 0; k<d_packet_size; k++)
								{
									int idx = j*d_packet_size+k;
									d_packet_nodeA[idx] = d_rx0_packet[k];
								}
								break;
							}
						}
					}
					// the same processing from here
					//check xor packet existed
					bool check_xor_pkt_exist = false;
					int xor_idx = 0;
					for(int j = 0; j<d_buffer_size; j++)
					{
						if(d_xored_packet_no[j]==d_rx0_pkt_no)
						{
							check_xor_pkt_exist = true;
							xor_idx = j;
							break;
						}
					}
					if(check_xor_pkt_exist==true)
					{
						//set data out
						set_data_out(xor_idx, d_rx0_pkt_no);
					}
					else
					{
						//check xor available
						for(int k = 0; k<d_buffer_size; k++)
						{
							if(d_packet_no_nodeB[k]==d_rx0_pkt_no)
							{
								for(int l = 0; l<d_buffer_size; l++)
								{
									if(d_xored_packet_no[l]==0x00)
									{
										d_xored_packet_no[l] = d_rx0_pkt_no;
										//std::cout<<"xored pos = "<<l<<"\n";
										//std::cout<<"branch A d_xored_packet_no = "<<(int)d_rx0_pkt_no<<"\n";
										int xor_idx = 0;
										int idx_nB = 0;
										unsigned char xored_sym = 0x00;
										for(int m = 0; m<d_packet_size; m++)
										{
											xor_idx = l*d_packet_size+m;
											idx_nB = k*d_packet_size+m;
											// xor data
											xored_sym = d_rx0_packet[m]^d_packet_nodeB[idx_nB];
											//std::cout<<(int)xored_sym<<", ";
											d_xored_packet[xor_idx] = xored_sym;
										}
										//std::cout<<std::endl;
										// set data out
										set_data_out(l, d_rx0_pkt_no);
										break;
									}
								}
								break;
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
		/* Receiving states from node B */
		switch(d_rx1_state)
		{
		case ST_RX1_IDLE:
		{
			if(in1[ni1]==d_end_nodeB_id)
			{
				d_rx1_state = ST_RX1_CHECK_NODEB_ID;
				d_check_nodeB_id_index++;
				d_check_nodeB_id_count++;
			}
			ni1++;
			break;
		}
		case ST_RX1_CHECK_NODEB_ID:
		{
			if(in1[ni1]==d_end_nodeB_id)
			{
				d_check_nodeB_id_count++;
			}
			ni1++;
			d_check_nodeB_id_index++;
			if(d_check_nodeB_id_index==3)
			{
				if(d_check_nodeB_id_count==3)
				{
					d_rx1_state = ST_RX1_CHECK_PACKET_NUMBER;
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
					ni1 = ni1 - 1;
				}
				d_check_nodeB_id_index = 0;
				d_check_nodeB_id_count = 0;
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
				if(a==b&&b==c) { rx1_pkt_no = a; recv_pkt = true;}
				/*
				if(a==b) { rx1_pkt_no = a; recv_pkt = true;}
				else 
				{
					if(b==c) { rx1_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) { rx1_pkt_no = c; recv_pkt = true;}
					}
				}*/
				if(recv_pkt == true)
				{
					if(rx1_pkt_no==0x00&&d_check_rx1_req==false)
					{
						d_check_rx1_req = true;
						d_rx1_state = ST_RX1_IDLE;
						//d_tx_state = ST_BUFFER_MANAGEMENT; ?? what state of transmission
					}
					else 
					{
						if(rx1_pkt_no!=0)
						{
							d_rx1_pkt_no = rx1_pkt_no;
							//std::cout<<"branch B recved packet no "<<(int)rx1_pkt_no<<"\n ";
							d_rx1_state = ST_RX1_GET_DATA;
						}
						else
						{
							d_rx1_state = ST_RX1_IDLE;
						}
					}
				}
				else
				{
					d_rx1_state = ST_RX1_IDLE;
				}
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
					bool check_pkt_exist = false;
					for(int i = 0; i<d_buffer_size; i++)
					{
						if(d_packet_no_nodeB[i]==d_rx1_pkt_no)
						{
							check_pkt_exist = true;
							break;
						}
					}
					if(check_pkt_exist==false)
					{
						bool check_shift_recv_data_buffer = true;
						int idx = 0;
						for(int j = 0; j<d_buffer_size; j++)
						{
							if(d_range_packet_no_nodeB[j]==d_rx1_pkt_no)
							{
								check_shift_recv_data_buffer = false;
								idx = j;
								break;
							}	
						}
						if(check_shift_recv_data_buffer==true)
						{
							//std::cout<<"branhch B check_shift_recv_data_buffer=true "<<(int)d_rx1_pkt_no<<std::endl;
							// shift expected range packet no nodeA
							unsigned char range_begin = 0x00;
							if((int)d_rx1_pkt_no<=127)
							{
								int temp = (int)d_rx1_pkt_no+128;
								range_begin = (unsigned char) temp;
								if(range_begin==0xFF)
								{
									range_begin = 0x01;
								}
							}
							else
							{
								int temp = (int)d_rx1_pkt_no-126;
								range_begin = (unsigned char) temp;
							}
							for(int j = 0; j<d_buffer_size; j++)
							{
								d_range_packet_no_nodeB[j] = range_begin;
								range_begin++;
								if(range_begin==0xFF)
								{
									range_begin = 0x01;
								}
							}
							// shift recv data buffer
							for(int j = 0; j<d_buffer_size; j++)
							{
								bool c = false;
								for(int k = 0; k<d_buffer_size; k++)
								{
									if(d_packet_no_nodeB[j]==d_range_packet_no_nodeB[k])
									{
										c = true;
										break;
									}
								}
								if(c==false)
								{
									d_packet_no_nodeB[j] = 0x00;
								}
							}
							// check shift xor data buffer
							shift_xored_data_buffer();
						}
						// put packet in recv data buffer by finding zero pkt no
						for(int j = 0; j<d_buffer_size; j++)
						{
							if(d_packet_no_nodeB[j]==0x00)
							{
								d_packet_no_nodeB[j] = d_rx1_pkt_no;
								for(int k = 0; k<d_packet_size; k++)
								{
									int idx = j*d_packet_size+k;
									d_packet_nodeB[idx] = d_rx1_packet[k];
								}
								break;
							}
						}
					}
					// the same processing from here
					//check xor packet existed
					bool check_xor_pkt_exist = false;
					int xor_idx = 0;
					for(int j = 0; j<d_buffer_size; j++)
					{
						if(d_xored_packet_no[j]==d_rx1_pkt_no)
						{
							check_xor_pkt_exist = true;
							xor_idx = j;
							break;
						}
					}
					if(check_xor_pkt_exist==true)
					{
						//set data out
						set_data_out(xor_idx, d_rx1_pkt_no);
					}
					else
					{
						//check xor available
						for(int k = 0; k<d_buffer_size; k++)
						{
							if(d_packet_no_nodeA[k]==d_rx1_pkt_no)
							{
								//std::cout<<"POS = <<"<<k<<"\n";
								for(int l = 0; l<d_buffer_size; l++)
								{
									if(d_xored_packet_no[l]==0x00)
									{
										d_xored_packet_no[l] = d_rx1_pkt_no;
										//std::cout<<"xored pos = "<<l<<"\n";
										//std::cout<<"branch B d_xored_packet_no = "<<(int)d_rx1_pkt_no<<"\n";
										int xor_idx = 0;
										int idx_nA = 0;
										unsigned char xored_sym = 0x00;
										for(int m = 0; m<d_packet_size; m++)
										{
											xor_idx = l*d_packet_size+m;
											idx_nA = k*d_packet_size+m;
											// xor data
											xored_sym = d_rx1_packet[m]^d_packet_nodeA[idx_nA];
											//std::cout<<(int)xored_sym<<", ";
											d_xored_packet[xor_idx] = xored_sym;
										}
										//std::cout<<std::endl;
										// set data out
										set_data_out(l, d_rx1_pkt_no);
										break;
									}
								}
								break;
							}
						}
					}
				}
				d_rx1_state = ST_RX1_IDLE;
				d_rx1_check_end_packet_index = 0;
				d_rx1_check_end_packet_count = 0;
			}
			break;
		}	
		}
		/* Transmission states */
		switch(d_tx_state)
		{
		case ST_TX_IDLE:
		{
			out[no] = 0x00;
			no++;
			if(d_check_rx0_req==true&&d_check_rx1_req==true)
			{
				std::cout<<"confirm trans"<<std::endl;
				d_tx_state = ST_TX_CONFIRM_TRANS;
			}
			break;
		}
		case ST_TX_CONFIRM_TRANS:
		{
			out[no] = d_confirm_req[d_confirm_index];
			d_confirm_index++;
			no++;
			if(d_confirm_index==6)
			{
				d_confirm_index = 0;
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_no_out[i]!=0x00)
					{
						d_packet_out_index = i;
						d_tx_state = ST_TX_NODE_ID_TRANS;
						break;
					}
				}
			}
			break;
		}
		case ST_TX_NODE_ID_TRANS:
		{
			out[no] = d_relay_node_id;
			no++;
			d_node_id_out_idx++;
			if(d_node_id_out_idx==3)
			{
				d_node_id_out_idx = 0;
				d_tx_state = ST_TX_PACKET_NO_TRANS;
			}
			break;
		}
		case ST_TX_PACKET_NO_TRANS:
		{
			out[no] = d_packet_no_out[d_packet_out_index];
			no++;
			d_packet_no_out_idx++;
			if(d_packet_no_out_idx==3)
			{
				d_packet_no_out_idx = 0;
				d_tx_state = ST_TX_XOR_DATA_TRANS;
			}
			break;
		}
		case ST_TX_XOR_DATA_TRANS:
		{
			out[no] = d_packet_out[d_packet_out_index*d_packet_size+d_xor_data_out_idx];
			d_xor_data_out_idx++;
			no++;
			if(d_xor_data_out_idx==d_packet_size)
			{
				d_xor_data_out_idx = 0;
				d_tx_state = ST_TX_GUARD_TRANS;
			}
			break;
		}
		case ST_TX_GUARD_TRANS:
		{
			out[no] = 0x00;
			d_guard_idx++;
			no++;
			if(d_guard_idx==d_guard_interval)
			{
				d_guard_idx = 0;
				d_packet_no_out[d_packet_out_index] = 0x00;
				bool c = false;
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_no_out[i]!=0x00)
					{
						d_packet_out_index = i;
						c = true;
						break;
					}
				}
				if(c==true)
				{
					d_tx_state = ST_TX_NODE_ID_TRANS;
				}
				else
				{
					d_tx_state = ST_TX_CHECK_DATA_OUT_BUFFER;
				}
			}
			break;
		}
		case ST_TX_CHECK_DATA_OUT_BUFFER:
		{
			out[no] = 0x00;
			no++;
			bool c = false;
			for(int i = 0; i<d_buffer_size; i++)
			{
				if(d_packet_no_out[i]!=0x00)
				{
					d_packet_out_index = i;
					c = true;
					break;
				}
			}
			if(c==true)
			{
				d_tx_state = ST_TX_NODE_ID_TRANS;
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

