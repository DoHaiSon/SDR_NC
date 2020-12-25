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
#include "relay_control_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <math.h>
#include <time.h>

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace DNC {

    relay_control::sptr
    relay_control::make(int data_size, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new relay_control_impl(data_size, end_nodeA_id, end_nodeB_id, relay_node_id, timeout));
    }

    /*
     * The private constructor
     */
    relay_control_impl::relay_control_impl(int data_size, unsigned char end_nodeA_id, unsigned char end_nodeB_id, unsigned char relay_node_id, int timeout)
      : gr::block("relay_control",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_data_size(data_size), d_end_nodeA_id(end_nodeA_id), d_end_nodeB_id(end_nodeB_id), d_relay_node_id(relay_node_id), 
	      d_timeout(timeout), d_beacon_index(0), d_req_data_retrans_index(0), d_beacon_id(0x00),
	      d_data_nodeA_index(0), d_data_nodeB_index(0), d_xored_data_index(0), d_header_index(0), d_packet_number_nodeA_index(0),
	      d_packet_number_nodeB_index(0), d_recved_data_nodeA(false), d_recved_data_nodeB(false), d_reach_data_nodeA(false), 		      d_reach_data_nodeB(false), d_reach_packet_number_nodeA(false), d_reach_packet_number_nodeB(false), d_beacon_trans(false), 	      d_next_packet_number(0x01), d_trans_packet_number(0x01), d_packet_number_nodeA_count(0), d_packet_no(1),
	      d_packet_number_nodeB_count(0), d_xored_packet_number_index(0), d_state(ST_RECV_PACKET), d_state_trans(ST_DO_BEACON_TRANS)
    {
	d_clock = clock();
	d_clock_diff = float(0.0);
	d_data_nodeA.resize(d_data_size);
	std::fill(d_data_nodeA.begin(), d_data_nodeA.end(), 0x00);
	d_data_nodeB.resize(d_data_size);
	std::fill(d_data_nodeB.begin(), d_data_nodeB.end(), 0x00);
	d_xored_data.resize(d_data_size);
	std::fill(d_xored_data.begin(), d_xored_data.end(), 0x00);
    }

    /*
     * Our virtual destructor.
     */
    relay_control_impl::~relay_control_impl()
    {
    }

    void
    relay_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
/*
	ninput_items_required[0] = noutput_items;
	ninput_items_required[1] = noutput_items;
*/
    }

    int
    relay_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
	//if(nInputItems0!=0&&nInputItems1!=0&&noutput_items!=0)
	//std::cout<<nInputItems0<<"_"<<nInputItems1<<"_"<<noutput_items<<std::endl;
	int nInputItems = std::min(nInputItems0, nInputItems1);
	int bound = 2*d_data_size;
	int nOutputItems = noutput_items;
        const unsigned char *in0 = (const unsigned char *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];	
        unsigned char *out = (unsigned char *) output_items[0];

	int ni = 0;
	int no = 0;
	while(ni<bound && no<bound)
	{
		switch (d_state)
		{
		case ST_RECV_PACKET:
		{
			d_clock_diff = ((float)(clock() - d_clock) / CLOCKS_PER_MSEC);
			if (d_clock_diff > (float)d_timeout)
			{
				if(d_beacon_trans==false)
				{
					//std::cout<<"timeout: needs to trans beacon "<<std::endl;
					d_state = ST_HEADER_TRANS;
					d_state_trans = ST_DO_BEACON_TRANS;
				}
				else
				{
					//std::cout<<"retrans xored data"<<std::endl;
					d_state = ST_HEADER_TRANS;
					d_state_trans = ST_DO_XORED_DATA_TRANS;
				}
				break;
			}
			/* data node A */
			if(d_reach_data_nodeA==true)
			{
				if(d_data_nodeA_index<d_data_size)
				{
					d_data_nodeA[d_data_nodeA_index] = in0[ni];
					//printf("%#02X, ", (unsigned char) in0[ni]);
					d_data_nodeA_index++;
					if(d_data_nodeA_index==d_data_size)
					{
						d_recved_data_nodeA = true;
						//std::cout<<"\n";
					}
				}
			}
			/* data node B */
			if(d_reach_data_nodeB==true)
			{
				if(d_data_nodeB_index<d_data_size)
				{
					d_data_nodeB[d_data_nodeB_index] = in1[ni];
					//printf("%#02X, ", (unsigned char) in1[ni]);
					d_data_nodeB_index++;
					if(d_data_nodeB_index==d_data_size)
					{
						d_recved_data_nodeB = true;
						//std::cout<<"\n";
					}
				}
			}
			/* check if received data from both two end nodes */
			if(d_recved_data_nodeA==true&&d_recved_data_nodeB==true)
			{
				d_state = ST_XORED_DATA;
				std::cout<<"recv packets from two end nodes: "<<d_packet_no<<std::endl;
				d_reach_packet_number_nodeA = false;
				d_reach_packet_number_nodeB = false;
				d_reach_data_nodeA = false;
				d_reach_data_nodeB = false;
				d_recved_data_nodeA = false;
				d_recved_data_nodeB = false;
				d_beacon_trans = true;
				d_packet_number_nodeA_index = 0;
				d_packet_number_nodeA_count = 0;
				d_packet_number_nodeB_index = 0;
				d_packet_number_nodeB_count = 0;
				d_data_nodeA_index = 0;
				d_data_nodeB_index = 0;
				d_xored_data_index = 0;
				out[no] = 0x00;
				ni++;
				no++;
				break;
			}
			/* header node A */
			if(d_reach_packet_number_nodeA==true)
			{
				if(d_packet_number_nodeA_index<3)
				{
					//printf("%#02X, ", (unsigned char) in0[ni]);
					if(in0[ni]==d_next_packet_number)
					{
						d_packet_number_nodeA_count++;
					}
					d_packet_number_nodeA_index++;
					if(d_packet_number_nodeA_index==3)
					{
						if(d_packet_number_nodeA_count>=2)
						{
							d_reach_data_nodeA = true;
							//std::cout<<"get correct packet number from node A\n";
						}
						else
						{
							d_reach_packet_number_nodeA = false;
							//std::cout<<"\nd_reach_packet_number_nodeA = false;"<<std::endl;
							d_packet_number_nodeA_index = 0;
							d_packet_number_nodeA_count = 0;
						}
					}
				}
			}
			/* header node B */
			if(d_reach_packet_number_nodeB==true)
			{
				if(d_packet_number_nodeB_index<3)
				{
					//printf("%#02X, ", (unsigned char) in1[ni]);
					if(in1[ni]==d_next_packet_number)
					{
						d_packet_number_nodeB_count++;
					}
					d_packet_number_nodeB_index++;
					if(d_packet_number_nodeB_index==3)
					{
						if(d_packet_number_nodeB_count>=2)
						{
							d_reach_data_nodeB = true;
							//std::cout<<"get correct packet number from node B \n";
						}
						else
						{
							d_reach_packet_number_nodeB = false;
							//std::cout<<"\nd_reach_packet_number_nodeB = false;"<<std::endl;
							d_packet_number_nodeB_index = 0;
							d_packet_number_nodeB_count = 0;
						}
					}
				}
			}
			/* check received packet */
			if(in0[ni]==d_end_nodeA_id && d_reach_packet_number_nodeA == false)
			{
				//std::cout<<"get data from node A !!!!!!!"<<std::endl;
				d_reach_packet_number_nodeA = true;
				d_clock = clock();
			}
			if(in1[ni]==d_end_nodeB_id && d_reach_packet_number_nodeB == false)
			{
				//std::cout<<"get data from node B !!!!!!!"<<std::endl;
				d_reach_packet_number_nodeB = true;
				d_clock = clock();
			}
			out[no] = 0x00;
			ni++;
			no++;
			break;
		}
		case ST_BEACON_TRANS:
		{
			if(d_beacon_index<d_data_size)
			{
				out[no] = d_beacon_id;
				d_beacon_index++;
				no++;
				ni++;
			}
			else
			{
				//std::cout<<"trans beacon!\n";
				d_beacon_index = 0;
				d_state = ST_RECV_PACKET;
				d_clock = clock();
			}
			break;
		}
		case ST_XORED_DATA:
		{
			if(d_xored_data_index<d_data_size)
			{
				d_xored_data[d_xored_data_index] = d_data_nodeA[d_xored_data_index]^d_data_nodeB[d_xored_data_index];
				d_xored_data_index++;
			}
			else
			{
				std::cout<<"data node A: \n";
				for(int i = 0; i<d_data_size; i++)
				{
				printf("%#02X, ", (unsigned char) d_data_nodeA[i]);
				}
				std::cout<<"\n";
				std::cout<<"data node B: \n";
				for(int i = 0; i<d_data_size; i++)
				{
				printf("%#02X, ", (unsigned char) d_data_nodeB[i]);
				}
				std::cout<<"\n";
				std::cout<<"xored data packet number: "<<d_packet_no<<"\n";
				for(int i = 0; i<d_data_size; i++)
				{
				printf("%#02X, ", (unsigned char) d_xored_data[i]);
				}
				std::cout<<"\n";
				std::cout<<"------------------"<<std::endl;
				d_trans_packet_number = d_next_packet_number;
				if(d_packet_no==255)
				{
					d_next_packet_number = 0X01;
					d_packet_no = 1;
				}
				else
				{
					d_packet_no++;
					d_next_packet_number++;
				}
				d_state = ST_HEADER_TRANS;
				d_state_trans = ST_DO_XORED_DATA_TRANS;
				d_xored_data_index = 0;
			}
			break;
		}
		case ST_HEADER_TRANS:
		{
			if(d_header_index<3)
			{
				out[no] = d_relay_node_id;
				d_header_index++;
				no++;
			}
			else
			{
				if(d_state_trans == ST_DO_BEACON_TRANS)
					d_state = ST_BEACON_TRANS;
				if(d_state_trans == ST_DO_XORED_DATA_TRANS)
					d_state = ST_XORED_PACKET_NUMBER_TRANS;
				d_header_index = 0;
			}
			break;
		}
		case ST_XORED_PACKET_NUMBER_TRANS:
		{
			if(d_xored_packet_number_index<3)
			{
				out[no] = d_trans_packet_number;
				d_xored_packet_number_index++;
				no++;
			}
			else
			{
				d_xored_packet_number_index = 0;
				d_state = ST_XORED_DATA_TRANS;
			}
			break;
		}
		case ST_XORED_DATA_TRANS:
		{
			if(d_xored_data_index<d_data_size)
			{
				out[no] = d_xored_data[d_xored_data_index];
				d_xored_data_index++;
				no++;
			}
			else
			{
				d_state = ST_RECV_PACKET;
				d_xored_data_index = 0;
				d_clock = clock();
				//consume_each(ni);
				//return no;
			}
			break;
		}
		}
	}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each(ni);
	//std::cout<<"consume "<<ni<<std::endl;
	//std::cout<<"return "<<no<<std::endl;
        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace DNC */
} /* namespace gr */

