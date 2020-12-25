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
#include "qam4_mapper_impl.h"
#include <vector>
#include <sys/time.h>
#include <cstdio>

namespace gr
{
namespace s4a
{

qam4_mapper::sptr qam4_mapper::make()
{
	return gnuradio::get_initial_sptr(new qam4_mapper_impl());
}

/*
 * The private constructor
 */
qam4_mapper_impl::qam4_mapper_impl() :
		gr::block("qam4_mapper", gr::io_signature::make(1, 1, sizeof(char)),
				gr::io_signature::make(1, 1, sizeof(gr_complex))), d_symbols(4,
				gr_complex(0, 0))
{
	printf("init::qam4_mapper\n");

	d_symbols[0] = gr_complex(-0.7, -0.7); // 00
	d_symbols[1] = gr_complex(-0.7, +0.7); // 01
	d_symbols[2] = gr_complex(+0.7, -0.7); // 10
	d_symbols[3] = gr_complex(+0.7, +0.7); // 11

	set_output_multiple(4);
}

/*
 * Our virtual destructor.
 */
qam4_mapper_impl::~qam4_mapper_impl()
{
}

void qam4_mapper_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int qam4_mapper_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	unsigned char *in = (unsigned char *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	int ni = 0;
	int no = 0;

	unsigned char byte = 0x0;

	while (ni < ninput_items && no < noutput_items)
	{
		byte = in[ni];

		out[no + 0] = d_symbols[((byte >> 0) & 0x3)];
		out[no + 1] = d_symbols[((byte >> 2) & 0x3)];
		out[no + 2] = d_symbols[((byte >> 4) & 0x3)];
		out[no + 3] = d_symbols[((byte >> 6) & 0x3)];

		ni++;
		no += 4;
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

