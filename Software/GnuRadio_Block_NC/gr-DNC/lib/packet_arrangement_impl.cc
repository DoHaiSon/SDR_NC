
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
#include "packet_arrangement_impl.h"

namespace gr {
  namespace DNC {

    packet_arrangement::sptr
    packet_arrangement::make(int buffer_size, int packet_size, unsigned char nodeA_id, unsigned char nodeB_id, int node)
    {
      return gnuradio::get_initial_sptr
        (new packet_arrangement_impl(buffer_size, packet_size, nodeA_id, nodeB_id, node));
    }

    /*
     * The private constructor
     */
    packet_arrangement_impl::packet_arrangement_impl(int buffer_size, int packet_size, unsigned char nodeA_id, unsigned char nodeB_id, int node)
      : gr::block("packet_arrangement",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_nodeA_id(nodeA_id), d_nodeB_id(nodeB_id),
	      d_rx_node_id(0x00), d_rx_packet_index(0), d_rx_packet_number(0x00), d_rx_data_index(0),
	      d_tx_packet_index(0), d_tx_data_index(0), d_break(false),
	      d_rx_state(ST_RX_IDLE), d_tx_state(ST_TX_CHECK_RECEIVED_PACKET)
    {
	if(node==0)
	{
		d_node = d_nodeA_id;
	}
	if(node==1)
	{
		d_node = d_nodeB_id;
	}
	d_packet_number.resize(buffer_size);
	std::fill(d_packet_number.begin(), d_packet_number.end(), 0x00);
	d_packet_data.resize(buffer_size*packet_size);
	std::fill(d_packet_data.begin(), d_packet_data.end(), 0x00);
    }

    /*
     * Our virtual destructor.
     */
    packet_arrangement_impl::~packet_arrangement_impl()
    {
    }

    void
    packet_arrangement_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    packet_arrangement_impl::general_work (int noutput_items,
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
	if(d_break==false)
	{
		while (ni<nInputItems)
		{
			switch(d_rx_state)
			{
				case ST_RX_IDLE:
				{
					if(in[ni]==d_nodeA_id)
					{
						d_rx_node_id = d_nodeA_id;
						d_rx_state = ST_RX_PACKET_NUMBER;
					}
					if(in[ni]==d_nodeB_id)
					{
						d_rx_node_id = d_nodeB_id;
						d_rx_state = ST_RX_PACKET_NUMBER;
					}
					ni++;
					break;
				}
				case ST_RX_PACKET_NUMBER:
				{
					if(d_rx_node_id==d_node)
					{
						d_rx_packet_index = (int) in[ni]-1;
						if(d_packet_number[d_rx_packet_index]!=0x00)
						{
							d_break = true;
							d_rx_state = ST_RX_IDLE;
							consume_each (ni-1);
							return no;
						}
						d_rx_packet_number = in[ni];
					}
					d_rx_state = ST_RX_PACKET_DATA;
					ni++;
					break;
				}
				case ST_RX_PACKET_DATA:
				{
					if(d_rx_node_id==d_node)
					{
						int data_idx = d_rx_packet_index*d_packet_size + d_rx_data_index;
						d_packet_data[data_idx] = in[ni];
					}
					d_rx_data_index++;
					ni++;
					if(d_rx_data_index==d_packet_size)
					{
						if(d_rx_node_id==d_node)
						{
							d_packet_number[d_rx_packet_index] = d_rx_packet_number;
						}
						d_rx_data_index = 0;
						d_rx_state = ST_RX_IDLE;
					}
					break;
				}
			}
		}
	}

	while (no<nOutputItems) 
	{
	    	switch(d_tx_state)
	    	{
			case ST_TX_CHECK_RECEIVED_PACKET:
			{
				if (d_packet_number[d_tx_packet_index]!=0x00)
				{
					d_tx_state = ST_TX_PACKET_DATA_OUT;
				}
				else
				{
					consume_each (ni);
					return no;
				}
				break;
			}
			case ST_TX_PACKET_DATA_OUT:
			{	
				int data_out_idx = d_tx_packet_index*d_packet_size + d_tx_data_index;
				out[no] = d_packet_data[data_out_idx];
				d_tx_data_index++;
				no++;
				if(d_tx_data_index==d_packet_size)
				{
					d_tx_packet_index++;
					if(d_tx_packet_index==d_buffer_size)
					{
						d_tx_packet_index = 0;
						d_break = false;
						for(int i = 0; i<d_buffer_size; i++)
						{
							d_packet_number[i] = 0x00;
						}
					}
					d_tx_data_index = 0;
					d_tx_state = ST_TX_CHECK_RECEIVED_PACKET;
				}
				break;
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

