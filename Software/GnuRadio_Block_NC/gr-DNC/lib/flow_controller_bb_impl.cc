
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
#include "flow_controller_bb_impl.h"

namespace gr {
  namespace DNC {

    flow_controller_bb::sptr
    flow_controller_bb::make(unsigned char key, int control_mode)
    {
      return gnuradio::get_initial_sptr
        (new flow_controller_bb_impl(key, control_mode));
    }

    /*
     * The private constructor
     */
    flow_controller_bb_impl::flow_controller_bb_impl(unsigned char key, int control_mode)
      : gr::block("flow_controller_bb",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_key(key), d_control_mode(control_mode), d_rcv_key(0x01), d_check_key(false)
    {}

    /*
     * Our virtual destructor.
     */
    flow_controller_bb_impl::~flow_controller_bb_impl()
    {
    }

    void
    flow_controller_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    flow_controller_bb_impl::general_work (int noutput_items,
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
	if(d_control_mode==1) // Equivalent Mode
	{
		if(ni1<nInputItems1)
		{
			if(in1[ni1]==d_key)
			{
				d_check_key = true;
			}
			else 
			{
				d_check_key = false;
			}
			ni1 = nInputItems1;
		}
		if(d_check_key==true)
		{
			while(ni0<nInputItems0&&no<nOutputItems)
			{
				out[no] = in0[ni0];
				ni0++;
				no++;
			}
		}
		else
		{
			ni0 = nInputItems0;
		}
	}
	else // Inequivalent Mode
	{
		if(ni1<nInputItems1)
		{
			if(in1[ni1]==d_key)
			{
				d_check_key = true;
			}
			else 
			{
				d_check_key = false;
			}
			ni1 = nInputItems1;
		}
		if(d_check_key!=true)
		{
			while(ni0<nInputItems0&&no<nOutputItems)
			{
				out[no] = in0[ni0];
				ni0++;
				no++;
			}
		}
		else
		{
			ni0 = nInputItems0;
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

