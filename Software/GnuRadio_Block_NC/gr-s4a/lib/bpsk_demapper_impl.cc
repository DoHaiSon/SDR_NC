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
#include "bpsk_demapper_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

bpsk_demapper::sptr bpsk_demapper::make()
{
	return gnuradio::get_initial_sptr(new bpsk_demapper_impl());
}

/*
 * The private constructor
 */
bpsk_demapper_impl::bpsk_demapper_impl() :
		gr::block("bpsk_demapper",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(char))), d_i(0), d_byte(0)
{
	printf("init::bpsk_demapper\n");
	set_output_multiple(1);

	//TODO remove me
	d_one = 0;
	d_zero = 0;
}

/*
 * Our virtual destructor.
 */
bpsk_demapper_impl::~bpsk_demapper_impl()
{
}

void bpsk_demapper_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	ninput_items_required[0] = 8 * noutput_items;
}

int bpsk_demapper_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	char *out = (char *) output_items[0];

	int ni = 0;
	int no = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		if (in[ni].real() > 0)
		{
			d_byte |= (0x1 << d_i);
			d_one++;
		}
		else
		{
			d_zero++;
		}

		d_i++;
		ni++;

		if (d_i % 8 == 0)
		{
			/*
			 printf("BER = %d / %d = %.4f\n", d_zero, d_zero + d_one,
			 (float) d_zero / (float) (d_zero + d_one));
			 */
			out[no] = d_byte;
			d_i = 0;
			d_byte = 0;
			no++;
		}
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

