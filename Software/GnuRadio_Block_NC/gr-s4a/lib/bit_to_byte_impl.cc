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
#include "bit_to_byte_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

bit_to_byte::sptr bit_to_byte::make()
{
	return gnuradio::get_initial_sptr(new bit_to_byte_impl());
}

/*
 * The private constructor
 */
bit_to_byte_impl::bit_to_byte_impl() :
		gr::block("bit_to_byte", gr::io_signature::make(1, 1, sizeof(char)),
				gr::io_signature::make(1, 1, sizeof(char))), d_byte(0x0), d_counter(
				0)
{
	printf("init::bit_to_byte\n");
	set_output_multiple(1);
}

/*
 * Our virtual destructor.
 */
bit_to_byte_impl::~bit_to_byte_impl()
{
}

void bit_to_byte_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	ninput_items_required[0] = 8 * noutput_items;
}

int bit_to_byte_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const char *in = (const char *) input_items[0];
	char *out = (char *) output_items[0];

	int ni = 0;
	int no = 0;
	int i = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		d_byte |= ((in[ni] & 0x1) << d_counter);
		d_counter++;
		ni++;

		if (d_counter % 8 == 0)
		{
			out[no] = d_byte;
			d_byte = 0x0;
			d_counter = 0;
			no++;
		}
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

