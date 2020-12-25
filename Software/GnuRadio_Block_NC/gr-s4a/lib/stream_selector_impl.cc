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
#include "stream_selector_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

stream_selector::sptr stream_selector::make(int magnitude_min,
		int magnitude_max, int input_stream_length, int output_stream_length)
{
	return gnuradio::get_initial_sptr(
			new stream_selector_impl(magnitude_min, magnitude_max,
					input_stream_length, output_stream_length));
}

/*
 * The private constructor
 */
stream_selector_impl::stream_selector_impl(int magnitude_min, int magnitude_max,
		int input_stream_length, int output_stream_length) :
		gr::block("stream_selector",
				gr::io_signature::make2(1, 1, sizeof(gr_complex),
						sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex))), d_magnitude_min(
				magnitude_min), d_magnitude_max(magnitude_max), d_input_stream_length(
				input_stream_length), d_output_stream_length(
				output_stream_length), d_state(ST_SYMS), d_syms_idx(0), d_zero_idx(
				0)
{
	printf("init::stream_selector\n");

	d_stream_selector_sz = output_stream_length - input_stream_length;

	if (d_stream_selector_sz < 0)
	{
		throw std::runtime_error(
				"init::stream_selector:: d_stream_selector_sz must be greater than 0");
	}
	printf("init::stream_selector::d_stream_selector_sz: %d complex syms\n",
			d_stream_selector_sz);

	set_output_multiple(output_stream_length);
}

/*
 * Our virtual destructor.
 */
stream_selector_impl::~stream_selector_impl()
{
}

void stream_selector_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_output_stream_length))
			/ d_output_stream_length;
	ninput_items_required[0] = d_input_stream_length * coeff;
}

int stream_selector_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	int i = 0;

	int ni = 0;
	int no = 0;

	bool isOK;
	float sym;

	while (ni < ninput_items && no < noutput_items)
	{
		for (i = 0; i < d_input_stream_length; i++)
		{
			sym = abs(in[ni + i]);
			if (sym >= d_magnitude_min && sym <= d_magnitude_max)
			{
				isOK = true;
				i = d_input_stream_length;
			}
		}

		if (isOK == true)
		{
			for (i = 0; i < d_input_stream_length; i++)
			{
				out[no + i] = in[ni + i];
				isOK = false;
			}
			no += d_input_stream_length;
			ni += d_input_stream_length;

			for (i = 0; i < d_stream_selector_sz; i++)
			{
				out[no + i] = gr_complex(0, 0);
			}
			no += d_stream_selector_sz;
		}
		else
		{
			ni += d_input_stream_length;
			isOK = false;
		}
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

