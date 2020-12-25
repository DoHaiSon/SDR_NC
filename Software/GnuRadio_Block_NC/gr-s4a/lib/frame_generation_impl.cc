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
#include "frame_generation_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>

namespace gr
{
namespace s4a
{

frame_generation::sptr frame_generation::make(int fft_length,
		int occupied_tones, int preamble_block_rep, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep)
{
	return gnuradio::get_initial_sptr(
			new frame_generation_impl(fft_length, occupied_tones,
					preamble_block_rep, pilot_block_rep, data_block_sz,
					data_block_rep, silence_block_rep));
}

/*
 * The private constructor
 */
frame_generation_impl::frame_generation_impl(int fft_length, int occupied_tones,
		int preamble_block_rep, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep) :
		gr::block("frame_generation",
				gr::io_signature::make(1, 1,
						sizeof(gr_complex) * occupied_tones),
				gr::io_signature::make(1, 1, sizeof(gr_complex) * fft_length)), d_fft_length(
				fft_length), d_occupied_tones(occupied_tones), d_zeros(
				fft_length, gr_complex(0, 0)), d_count(0), d_state(ST_IDLE), d_preamble_block_rep(
				preamble_block_rep), d_preamble_block_idx(0), d_pilot_block_rep(
				pilot_block_rep), d_pilot_block_idx(0), d_data_block_sz(
				data_block_sz), d_data_block_rep(data_block_rep), d_data_block_idx(
				0), d_silence_block_rep(silence_block_rep), d_silence_block_idx(
				0)
{
	if (occupied_tones > fft_length)
		throw std::runtime_error(
				"FFT length should be bigger than occupied tones...\n");

	d_left_padding = floor((fft_length - occupied_tones) / 2);

	// Init pilot sequence
	std::vector<gr_complex> pilot =
			s4a::frame_generation_impl::generate_complex_sequence(fft_length,
					d_left_padding, 0, 0);
	d_pilot = pilot;

	// Init preamble sequence
	std::vector<gr_complex> preamble =
			s4a::frame_generation_impl::generate_complex_sequence(fft_length,
					d_left_padding, 1, 0);
	d_preamble = preamble;

	// Init data buffer
	d_data.resize(d_fft_length * data_block_sz);
	std::fill(d_data.begin(), d_data.end(), gr_complex(0, 0));

	printf("init::frame_generation\n");

	if (preamble_block_rep < 1)
		throw std::runtime_error(
				"frame_generation::d_preamble_block_rep should be bigger than 0");

	if (pilot_block_rep < 1)
		throw std::runtime_error(
				"frame_generation::d_pilot_block_rep should be bigger than 0");

	d_block_sz = (preamble_block_rep + pilot_block_rep
			+ data_block_sz * data_block_rep + silence_block_rep);

	// Set timeout values
	d_timeout_syms_total = d_block_sz;
	d_timeout_syms_idx = 0;
	d_timeout_ref_sym = gr_complex(0, 0);

	/*
	 Set atomic output item size such that we can guarantee to process
	 the whole frame in one block execution.
	 */
	set_output_multiple(d_block_sz);
}

/*
 * Our virtual destructor.
 */
frame_generation_impl::~frame_generation_impl()
{
}

void frame_generation_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_block_sz)) / d_block_sz;
	ninput_items_required[0] = d_data_block_sz * coeff;
}

std::vector<gr_complex> frame_generation_impl::generate_complex_sequence(
		int seq_size, int left_padding, int zeros_period, int seed = 0)
{
	std::vector<gr_complex> result(seq_size, gr_complex(0, 0));

	int i = 0;
	int istart = left_padding;
	int istop = seq_size - left_padding;
	// Initialize seed
	srand(seed);

	std::vector<gr_complex> syms(2, gr_complex(0, 0));
	syms[0] = gr_complex(+0.7, +0.7);
	syms[1] = gr_complex(-0.7, -0.7);

	for (i = istart; i < istop; i += zeros_period + 1)
	{
		gr_complex sym = syms[rand() % 2];
		result[i] = sym;
	}
	return result;
}

int frame_generation_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];

	const gr_complex *in = (const gr_complex *) input_items[0];
	;
	gr_complex *out = (gr_complex *) output_items[0];

	int i = 0;
	int ni = 0;
	int no = 0;
	int nidx = 0;
	int nodx = 0;

	double timeout_check = 0.0;

	while (ni < ninput_items && no < noutput_items)
	{
		switch (d_state)
		{
		case ST_IDLE:
		{
			timeout_check = 0.0;
			for (i = 0; i < d_occupied_tones; i++)
			{
				timeout_check += abs(in[nidx + i]);
			}

			if (timeout_check > 0.0)
			{
				d_state = ST_PREAMBLE;
			}
			else
			{
				d_state = ST_TIMEOUT;
			}
			break;
		}
		case ST_PREAMBLE:
			if (d_preamble_block_idx < d_preamble_block_rep)
			{
				memcpy(&out[nodx], &d_preamble[0],
						d_fft_length * sizeof(gr_complex));
				nodx += d_fft_length;
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
				memcpy(&out[nodx], &d_pilot[0],
						d_fft_length * sizeof(gr_complex));
				nodx += d_fft_length;
				d_pilot_block_idx++;
				d_state = ST_PILOT;
			}
			else
			{
				d_pilot_block_idx = 0;
				d_state = ST_DATA_STORE;
			}
			break;
		case ST_DATA_STORE:
		{
			if (d_data_block_idx < d_data_block_sz)
			{
				for (i = 0; i < d_occupied_tones; i++)
				{
					d_data[d_data_block_idx * d_fft_length + d_left_padding + i] =
							in[nidx + i];
				}
				nidx += d_occupied_tones;
				d_data_block_idx++;
				d_state = ST_DATA_STORE;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_DATA;
			}
			break;
		}
		case ST_DATA:
			if (d_data_block_idx < d_data_block_rep)
			{

				memcpy(&out[nodx], &d_data[0],
						d_data_block_sz * d_fft_length * sizeof(gr_complex));

				nodx += d_data_block_sz * d_fft_length;
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
				memcpy(&out[nodx], &d_zeros[0],
						d_fft_length * sizeof(gr_complex)); // Fill with zeros
				nodx += d_fft_length;
				d_silence_block_idx++;
				d_state = ST_SILENCE;
			}
			else
			{
				ni += d_data_block_sz;
				no += d_block_sz;
				d_silence_block_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		case ST_TIMEOUT:
		{
			if (d_timeout_syms_idx < d_block_sz)
			{
				for (i = 0; i < d_fft_length; i++)
				{
					out[nodx + i] = d_timeout_ref_sym;
				}
				nodx += d_fft_length;
				d_timeout_syms_idx++;
			}
			else
			{
				nidx += d_data_block_sz * d_occupied_tones;
				ni += d_data_block_sz;
				no += d_block_sz;
				d_timeout_syms_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		}
		default:
			throw std::invalid_argument("frame generation::invalid state");
			break;

		}
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

