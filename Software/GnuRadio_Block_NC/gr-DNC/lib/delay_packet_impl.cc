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
#include "delay_packet_impl.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <time.h>

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace DNC {

    delay_packet::sptr
    delay_packet::make(int data_size, int max_delay)
    {
      return gnuradio::get_initial_sptr
        (new delay_packet_impl(data_size, max_delay));
    }

    /*
     * The private constructor
     */
    delay_packet_impl::delay_packet_impl(int data_size, int max_delay)
      : gr::block("delay_packet",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
	      d_data_size(data_size), d_max_delay(max_delay), d_data_index(0), d_delay_val(0), d_delay_index(0), d_header_index(0),
	      d_packet_number(0x01), d_packet_no(1), d_count(0), d_state(ST_CHECK)
    {
	d_clock = clock();
	d_clock_diff = d_clock;
	int mi_delay = d_data_size;	
	int ma_delay = mi_delay+d_max_delay;
	d_delay_val = rand() % ma_delay + mi_delay;
    }

    /*
     * Our virtual destructor.
     */
    delay_packet_impl::~delay_packet_impl()
    {
    }

    void
    delay_packet_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
	ninput_items_required[0] = noutput_items;
    }

    int
    delay_packet_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems = ninput_items[0];	
	int nOutputItems = noutput_items;
        const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];

	int ni = 0;
	int no = 0;
	while(ni<nInputItems && no<nOutputItems)
	{
		switch(d_state)
		{
		case ST_ADD_DELAY:
		{
			/*
			if(d_delay_index<d_delay_val)
			{
				out[no] = 0x00;
				d_delay_index++;
				no++;
			}
			else
			{
				int mi_delay = d_data_size;
				int ma_delay = mi_delay + d_max_delay;
				d_delay_val = rand() % ma_delay + mi_delay;
				d_delay_index = 0;
				d_state = ST_HEADER;
			}
			*/
			d_clock_diff = (clock() - d_clock) / CLOCKS_PER_MSEC;
			if (d_clock_diff > d_max_delay)
			{
				//std::cout<<"timeout"<<std::endl;
				d_state = ST_HEADER;
				d_clock = clock();
			}
			break;
		}
		case ST_HEADER:
		{
			if(d_header_index<3)
			{
				out[no] = in[ni];
				//printf("%#02X, ", (unsigned char) out[no]);
				no++;
				ni++;
				d_header_index++;
				break;
			}
			if(d_header_index>=3&&d_header_index<6)
			{
				out[no] = d_packet_number;
				//printf("%#02X, ", (unsigned char) out[no]);
				d_header_index++;
				no++;
				ni++;
			}
			if(d_header_index==6)
			{
				d_state = ST_DATA;
				d_header_index = 0;
				if( d_packet_no==255)
				{
					d_packet_number = 0x01;
					d_packet_no = 1;
				}
				else
				{
					d_packet_number++;
					d_packet_no++;
				}
			}
			break;
		}
		case ST_CHECK:
		{
			/*
			if(in[ni]==0x00)
			{
				d_count++;
			}
			ni++;
			if(d_count==10000)
			{
				d_count = 0;
				d_state = ST_SLEEP;
			}
			else
			{
				out[no] = 0x00;
				no++;
			}
			*/
			if(in[ni]==0x00)
			{
				d_state = ST_DATA;
			}
			ni++;
			break;
		}
		case ST_SLEEP:
		{
			out[no] = 0x01;
			d_count++;
			if(d_count==10000)
			{
				d_count = 0;
				d_state = ST_CHECK;
			}
			no++;
			ni++;
			break;
		}
		case ST_DATA:
		{
			if(d_data_index<d_data_size)
			{
				out[no] = in[ni];
				//printf("%#02X, ", (unsigned char) out[no]);
				no++;
				ni++;
				d_data_index++;
			}
			else
			{
				//std::cout<<"\n";
				//d_clock = clock();
				d_data_index = 0;
				d_state = ST_CHECK;
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

