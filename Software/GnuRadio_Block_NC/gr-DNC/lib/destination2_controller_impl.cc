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
#include "destination2_controller_impl.h"

namespace gr {
  namespace DNC {

    destination2_controller::sptr
    destination2_controller::make(int buffer_size, int packet_size, int guard_interval, unsigned char source1_id, unsigned char source2_id, unsigned char relay1_id, unsigned char relay2_id, unsigned char destination1_id, unsigned char destination2_id, int network_coding)
    {
      return gnuradio::get_initial_sptr
        (new destination2_controller_impl(buffer_size, packet_size, guard_interval, source1_id, source2_id, relay1_id, relay2_id, destination1_id, destination2_id, network_coding));
    }

    /*
     * The private constructor
     */
    destination2_controller_impl::destination2_controller_impl(int buffer_size, int packet_size, int guard_interval, unsigned char source1_id, unsigned char source2_id, unsigned char relay1_id, unsigned char relay2_id, unsigned char destination1_id, unsigned char destination2_id, int network_coding)
      : gr::block("destination2_controller",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char))),
	      d_buffer_size(buffer_size), d_packet_size(packet_size), d_guard_interval(guard_interval), d_source1_id(source1_id),
	      d_source2_id(source2_id), d_relay1_id(relay1_id), d_relay2_id(relay2_id), d_destination1_id(destination1_id),
	      d_destination2_id(destination2_id), d_network_coding(network_coding)
    {}

    /*
     * Our virtual destructor.
     */
    destination2_controller_impl::~destination2_controller_impl()
    {
    }

    void
    destination2_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    destination2_controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int nInputItems = ninput_items[0];
	int nOutputItems = noutput_items;
	
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out0 = (unsigned char *) output_items[0];
	unsigned char *out1 = (unsigned char *) output_items[1];

	int ni = 0;
	int no = 0;



        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (ni);

        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace DNC */
} /* namespace gr */

