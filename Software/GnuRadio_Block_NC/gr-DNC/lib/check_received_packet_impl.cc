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
#include "check_received_packet_impl.h"
#include <cstdio>
#include <time.h> 
#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)
namespace gr {
  namespace DNC {

    check_received_packet::sptr
    check_received_packet::make(int data_size, char check_node_id)
    {
      return gnuradio::get_initial_sptr
        (new check_received_packet_impl(data_size, check_node_id));
    }

    /*
     * The private constructor
     */
    check_received_packet_impl::check_received_packet_impl(int data_size, char check_node_id)
      : gr::block("check_received_packet",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
	      d_out_size(data_size+3), d_check_node_id(check_node_id),
	      d_data_index(0), d_header_count(0), d_node_id_count(0),
	      d_state(ST_WAIT_HEADER)
    {
	t0 = clock();
	t1 = t0;
    }

    /*
     * Our virtual destructor.
     */
    check_received_packet_impl::~check_received_packet_impl()
    {
    }

    void
    check_received_packet_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
	ninput_items_required[0] = noutput_items;
    }

    int
    check_received_packet_impl::general_work (int noutput_items,
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
	while (ni<nInputItems && no<nOutputItems)
	{
		switch(d_state)
		{
		case ST_WAIT_HEADER:
		{
			t1 = clock() - t0;
			//std::cout<<t1<<std::endl;
			float diff_t = ((float)t1)/CLOCKS_PER_MSEC;
			if(diff_t>(float)10)
			{
				//std::cout<<"It took me "<<t1<<" clicks ("<<diff_t<<" mili-seconds)\n";
				t0 = clock();
			}
			if(in[ni]==d_check_node_id)
			{
				d_header_count++;
				d_node_id_count++;
				d_state = ST_CHECK_HEADER;
			}
			out[no] = 0x00;
			no++;
			ni++;
			break;
		}
		case ST_CHECK_HEADER:
		{
			if(in[ni]==d_check_node_id)
			{
				d_node_id_count++;
			}
			d_header_count++;
			if(d_header_count==3)
			{
				if(d_node_id_count==3)
				{
					//std::cout<<"detect node ID ";
					//printf("%#02X \n", (unsigned char) d_check_node_id);
					out[no] = d_check_node_id;
					d_state = ST_OUT;
				}
				else
				{
					
					out[no] = 0x00;
					d_state = ST_WAIT_HEADER;
				}
				d_header_count = 0;
				d_node_id_count = 0;
				no++;
				ni++;
				break;
			}
			out[no] = 0x00;
			no++;
			ni++;
			break;
		}
		case ST_OUT:
		{
			if(d_data_index<d_out_size)
			{
				out[no] = in[ni];
				d_data_index++;
				ni++;
				no++;
			}
			else
			{
				d_state = ST_WAIT_HEADER;
				d_data_index = 0;
			}
			break;
		}
		}
	}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (ni);
	//std::cout<<"consume "<<ni<<std::endl;
	//std::cout<<"return "<<no<<std::endl;
        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace DNC */
} /* namespace gr */

