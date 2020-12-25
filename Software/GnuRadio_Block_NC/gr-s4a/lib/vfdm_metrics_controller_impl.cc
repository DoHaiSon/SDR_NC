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
#include "vfdm_metrics_controller_impl.h"
#include <cstdio>
#include <string.h>

namespace gr
{
namespace s4a
{

vfdm_metrics_controller::sptr vfdm_metrics_controller::make(
		int data_syms_length, int channel_size, int normalize_h)
{
	return gnuradio::get_initial_sptr(
			new vfdm_metrics_controller_impl(data_syms_length, channel_size,
					normalize_h));
}

/*
 * The private constructor
 */
vfdm_metrics_controller_impl::vfdm_metrics_controller_impl(int data_syms_length,
		int channel_size, int normalize_h) :
		gr::block("vfdm_metrics_controller",
				gr::io_signature::make2(2, 2,
						sizeof(gr_complex) * data_syms_length,
						sizeof(gr_complex)
								* (int(pow(2, ceil(log2(5 + channel_size)))))),
				gr::io_signature::make3(3, 3,
						sizeof(gr_complex) * data_syms_length,
						sizeof(gr_complex) * data_syms_length,
						sizeof(gr_complex) * channel_size)), d_channel_size(
				channel_size), d_normalize_h(normalize_h)
{
	// Load FFTW
	d_ifft = new fft::fft_complex(channel_size, true);

	d_nulled_syms = new gr_complex[data_syms_length];
	d_nulled_h_t = new gr_complex[channel_size];
	int i = 0;
	for (i = 0; i < data_syms_length; i++)
	{
		d_nulled_syms[i] = gr_complex(0, 0);
	}
	for (i = 0; i < channel_size; i++)
	{
		d_nulled_h_t[i] = gr_complex(0, 0);
	}

	d_metrics_size = int(pow(2, ceil(log2(5 + channel_size))));

	printf("vfdm_metrics_controller::metrics_size %d\n", d_metrics_size);
}

/*
 * Our virtual destructor.
 */
vfdm_metrics_controller_impl::~vfdm_metrics_controller_impl()
{
}

void vfdm_metrics_controller_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int vfdm_metrics_controller_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = std::min(ninput_items_v[0], ninput_items_v[1]);

	const gr_complex *in_syms = (const gr_complex *) input_items[0];
	const gr_complex *in_metrics = (const gr_complex *) input_items[1];

	gr_complex *out_ofdm = (gr_complex *) output_items[0];
	gr_complex *out_vfdm = (gr_complex *) output_items[1];
	gr_complex *out_h_t = (gr_complex *) output_items[2];

	int i = 0;

	int ni = 0;
	int no = 0;

	int nidx_syms = 0;
	int nidx_metrics = 0;

	int nodx_ofdm = 0;
	int nodx_vfdm = 0;
	int nodx_h_t = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		if (in_metrics[nidx_metrics + 4].real() == 0.0)
		{
			/****************************************************
			 ZEROS
			 *****************************************************/
			// Send zeros to the all outputs
			memcpy(&out_ofdm[nodx_h_t], &d_nulled_syms[0],
					sizeof(gr_complex) * d_data_syms_length);
			memcpy(&out_vfdm[nodx_vfdm], &d_nulled_syms[0],
					sizeof(gr_complex) * d_data_syms_length);
			memcpy(&out_h_t[nodx_ofdm], &d_nulled_h_t[0],
					sizeof(gr_complex) * d_channel_size);

		}
		else if (in_metrics[nidx_metrics + 4].real() == 1.0)
		{
			/****************************************************
			 OFDM
			 *****************************************************/
			// Null h_t
			memcpy(&out_ofdm[nodx_ofdm], &in_syms[nidx_syms],
					sizeof(gr_complex) * d_data_syms_length);

			// Null out_vfdm
			memcpy(&out_vfdm[nodx_vfdm], &d_nulled_syms[0],
					sizeof(gr_complex) * d_data_syms_length);

			// Send input symbols to the out_ofdm
			memcpy(&out_h_t[nodx_h_t], &d_nulled_h_t[0],
					sizeof(gr_complex) * d_channel_size);

		}
		else if (in_metrics[nidx_metrics + 4].real() == 2.0)
		{
			/****************************************************
			 VFDM
			 *****************************************************/
			// Copy symbols to the input buffer
			memcpy(d_ifft->get_inbuf(), &in_metrics[nidx_metrics + 5],
					d_channel_size * sizeof(gr_complex));

			// Get the input buffer of FFTW
			gr_complex *dst = d_ifft->get_inbuf();

			// half length of complex array (for FFT shift)
			unsigned int len = (unsigned int) (floor(d_channel_size / 2.0));
			memcpy(&dst[0], &in_metrics[nidx_metrics + 5 + len],
					sizeof(gr_complex) * (d_channel_size - len));
			memcpy(&dst[d_channel_size - len], &in_metrics[nidx_metrics + 5],
					sizeof(gr_complex) * len);

			// Execute IFFT
			d_ifft->execute();

			// Send the h_t to the output
			memcpy(&out_h_t[nodx_h_t], d_ifft->get_outbuf(),
					sizeof(gr_complex) * d_channel_size);

			// Normalize h_t
			if (d_normalize_h == 1)
			{
				float max_abs_h = 0.0f;
				float max_abs_h_inv = 0.0f;
				float abs_h;
				for (i = 0; i < d_channel_size; i++)
				{
					abs_h = abs(out_h_t[nodx_h_t + i]);
					if (abs_h > max_abs_h)
					{
						max_abs_h = abs_h;
					}
				}
				if (max_abs_h > 0)
				{
					max_abs_h_inv = 1 / (max_abs_h);

					for (i = 0; i < d_channel_size; i++)
					{
						gr_complex s1 = out_h_t[nodx_h_t + i];
						out_h_t[nodx_h_t + i] = max_abs_h_inv * s1;
					}
				}
			}

			// Send the input symbols to the vfdm_out
			memcpy(&out_vfdm[nodx_vfdm], &in_syms[nidx_syms],
					sizeof(gr_complex) * d_data_syms_length);

			// Null the ofdm_out by pushing zeros
			memcpy(&out_ofdm[nodx_ofdm], &d_nulled_syms[0],
					sizeof(gr_complex) * d_data_syms_length);

		}
		else
		{
			printf("vfdm_metrics_controller::ni = %d,", ni);
			printf("ninput_items = %d, ", ninput_items);
			printf("no = %d, ", no);
			printf("noutput_items = %d\n ", noutput_items);
			printf("transmission_mode = %.4f + 1i*(%.4f) \n",
					in_metrics[nidx_metrics + 4].real(),
					in_metrics[nidx_metrics + 4].imag());
			throw std::runtime_error(
					"vfdm_metrics_controller::wrong transmission_mode");
		}

		nodx_ofdm += d_data_syms_length;
		nodx_vfdm += d_data_syms_length;
		nodx_h_t += d_channel_size;
		no++;

		nidx_syms += d_data_syms_length;
		nidx_metrics += d_metrics_size;
		ni++;
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

