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
#include "qam4_demapper_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

qam4_demapper::sptr qam4_demapper::make()
{
	return gnuradio::get_initial_sptr(new qam4_demapper_impl());
}

/*
 * The private constructor
 */
qam4_demapper_impl::qam4_demapper_impl() :
		gr::block("qam4_demapper",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(char))), d_i(0), d_byte(0)
{
	printf("init::qam4_demapper\n");
	set_output_multiple(1);
	d_one = 0;
	d_zero = 0;
}

/*
 * Our virtual destructor.
 */
qam4_demapper_impl::~qam4_demapper_impl()
{
}

void qam4_demapper_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	ninput_items_required[0] = 4 * noutput_items;
}

int qam4_demapper_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	char *out = (char *) output_items[0];

	int ni = 0;
	int no = 0;

	float real = 0.0;
	float imag = 0.0;

	while (ni < ninput_items && no < noutput_items)
	{
		real = in[ni].real();
		imag = in[ni].imag();

		if (real > 0.0 && imag > 0.0)
		{
			d_byte |= (0x3 << d_i);
			d_zero++;
		}
		else if (real > 0.0 && imag < 0.0)
		{
			d_byte |= (0x2 << d_i);
			d_zero++;
		}
		else if (real < 0.0 && imag > 0.0)
		{
			d_byte |= (0x1 << d_i);
			d_one++;
		}
		else if (real < 0.0 && imag < 0.0)
		{
			d_byte |= (0x0 << d_i);
			d_zero++;
		}
		else
		{
			d_zero++;
			d_byte |= (0x0 << d_i);
		}

		d_i += 2;
		ni++;

		if (d_i % 8 == 0)
		{
			out[no] = d_byte;
			d_i = 0;
			d_byte = 0x0;
			no++;
		}
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

