/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "zero_padding_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

zero_padding::sptr zero_padding::make(int input_stream_length,
		int output_stream_length)
{
	return gnuradio::get_initial_sptr(
			new zero_padding_impl(input_stream_length, output_stream_length));
}

/*
 * The private constructor
 */
zero_padding_impl::zero_padding_impl(int input_stream_length,
		int output_stream_length) :
		gr::block("zero_padding",
				gr::io_signature::make2(1, 1, sizeof(gr_complex),
						sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
	printf("init::zero_padding\n");

	d_zero_padding_sz = output_stream_length - input_stream_length;

	if (d_zero_padding_sz < 0)
	{
		throw std::runtime_error(
				"init::zero_padding:: d_zero_padding_sz must be greater than 0");
	}
	printf("init::zero_padding::d_zero_padding_sz: %d complex syms\n",
			d_zero_padding_sz);

	set_output_multiple(output_stream_length);
}

/*
 * Our virtual destructor.
 */
zero_padding_impl::~zero_padding_impl()
{
}

void zero_padding_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_output_stream_length))
			/ d_output_stream_length;
	ninput_items_required[0] = d_input_stream_length * coeff;
}

int zero_padding_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	int i = 0;

	int ni = 0;
	int no = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		switch (d_state)
		{
		case ST_SYMS:
			if (d_syms_idx < d_input_stream_length)
			{
				out[no] = in[ni];
				ni++;
				no++;
				d_syms_idx++;

				if (d_syms_idx >= d_input_stream_length)
				{
					d_syms_idx = 0;
					d_state = ST_ZEROS;
				}
				else
				{
					d_state = ST_SYMS;
					break;
				}
			}
		case ST_ZEROS:
			for (i = d_zero_idx; i < d_zero_padding_sz; i++)
			{
				if (no < noutput_items)
				{
					out[no] = gr_complex(0, 0);
					no++;
					d_zero_idx++;
				}
				else
				{
					// If output buffer hasn't enough capacity
					// We cancel the loop and check it on next scheduler
					// turn
					i = d_zero_padding_sz;
					d_state = ST_ZEROS;
					break;
				}
			}
			d_zero_idx = 0;
			d_state = ST_SYMS;
			break;
		default:
			throw std::invalid_argument("zero padding::invalid state");
			break;

		}
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

