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
#include "LC_decoding_impl.h"

namespace gr {
  namespace DNC {

    LC_decoding::sptr
    LC_decoding::make(int image_width, int image_height)
    {
      return gnuradio::get_initial_sptr
        (new LC_decoding_impl(image_width, image_height));
    }

    /*
     * The private constructor
     */
    LC_decoding_impl::LC_decoding_impl(int image_width, int image_height)
      : gr::block("LC_decoding",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_image_width(image_width), d_image_height(image_height),
	      d_rx0_pointer(0), d_rx1_pointer(0), d_tx_pointer(0),
	      d_rx0_index(0), d_rx1_index(0), d_tx_index(0), d_delay(0)
    {
	d_based_data.resize(image_width*image_height);
	std::fill(d_based_data.begin(), d_based_data.end(), 0.0);

	d_enhanced_data.resize(image_width*image_height);
	std::fill(d_enhanced_data.begin(), d_enhanced_data.end(), 0.0);

	d_original_data.resize(image_width*image_height);
	std::fill(d_original_data.begin(), d_original_data.end(), 0.0);

	d_check_rows_based.resize(image_height);
	std::fill(d_check_rows_based.begin(), d_check_rows_based.end(), 0);

	d_check_rows_enhanced.resize(image_height);
	std::fill(d_check_rows_enhanced.begin(), d_check_rows_enhanced.end(), 0);

	d_check_rows_origin.resize(image_height);
	std::fill(d_check_rows_origin.begin(), d_check_rows_origin.end(), 0);

    }

    /*
     * Our virtual destructor.
     */
    LC_decoding_impl::~LC_decoding_impl()
    {
    }

    void
    LC_decoding_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    LC_decoding_impl::general_work (int noutput_items,
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
	while (ni0<nInputItems0)
	{
		int idx = d_rx0_pointer*d_image_width + d_rx0_index;
		d_based_data[idx] = in0[ni0];
		d_rx0_index++;
		ni0++;
		if(d_rx0_index==d_image_width)
		{
			d_rx0_index = 0;
			d_check_rows_based[d_rx0_pointer] = 1;
			d_rx0_pointer = d_rx0_pointer + 1;
		}
	}
	while (ni1<nInputItems1)
	{
		int idx = d_rx1_pointer*d_image_width + d_rx1_index;
		d_enhanced_data[idx] = in1[ni1];
		d_rx1_index++;
		ni1++;
		if(d_rx1_index==d_image_width)
		{
			d_rx1_index = 0;
			d_check_rows_enhanced[d_rx1_pointer] = 1;
			d_rx1_pointer = d_rx1_pointer + 1;
		}
	}
	while (no<nOutputItems)
	{
		if(d_check_rows_origin[d_tx_pointer]==0 && d_tx_pointer<d_image_width &&
		   d_check_rows_based[d_tx_pointer]==1 && d_check_rows_enhanced[d_tx_pointer]==1)
		{
			for(int i = 0; i<d_image_width; i++)
			{
				int temp_idx = d_tx_pointer*d_image_width+i;
				d_original_data[temp_idx] = d_based_data[temp_idx]+d_enhanced_data[temp_idx];
			}
			d_check_rows_origin[d_tx_pointer] = 1;
		}
		if(d_check_rows_origin[d_tx_pointer]==1&&d_tx_pointer<d_image_width)
		{
			if(d_delay<5000)
			{
				d_delay++;
				consume (0, ni0);
				consume (1, ni1);
				return no;
			}
			out[no] = d_original_data[d_tx_pointer*d_image_width+d_tx_index];
			d_tx_index++;
			no++;
			if(d_tx_index==d_image_width)
			{
				d_tx_index = 0;
				d_delay = 0;
				d_tx_pointer++;
			}
		}
		else
		{
			consume (0, ni0);
			consume (1, ni1);
			return no;
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

