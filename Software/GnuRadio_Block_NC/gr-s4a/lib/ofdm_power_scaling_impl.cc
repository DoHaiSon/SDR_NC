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
#include "ofdm_power_scaling_impl.h"

namespace gr
{
namespace s4a
{

ofdm_power_scaling::sptr ofdm_power_scaling::make(int fft_length,
		float preamble_coeff, float pilot_coeff, float data_coeff,
		int preamble_block_rep, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep)
{
	return gnuradio::get_initial_sptr(
			new ofdm_power_scaling_impl(fft_length, preamble_coeff, pilot_coeff,
					data_coeff, preamble_block_rep, pilot_block_rep,
					data_block_sz, data_block_rep, silence_block_rep));
}

/*
 * The private constructor
 */
ofdm_power_scaling_impl::ofdm_power_scaling_impl(int fft_length,
		float preamble_coeff, float pilot_coeff, float data_coeff,
		int preamble_block_rep, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep) :
		gr::block("ofdm_power_scaling",
				gr::io_signature::make(1, 1, sizeof(gr_complex) * fft_length),
				gr::io_signature::make(1, 1, sizeof(gr_complex) * fft_length)), d_fft_length(
				fft_length), d_preamble_coeff(preamble_coeff), d_pilot_coeff(
				pilot_coeff), d_data_coeff(data_coeff), d_count(0), d_state(
				ST_PREAMBLE), d_preamble_block_rep(preamble_block_rep), d_preamble_block_idx(
				0), d_pilot_block_rep(pilot_block_rep), d_pilot_block_idx(0), d_data_block_sz(
				data_block_sz), d_data_block_rep(data_block_rep), d_data_block_idx(
				0), d_silence_block_rep(silence_block_rep), d_silence_block_idx(
				0)
{
	if (d_pilot_block_rep < 1)
		throw std::runtime_error(
				"ofdm_power_scaling::d_pilot_block_rep should be bigger than 0");

	if (d_preamble_block_rep < 1)
		throw std::runtime_error(
				"ofdm_power_scaling::d_preamble_block_rep should be bigger than 0");

	d_data_block_total = data_block_rep * data_block_sz;
}

/*
 * Our virtual destructor.
 */
ofdm_power_scaling_impl::~ofdm_power_scaling_impl()
{
}

void ofdm_power_scaling_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int ofdm_power_scaling_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = std::min(ninput_items_v[0], ninput_items_v[0]);
	const gr_complex *in = (const gr_complex *) input_items[0];

	gr_complex *out = (gr_complex *) output_items[0];

	int i = 0;
	int no = 0; // number items output
	int ni = 0; // number items read from input
	int nidx = 0;
	int nodx = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		nidx = d_fft_length * ni;
		nodx = d_fft_length * no;

		switch (d_state)
		{
		case ST_IDLE:
			d_state = ST_PREAMBLE;
			break;
		case ST_PREAMBLE:
			if (d_preamble_block_idx < d_preamble_block_rep)
			{
				for (i = 0; i < d_fft_length; i++)
				{
					out[nodx + i] = d_preamble_coeff * in[nidx + i];
				}
				ni++;
				no++;
				d_preamble_block_idx++;
				d_state = ST_PREAMBLE;
			}
			else
			{
				d_preamble_block_idx = 0;
				d_state = ST_PILOT;
			}
			break;
		case ST_PILOT:
			if (d_pilot_block_idx < d_pilot_block_rep)
			{
				for (i = 0; i < d_fft_length; i++)
				{
					out[nodx + i] = d_pilot_coeff * in[nidx + i];
				}
				ni++;
				no++;
				d_pilot_block_idx++;
				d_state = ST_PILOT;
			}
			else
			{
				d_pilot_block_idx = 0;
				d_state = ST_DATA;
			}
			break;
		case ST_DATA:
			// Check if we have data block?
			if (d_data_block_idx < d_data_block_total)
			{
				for (i = 0; i < d_fft_length; i++)
				{
					out[nodx + i] = d_data_coeff * in[nidx + i];
				}
				ni++;
				no++;
				d_data_block_idx++;
				d_state = ST_DATA;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_SILENCE;
			}
			break;
		case ST_SILENCE:
			if (d_silence_block_idx < d_silence_block_rep)
			{
				for (i = 0; i < d_fft_length; i++)
				{
					out[nodx + i] = in[nidx + i];
				}
				ni++;
				no++;
				d_silence_block_idx++;
				d_state = ST_SILENCE;
			}
			else
			{
				d_silence_block_idx = 0;
				d_state = ST_PREAMBLE;
			}
			break;
		default:
			throw std::invalid_argument("ofdm_power_scaling::invalid state");
			break;
		}
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

