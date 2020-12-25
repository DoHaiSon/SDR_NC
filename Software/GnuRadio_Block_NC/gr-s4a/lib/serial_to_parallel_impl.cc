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
#include "serial_to_parallel_impl.h"
#include <cstdio>
#include <string.h>

namespace gr
{
namespace s4a
{

serial_to_parallel::sptr serial_to_parallel::make(int parallel_size)
{
	return gnuradio::get_initial_sptr(
			new serial_to_parallel_impl(parallel_size));
}

/*
 * The private constructor
 */
serial_to_parallel_impl::serial_to_parallel_impl(int parallel_size) :
		gr::block("serial_to_parallel",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(1, 1,
						sizeof(gr_complex) * parallel_size)), d_parallel_size(
				parallel_size)
{
	printf("init::serial_to_parallel\n");

	set_output_multiple(1);
}

/*
 * Our virtual destructor.
 */
serial_to_parallel_impl::~serial_to_parallel_impl()
{
}

void serial_to_parallel_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = d_parallel_size * noutput_items;
}

int serial_to_parallel_impl::general_work(int noutput_items,
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
		memcpy(&out[ni], &in[ni], d_parallel_size * sizeof(gr_complex));

		ni += d_parallel_size;
		no++;
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

