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
#include "csi_converter_impl.h"
#include <cstdio>
#include <string.h>

namespace gr
{
namespace s4a
{

csi_converter::sptr csi_converter::make(int data_syms_length, int channel_size)
{
	return gnuradio::get_initial_sptr(
			new csi_converter_impl(data_syms_length, channel_size));
}

/*
 * The private constructor
 */
csi_converter_impl::csi_converter_impl(int data_syms_length, int channel_size) :
		gr::block("csi_converter",
				gr::io_signature::make2(2, 2,
						sizeof(gr_complex) * data_syms_length,
						sizeof(gr_complex) * channel_size),
				gr::io_signature::make2(2, 2,
						sizeof(gr_complex) * data_syms_length,
						sizeof(gr_complex) * channel_size)), d_data_syms_length(
				data_syms_length), d_channel_size(channel_size)
{
	// Load FFTW
	d_ifft = new fft::fft_complex(channel_size, true);
}

/*
 * Our virtual destructor.
 */
csi_converter_impl::~csi_converter_impl()
{
}

void csi_converter_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int csi_converter_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = std::min(ninput_items_v[0], ninput_items_v[1]);

	const gr_complex *in1 = (const gr_complex *) input_items[0];
	const gr_complex *in2 = (const gr_complex *) input_items[1];

	gr_complex *out1 = (gr_complex *) output_items[0];
	gr_complex *out2 = (gr_complex *) output_items[1];

	int i = 0;

	int ni = 0;
	int no = 0;

	int nidx1 = 0;
	int nidx2 = 0;

	int nodx1 = 0;
	int nodx2 = 0;

	double timeout_check = 0.0;
	while (ni < ninput_items && no < noutput_items)
	{
		timeout_check = 0;
		for (i = 0; i < d_channel_size; i++)
		{
			timeout_check += abs(in2[nidx2 + i]);
		}

		if (timeout_check > 0.0)
		{
			// Copy symbols to the input buffer
			memcpy(d_ifft->get_inbuf(), &in2[nidx2],
					d_channel_size * sizeof(gr_complex));

			// Get the input buffer of FFTW
			gr_complex *dst = d_ifft->get_inbuf();

			// half length of complex array (for FFT shift)
			unsigned int len = (unsigned int) (floor(d_channel_size / 2.0));
			memcpy(&dst[0], &in2[nidx2 + len],
					sizeof(gr_complex) * (d_channel_size - len));
			memcpy(&dst[d_channel_size - len], &in2[nidx2],
					sizeof(gr_complex) * len);

			// Execute IFFT
			d_ifft->execute();

			// Send the result to the output
			memcpy(&out2[nodx2], d_ifft->get_outbuf(),
					sizeof(gr_complex) * d_channel_size);

			memcpy(&out1[nodx1], &in1[nidx1],
					sizeof(gr_complex) * d_data_syms_length);
		}
		else
		{
			for (i = 0; i < d_data_syms_length; i++)
			{
				out1[nodx1 + i] = gr_complex(0, 0);
			}
			memcpy(&out2[nodx2], &in2[nidx2],
					sizeof(gr_complex) * d_channel_size);
		}

		nodx1 += d_data_syms_length;
		nodx2 += d_channel_size;
		no++;

		nidx1 += d_data_syms_length;
		nidx2 += d_channel_size;
		ni++;
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

