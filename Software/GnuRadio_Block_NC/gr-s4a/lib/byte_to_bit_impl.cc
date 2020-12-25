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
#include "byte_to_bit_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

byte_to_bit::sptr byte_to_bit::make()
{
	return gnuradio::get_initial_sptr(new byte_to_bit_impl());
}

/*
 * The private constructor
 */
byte_to_bit_impl::byte_to_bit_impl() :
		gr::block("byte_to_bit", gr::io_signature::make(1, 1, sizeof(char)),
				gr::io_signature::make(1, 1, sizeof(char)))
{
	printf("init::byte_to_bit\n");
	set_output_multiple(8);
}

/*
 * Our virtual destructor.
 */
byte_to_bit_impl::~byte_to_bit_impl()
{
}

void byte_to_bit_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % 8)) / 8;
	ninput_items_required[0] = coeff;
}

int byte_to_bit_impl::general_work(int noutput_items,
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
		for (i = 0; i < 8; i++)
		{
			out[no] = ((in[ni] >> i) & 0x1);
			no++;
		}
		ni++;
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

