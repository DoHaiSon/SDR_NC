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
#include "console_sink_bb_impl.h"
#include <cstdio>
#include <iostream>

namespace gr
{
namespace s4a
{

console_sink_bb::sptr console_sink_bb::make(std::string sequence_name,
		int sequence_length, int print_rate)
{
	return gnuradio::get_initial_sptr(
			new console_sink_bb_impl(sequence_name, sequence_length,
					print_rate));
}

/*
 * The private constructor
 */
console_sink_bb_impl::console_sink_bb_impl(std::string sequence_name,
		int sequence_length, int print_rate) :
		gr::block("console_sink_bb",
				gr::io_signature::make(1, 1, sizeof(unsigned char)),
				gr::io_signature::make(0, 0, 0)), d_sequence_name(
				sequence_name), d_sequence_idx(0), d_sequence_length(
				sequence_length), d_print_rate_idx(0), d_print_rate(print_rate), d_state(
				ST_STORE)
{
	printf("init::console_sink_bb\n");
}

/*
 * Our virtual destructor.
 */
console_sink_bb_impl::~console_sink_bb_impl()
{
}

void console_sink_bb_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int console_sink_bb_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const unsigned char *in = (const unsigned char *) input_items[0];
	char *out = (char *) output_items[0];

	int i = 0;
	int ni = 0;

	while ((ni + d_sequence_length) <= ninput_items)
	{
		if (d_print_rate_idx % d_print_rate == 0)
		{
			std::cout << d_sequence_name << "=["; 
			for (i = 0; i < d_sequence_length; i++)
			{
				printf("%i ", (int) in[ni + i]);
			}
			printf("]\n");
			d_print_rate_idx = 0;
		}
		d_print_rate_idx++;
		ni += d_sequence_length;
	}
	consume_each(ni);
	return 0;
}

} /* namespace s4a */
} /* namespace gr */

