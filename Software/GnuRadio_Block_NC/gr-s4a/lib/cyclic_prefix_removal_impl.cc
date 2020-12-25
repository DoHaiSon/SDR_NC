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
#include "cyclic_prefix_removal_impl.h"

namespace gr
{
namespace s4a
{

cyclic_prefix_removal::sptr cyclic_prefix_removal::make(int fft_length,
		int cp_length)
{
	return gnuradio::get_initial_sptr(
			new cyclic_prefix_removal_impl(fft_length, cp_length));
}

/*
 * The private constructor
 */
cyclic_prefix_removal_impl::cyclic_prefix_removal_impl(int fft_length,
		int cp_length) :
		gr::block("cyclic_prefix_removal",
				gr::io_signature::make(1, 1,
						(fft_length + cp_length) * sizeof(gr_complex)),
				gr::io_signature::make(1, 1, fft_length * sizeof(gr_complex))), d_fft_length(
				fft_length), d_cp_length(cp_length)
{
	d_in_length = fft_length + cp_length;
}

/*
 * Our virtual destructor.
 */
cyclic_prefix_removal_impl::~cyclic_prefix_removal_impl()
{
}

void cyclic_prefix_removal_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int cyclic_prefix_removal_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];

	gr_complex *in = (gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	int ni = 0, no = 0;
	int nodx = 0, nidx = 0;
	while (ni < ninput_items && no < noutput_items)
	{
		memcpy(&out[nodx], &in[nidx + d_cp_length],
				d_fft_length * sizeof(gr_complex));

		nidx += d_in_length;
		nodx += d_fft_length;
		ni++;
		no++;
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

