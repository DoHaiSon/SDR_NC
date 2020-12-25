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
#include "end_node_control_impl.h"
#include <cstdio>

namespace gr {
  namespace DNC {

    end_node_control::sptr
    end_node_control::make(int data_size, unsigned char end_node_id, unsigned char relay_node_id)
    {
      return gnuradio::get_initial_sptr
        (new end_node_control_impl(data_size, end_node_id, relay_node_id));
    }

    /*
     * The private constructor
     */
    end_node_control_impl::end_node_control_impl(int data_size, unsigned char end_node_id, unsigned char relay_node_id)
      : gr::block("end_node_control",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
	      d_data_size(data_size), d_end_node_id(end_node_id), d_relay_node_id(relay_node_id), d_beacon_id(0x00),
	      d_check_index(0), d_beacon_count(0), d_check_packet_number_count(0), d_check_recv_beacon(false), 
	      d_origin_data_index(0), d_header_index(0), d_data_trans_index(0), d_xored_data_index(0), d_data_out_index(0), 
	      d_next_packet_number(0x01), d_trans_packet_number(0x01), d_check_xored_data_out(false), d_check_recv_xored_data(false),
	      d_packet_number_count(1), d_state(ST_IDLE)
    {
	d_origin_data.resize(d_data_size);
	std::fill(d_origin_data.begin(), d_origin_data.end(), 0x00);
	d_xored_data.resize(d_data_size);
	std::fill(d_xored_data.begin(), d_xored_data.end(), 0x00);
	d_data_out.resize(d_data_size);
	std::fill(d_data_out.begin(), d_data_out.end(), 0x00);
	d_temp_data.resize(6);
	std::fill(d_temp_data.begin(), d_temp_data.end(), 0x00);
    }

    /*
     * Our virtual destructor.
     */
    end_node_control_impl::~end_node_control_impl()
    {
    }

    void
    end_node_control_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
/*
	ninput_items_required[0] = noutput_items;
	ninput_items_required[1] = noutput_items;
*/
    }

    int
    end_node_control_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems0 = ninput_items[0];
        int nInputItems1 = ninput_items[1];
	int nOutputItems = noutput_items;
	//if(nInputItems0!=0&&nInputItems1!=0&&nOutputItems!=0)
	//std::cout<<nInputItems0<<"/ "<<nInputItems1<<"/ "<<nOutputItems<<"\n";
        const unsigned char *in0 = (const unsigned char *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];
        unsigned char *out0 = (unsigned char *) output_items[0];
	unsigned char *out1 = (unsigned char *) output_items[1];

	int ni0 = 0;
	int ni1 = 0;
	int no = 0;

	while(ni0<nInputItems0 && ni1<nInputItems1 && no<nOutputItems)
	{
		switch(d_state)
		{
		case ST_IDLE:
		{
			if(in0[ni0]==d_relay_node_id)
			{
				d_state = ST_CHECK_PACKET_TYPE;
			}
			out0[no] = 0x00;
			out1[no] = 0x00;
			no++;
			ni0++;
			break;
		}
		case ST_CHECK_PACKET_TYPE:
		{
			d_check_index++;
			if(in0[ni0]==d_beacon_id&&d_check_recv_xored_data==false)
			{
				d_beacon_count++;
			}
			if(in0[ni0]==d_next_packet_number)
			{
				d_check_packet_number_count++;
			}
			if(d_check_index==3)
			{
				if(d_check_packet_number_count>=2)
				{
					d_check_recv_xored_data = true;
					d_state = ST_STORE_RECV_DATA;
				}
				else
				{
					d_state = ST_HEADER_TRANS;
				}
				d_check_packet_number_count = 0;
				if(d_check_recv_xored_data==false)
				{
					if(d_beacon_count>=2)
					{
						if(d_check_recv_beacon==false)
						{
							d_state = ST_GET_ORIGIN_DATA;
							d_check_recv_beacon = true;
						}
						else
						{
							d_state = ST_HEADER_TRANS;
						}
					}
					else
					{
						d_state = ST_IDLE;
					}
					d_beacon_count = 0;
				}
				d_check_index = 0;
			}
			out0[no] = 0x00;
			out1[no] = 0x00;
			no++;
			ni0++;
			break;
		}
		case ST_STORE_RECV_DATA:
		{
			if(d_xored_data_index<d_data_size)
			{
				d_xored_data[d_xored_data_index] = in0[ni0];
				d_xored_data_index++;
				ni0++;
			}
			else
			{
				d_xored_data_index = 0;
				//std::cout<<"node ";
				//printf("%#02X receives packet number: ", (unsigned char) d_end_node_id);
				//std::cout<<d_packet_number_count<<"\n";
				d_state = ST_XOR_OPERATION;
			}
			break;
		}
		case ST_XOR_OPERATION:
		{
			if(d_data_out_index<d_data_size)
			{
				d_data_out[d_data_out_index] = d_xored_data[d_data_out_index]^d_origin_data[d_data_out_index];
				printf("%i ", (int) d_data_out[d_data_out_index]);
				d_data_out_index++;
			}
			else
			{
				std::cout<<std::endl;
				d_data_out_index = 0;
				d_check_xored_data_out = true;
				if(d_packet_number_count==255)
				{
					d_next_packet_number = 0x01;
					d_packet_number_count = 1;
				}
				else
				{
					d_next_packet_number++;
					d_packet_number_count++;
				}
				d_trans_packet_number = d_next_packet_number;
				d_state = ST_GET_ORIGIN_DATA;
			}
			break;
		}
		case ST_GET_ORIGIN_DATA:
		{
			if(d_origin_data_index==0)
			{
				ni1++;
				d_origin_data_index++;
			}
			else
			{
				if(d_origin_data_index<=d_data_size)
				{
					d_origin_data[d_origin_data_index-1] = in1[ni1];
					d_origin_data_index++;
					ni1++;
				}
				else
				{
					d_origin_data_index = 0;
					d_state = ST_HEADER_TRANS;
				}
			}
			break;
		}
		case ST_HEADER_TRANS:
		{
			if(d_header_index<3)
			{
				out0[no] = d_end_node_id;
				out1[no] = 0x00;
				no++;
				d_header_index++;
			}
			else
			{
				d_header_index = 0;
				d_state = ST_PACKET_NUMBER_TRANS;
			}
			break;
		}
		case ST_PACKET_NUMBER_TRANS:
		{
			if(d_header_index<3)
			{
				out0[no] = d_trans_packet_number;
				d_header_index++;
				if(d_check_xored_data_out==true&&d_header_index==3)
				{
					out1[no] = d_end_node_id;
				}
				else
				{
					out1[no] = 0x00;
				}
				no++;
			}
			else
			{
				d_header_index = 0;
				d_state = ST_DATA_TRANS;
			}
			break;
		}
		case ST_DATA_TRANS:
		{
			if(d_data_trans_index<d_data_size)
			{
				out0[no] = d_origin_data[d_data_trans_index];
				if(d_check_xored_data_out==true)
				{
					out1[no] = d_data_out[d_data_trans_index];
				}
				else
				{
					out1[no] = 0x00;
				}
				no++;
				d_data_trans_index++;
			}
			else
			{
				d_check_xored_data_out = false;
				d_data_trans_index = 0;
				d_state = ST_IDLE;
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

