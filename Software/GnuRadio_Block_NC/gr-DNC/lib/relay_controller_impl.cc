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
#include "relay_controller_impl.h"

namespace gr {
  namespace DNC {

    relay_controller::sptr
    relay_controller::make(int packet_size, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id)
    {
      return gnuradio::get_initial_sptr
        (new relay_controller_impl(packet_size, guard_interval, end_nodeA_id, end_nodeB_id, relay_node_id));
    }

    /*
     * The private constructor
     */
    relay_controller_impl::relay_controller_impl(int packet_size, int guard_interval, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id)
      : gr::block("relay_controller",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_packet_size(packet_size), d_guard_interval(guard_interval), d_end_nodeA_id(end_nodeA_id), d_end_nodeB_id(end_nodeB_id), 
	      d_relay_node_id(relay_node_id),  d_buffer_size(254), d_curr_part(0), d_check_nodeA_id_index(0), d_check_nodeA_id_count(0), 
	      d_check_nodeB_id_count(0), d_check_nodeB_id_index(0), d_rx0_packet_number_index(0), d_rx1_packet_number_index(0), 
	      d_check_rx0_req(false), d_check_rx1_req(false), d_rx0_pkt_no(0x00), d_rx1_pkt_no(0x00), d_rx0_data_index(0), 
	      d_rx1_data_index(0), d_confirm_index(0), d_packet_out_index(0), d_node_id_out_idx(0), d_packet_no_out_idx(0), 
	      d_xor_data_out_idx(0), d_guard_idx(0), d_rx0_check_end_packet_count(0), d_rx0_check_end_packet_index(0),
	      d_rx1_check_end_packet_count(0), d_rx1_check_end_packet_index(0), d_confirm_sent(false), d_check_full_xored(false),
	      d_expected_session_no(0x01), d_rx0_check_session_index(0), d_rx0_check_session_count(0), d_rx1_check_session_index(0), 	
	      d_rx1_check_session_count(0), d_rx0_state(ST_RX0_IDLE), d_rx1_state(ST_RX1_IDLE), d_tx_state(ST_TX_CONFIRM_TRANS)
    {
	if (guard_interval<6)
		throw std::runtime_error("Invalid parameter! Please make guard interval be greater than or equal to 6...!\n");

	if (end_nodeA_id==0x00||end_nodeB_id==0x00)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from 0...!!!\n");

	if (relay_node_id==0x00)
		throw std::runtime_error("Invalid parameter! Relay Node ID must be different from 0...!!!\n");

	if (end_nodeA_id==relay_node_id||end_nodeB_id==relay_node_id)
		throw std::runtime_error("Invalid parameter! End Node ID must be different from Relay Node ID...!!!\n");

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

	d_ch = 0;
	d_image_out_idx = 0;
	d_pkt_out_no = 0;

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
    relay_controller_impl::~relay_controller_impl()
    {
    }

    void
    relay_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    void
    relay_controller_impl::set_data_out(int xored_pos, unsigned char xored_pkt_no)
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


    int
    relay_controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
	int nOutputItems = noutput_items;
	int bound = 0;
	bound = std::min(nInputItems0, nInputItems1);
	bound = std::min(bound, nOutputItems);
	if(bound>(2*d_packet_size))
	{
		bound = 2*d_packet_size;
	}
	
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
				if(a==b) { rx0_pkt_no = a; recv_pkt = true;}
				else 
				{
					if(b==c) { rx0_pkt_no = b; recv_pkt = true;}
					else
					{
						if(c==a) { rx0_pkt_no = c; recv_pkt = true;}
					}
				}
				if(recv_pkt == true)
				{
					if(rx0_pkt_no!=0&&d_confirm_sent==true)
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
					if(d_packet_no_nodeA[idx]==0x00)
					{
						//d_ch++;
						//std::cout<<"\nreceived "<<d_ch<<" packets from node A\n";
						d_packet_no_nodeA[idx] = d_rx0_pkt_no;
						for(int k = 0; k<d_packet_size; k++)
						{
							int data_idx = idx*d_packet_size+k;
							d_packet_nodeA[data_idx] = d_rx0_packet[k];
							//std::cout<<(int)d_rx0_packet[k]<<", ";
						}
						if(d_packet_no_nodeB[idx]==d_rx0_pkt_no)
						{
							d_xored_packet_no[idx] = d_rx0_pkt_no;
							int sour = 0;
							for(int m = 0; m<d_packet_size; m++)
							{
								sour = idx*d_packet_size+m;
								d_xored_packet[sour] = d_rx0_packet[m]^d_packet_nodeB[sour];
							}
							set_data_out(idx, d_rx0_pkt_no);
							int check_count = 0;
							int temp_idx = 0;
							for(int m = 0; m<127; m++)
							{
								temp_idx = m+d_curr_part*127;
								if(d_xored_packet_no[temp_idx]!=0)
								{
									check_count++;
								}
							}
							if(check_count==127)
							{
								if(d_curr_part==1) { d_curr_part = 0; }
								else 		   { d_curr_part = 1; }
								std::cout<<"Move to part "<<d_curr_part<<"\n";
								for(int l = 0; l<127; l++)
								{
									temp_idx = l+d_curr_part*127;
									d_packet_no_nodeA[temp_idx] = 0x00;
									d_packet_no_nodeB[temp_idx] = 0x00;
									d_xored_packet_no[temp_idx] = 0x00;
								}
							}
						}
					}
					else
					{
						if(d_xored_packet_no[idx]==d_rx0_pkt_no)
						{
							set_data_out(idx, d_rx0_pkt_no);
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
					if(rx1_pkt_no!=0&&d_confirm_sent==true)
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
					int idx = (int)d_rx1_pkt_no - 1;
					if(d_packet_no_nodeB[idx] == 0x00)
					{
						d_packet_no_nodeB[idx] = d_rx1_pkt_no;
						for(int k = 0; k<d_packet_size; k++)
						{
							int data_idx = idx*d_packet_size+k;
							d_packet_nodeB[data_idx] = d_rx1_packet[k];
						}
						if(d_packet_no_nodeA[idx]==d_rx1_pkt_no)
						{
							d_xored_packet_no[idx] = d_rx1_pkt_no;
							int sour = 0;
							for(int m = 0; m<d_packet_size; m++)
							{
								sour = idx*d_packet_size+m;
								d_xored_packet[sour] = d_rx1_packet[m]^d_packet_nodeA[sour];
							}
							set_data_out(idx, d_rx1_pkt_no);
							int check_count = 0;
							int temp_idx = 0;
							for(int m = 0; m<127; m++)
							{
								temp_idx = m+d_curr_part*127;
								if(d_xored_packet_no[temp_idx]!=0)
								{
									check_count++;
								}
							}
							if(check_count==127)
							{
								if(d_curr_part==1) { d_curr_part = 0; }
								else 		   { d_curr_part = 1; }
								std::cout<<"Move to part "<<d_curr_part<<"\n";
								for(int l = 0; l<127; l++)
								{
									temp_idx = l+d_curr_part*127;
									d_packet_no_nodeA[temp_idx] = 0x00;
									d_packet_no_nodeB[temp_idx] = 0x00;
									d_xored_packet_no[temp_idx] = 0x00;
								}
							}
						}
					}
					else
					{
						if(d_xored_packet_no[idx] == d_rx1_pkt_no)
						{
							set_data_out(idx, d_rx1_pkt_no);
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
		/* Transmission states */
		switch(d_tx_state)
		{
		case ST_TX_CONFIRM_TRANS:
		{
			out[no] = d_confirm_req[d_confirm_index];
			d_confirm_index++;
			no++;
			/*
			if(d_ch==125)
			{
				d_tx_state = ST_IMAGE_OUT;
				break;
			}*/
			if(d_confirm_index==6)
			{
				d_confirm_sent = true;
				d_confirm_index = 0;
				for(int i = 0; i<d_buffer_size; i++)
				{
					if(d_packet_no_out[i]!=0x00)
					{
						d_packet_out_index = i;
						std::cout<<"Leave state of sending confirm\n";
						d_tx_state = ST_TX_NODE_ID_TRANS;
						break;
					}
				}
			}
			break;
		}
		/*
		case ST_IMAGE_OUT:
		{
			if(d_image_out_idx==0)
			{
				out[no] = d_end_nodeA_id;
				d_image_out_idx++;
				no++;
			}
			else
			{
				int imag_idx = d_pkt_out_no*d_packet_size + d_image_out_idx-1;
				out[no] = d_packet_nodeA[imag_idx];
				no++;
				d_image_out_idx++;
				if(d_image_out_idx==(d_packet_size+1))
				{
					d_pkt_out_no++;
					if(d_pkt_out_no==128)
					{
						d_tx_state = ST_TX_CONFIRM_TRANS;
					}
					d_image_out_idx = 0;
				}
			}
			break;
		}
		*/
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

