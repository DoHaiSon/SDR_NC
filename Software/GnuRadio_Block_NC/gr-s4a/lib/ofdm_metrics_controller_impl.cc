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
#include "ofdm_metrics_controller_impl.h"
#include <cstdio>

namespace gr
{
namespace s4a
{

ofdm_metrics_controller::sptr ofdm_metrics_controller::make(
		int data_syms_length, int channel_size)
{
	return gnuradio::get_initial_sptr(
			new ofdm_metrics_controller_impl(data_syms_length, channel_size));
}

/*
 * The private constructor
 */
ofdm_metrics_controller_impl::ofdm_metrics_controller_impl(int data_syms_length,
		int channel_size) :
		gr::block("ofdm_metrics_controller",
				gr::io_signature::make2(2, 2,
						sizeof(gr_complex) * data_syms_length,
						sizeof(gr_complex)
								* (int(pow(2, ceil(log2(5 + channel_size)))))),
				gr::io_signature::make(1, 1,
						sizeof(gr_complex) * data_syms_length)), d_data_syms_length(
				data_syms_length), d_channel_size(channel_size)
{
	d_nulled_syms = new gr_complex[data_syms_length];
	int i = 0;
	for (i = 0; i < data_syms_length; i++)
	{
		d_nulled_syms[i] = gr_complex(0, 0);
	}

	d_metrics_size = int(pow(2, ceil(log2(5 + channel_size))));

	printf("ofdm_metrics_controller::metrics_size %d\n", d_metrics_size);
}

/*
 * Our virtual destructor.
 */
ofdm_metrics_controller_impl::~ofdm_metrics_controller_impl()
{
}

void ofdm_metrics_controller_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = 1;
}

int ofdm_metrics_controller_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = std::min(ninput_items_v[0], ninput_items_v[1]);

	const gr_complex *in_syms = (const gr_complex *) input_items[0];
	const gr_complex *in_metrics = (const gr_complex *) input_items[1];

	gr_complex *out_ofdm = (gr_complex *) output_items[0];
	gr_complex *out_h_t = (gr_complex *) output_items[2];

	int i = 0;

	int ni = 0;
	int no = 0;

	int nidx_syms = 0;
	int nidx_metrics = 0;

	int nodx_ofdm = 0;
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

		}
		else if (in_metrics[nidx_metrics + 4].real() == 1.0)
		{
			/****************************************************
			 OFDM
			 *****************************************************/
			memcpy(&out_ofdm[nodx_ofdm], &in_syms[nidx_syms],
					sizeof(gr_complex) * d_data_syms_length);

		}
		else
		{
			printf("ofdm_metrics_controller::ni = %d,", ni);
			printf("ninput_items = %d, ", ninput_items);
			printf("no = %d, ", no);
			printf("noutput_items = %d\n ", noutput_items);
			printf("transmission_mode = %.4f + 1i*(%.4f) \n",
					in_metrics[nidx_metrics + 4].real(),
					in_metrics[nidx_metrics + 4].imag());
			throw std::runtime_error(
					"ofdm_metrics_controller::wrong transmission_mode");
		}

		nodx_ofdm += d_data_syms_length;
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

