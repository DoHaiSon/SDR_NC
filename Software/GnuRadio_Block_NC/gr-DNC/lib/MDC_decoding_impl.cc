
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
#include "MDC_decoding_impl.h"
#include <algorithm>

namespace gr {
  namespace DNC {

    MDC_decoding::sptr
    MDC_decoding::make(int image_width, int image_height, int no_descriptions)
    {
      return gnuradio::get_initial_sptr
        (new MDC_decoding_impl(image_width, image_height, no_descriptions));
    }

    /*
     * The private constructor
     */
    MDC_decoding_impl::MDC_decoding_impl(int image_width, int image_height, int no_descriptions)
      : gr::block("MDC_decoding",
              gr::io_signature::make2(2, 2, sizeof(unsigned char), sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
	      d_image_width(image_width), d_image_height(image_height), d_no_descriptions(no_descriptions),
	      d_rx0_index(0), d_rx1_index(0), d_tx_index(0), d_rx0_pointer(0), d_rx1_pointer(1), d_tx_pointer(0), 
	      d_check_end(0), d_delay(0)
    {
	if(no_descriptions==1)
	{
		d_state_tx = ST1_CHECK_DATA_OUT;
	}
	else
	{
		d_state_tx = ST2_CHECK_DATA_OUT;
	}
	d_data_in.resize(image_width*image_height);
	std::fill(d_data_in.begin(), d_data_in.end(), 0x00);

	d_data_out.resize(image_width*image_height);
	std::fill(d_data_out.begin(), d_data_out.end(), 0x00);

	d_check_rows.resize(image_height);
	std::fill(d_check_rows.begin(), d_check_rows.end(), 0);
    }

    /*
     * Our virtual destructor.
     */
    MDC_decoding_impl::~MDC_decoding_impl()
    {
    }

    void
    MDC_decoding_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    MDC_decoding_impl::general_work (int noutput_items,
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

	while(ni0<nInputItems0)
	{
		int rx0_index = d_rx0_pointer*d_image_width + d_rx0_index;
		d_data_in[rx0_index] = in0[ni0];
		d_rx0_index++;
		ni0++;
		if(d_rx0_index==d_image_width)
		{
			d_rx0_index = 0;
			d_check_rows[d_rx0_pointer] = 1;
			d_rx0_pointer = d_rx0_pointer + 2;
		}
	}

	while(ni1<nInputItems1)
	{
		int rx1_index = d_rx1_pointer*d_image_width + d_rx1_index;
		d_data_in[rx1_index] = in1[ni1];
		d_rx1_index++;
		ni1++;
		if(d_rx1_index==d_image_width)
		{
			d_rx1_index = 0;
			d_check_rows[d_rx1_pointer] = 1;
			d_rx1_pointer = d_rx1_pointer + 2;
		}
	}

	while(no<nOutputItems)
	{
		if(d_no_descriptions==1)
		{
			switch(d_state_tx)
			{
				case ST1_CHECK_DATA_OUT:
				{
					if(d_check_rows[d_tx_pointer]==1)
					{
						d_state_tx = ST1_DECODING;
					}
					else
					{
						consume (0, ni0);
						consume (1, ni1);
						return no;
					}
					break;
				}
				case ST1_DECODING:
				{
					if(d_tx_index<d_image_width)
					{
						int in_index = d_tx_pointer*d_image_width+d_tx_index;
						int out_index = 2*d_tx_pointer*d_image_width+d_tx_index;
						d_data_out[out_index] = d_data_in[in_index];
						d_tx_index++;
						if(d_tx_index==d_image_width)
						{
							if(d_tx_pointer<((d_image_height/2)-1))
							{
								d_tx_pointer++;
								d_state_tx = ST1_CHECK_DATA_OUT;
							}
						}
						break;
					}
					if(d_tx_index>=d_image_width)
					{
						if(d_tx_pointer==(d_image_height/2))
						{
							int in_index = (d_tx_pointer-1)*d_image_width+d_tx_index;
							int out_index = 2*d_tx_pointer*d_image_width+d_tx_index;
							d_data_out[out_index] = d_data_in[in_index];
							d_tx_index++;
						}
						else
						{
							int pxl0 = 0;
							int pxl1 = 0;
							int pxl_rectr = 0;
							int tx_index = (d_tx_pointer-1)*d_image_width+d_tx_index;
							pxl0 = (int)d_data_in[tx_index-d_image_width];
							pxl1 = (int)d_data_in[tx_index];
							pxl_rectr = (pxl0+pxl1)/2;
							tx_index = 2*(d_tx_pointer-1)*d_image_width+d_tx_index;
							d_data_out[tx_index] = (unsigned char) pxl_rectr;
							d_tx_index++;
						}
						if(d_tx_index==(2*d_image_width))
						{
							d_tx_index = 0;
							d_state_tx = ST1_DATA_OUT;
						}
					}
					break;
				}
				case ST1_DATA_OUT:
				{
					int tx_index = 2*(d_tx_pointer-1)*d_image_width+d_tx_index;
					out[no] = d_data_out[tx_index];
					d_tx_index++;
					no++;
					if(d_tx_index==(2*d_image_width))
					{
						d_tx_index = 0;
						d_state_tx = ST1_DELAY;
					}
					break;
				}
				case ST1_FREE:
				{
					consume (0, ni0);
					consume (1, ni1);
					return no;
				}
				case ST1_DELAY:
				{
					d_delay++;
					if(d_delay==5000)
					{
						d_delay = 0;
						d_state_tx = ST1_DECODING;
						if(d_tx_pointer==((d_image_height/2)-1))
						{
							d_check_end++;
							if(d_check_end==2)
							{
								d_state_tx = ST1_FREE;
							}
						}
					}
					else
					{
						consume (0, ni0);
						consume (1, ni1);
						return no;
					}
				}
			}
		}
		if(d_no_descriptions==2)
		{
			switch(d_state_tx)
			{
				case ST2_CHECK_DATA_OUT:
				{
					if(d_check_rows[d_tx_pointer]==1)
					{
						d_state_tx = ST2_DATA_OUT;
					}
					else
					{
						consume (0, ni0);
						consume (1, ni1);
						return no;
					}
					break;
				}
				case ST2_DATA_OUT:
				{
					out[no] = d_data_in[d_tx_pointer*d_image_width+d_tx_index];
					d_tx_index++;
					no++;
					if(d_tx_index==d_image_width)
					{
						d_tx_index = 0;
						d_state_tx = ST2_DELAY;
					}
				}
				case ST2_FREE:
				{
					consume (0, ni0);
					consume (1, ni1);
					return no;
				}
				case ST2_DELAY:
				{
					d_delay++;
					if(d_delay==5000)
					{
						d_delay = 0;
						if(d_tx_pointer==(d_image_height-1))
						{
							d_state_tx = ST2_FREE;
						}
						else
						{
							d_state_tx = ST2_CHECK_DATA_OUT;
						}
						if(d_tx_pointer<(d_image_height/2))
						{
							d_tx_pointer = d_tx_pointer + (d_image_height/2);
						}
						else
						{
							d_tx_pointer = d_tx_pointer - (d_image_height/2) + 1;
						}
					}
					else
					{
						consume (0, ni0);
						consume (1, ni1);
						return no;
					}
				}
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

