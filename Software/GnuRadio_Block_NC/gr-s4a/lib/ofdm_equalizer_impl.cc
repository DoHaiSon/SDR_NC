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
#include "ofdm_equalizer_impl.h"
#include <s4a/frame_generation.h>

#define VERBOSE 1
#define M_TWOPI (2*M_PI)
#define TINY 1.0e-20
#define MAX_NUM_SYMBOLS 1000

namespace gr
{
namespace s4a
{

ofdm_equalizer::sptr ofdm_equalizer::make(int fft_length, int occupied_tones,
		int cp_length, int preamble_block_rep, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		int verbose)
{
	return gnuradio::get_initial_sptr(
			new ofdm_equalizer_impl(fft_length, occupied_tones, cp_length,
					preamble_block_rep, pilot_block_rep, data_block_sz,
					data_block_rep, silence_block_rep, verbose));
}

/*
 * The private constructor
 */
ofdm_equalizer_impl::ofdm_equalizer_impl(int fft_length, int occupied_tones,
		int cp_length, int preamble_block_rep, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		int verbose) :
		gr::block("ofdm_equalizer",
				gr::io_signature::make(1, 1, fft_length * sizeof(gr_complex)),
				gr::io_signature::make3(3, 3,
						data_block_sz * occupied_tones * sizeof(gr_complex),
						occupied_tones * sizeof(gr_complex), sizeof(float))), d_fft_length(
				fft_length), d_occupied_tones(occupied_tones), d_cp_length(
				cp_length), d_preamble_block_rep(preamble_block_rep), d_preamble_block_idx(
				0), d_pilot_block_rep(pilot_block_rep), d_pilot_block_idx(0), d_data_block_sz(
				data_block_sz), d_data_block_rep(data_block_rep), d_data_block_idx(
				0), d_silence_block_rep(silence_block_rep), d_silence_block_idx(
				0), d_state(ST_IDLE), d_est_SNR(0.0f), d_signal_power(0.0f), d_noise_power(
				0.0f), d_preamble_power(0.0f), d_pilot_power(0.0f), d_data_power(
				0.0f), d_verbose(verbose)
{
	d_left_padding = floor((fft_length - occupied_tones) / 2);

	// Init syms sequence (to store data syms)
	d_data.resize(occupied_tones * data_block_sz * data_block_rep);
	std::fill(d_data.begin(), d_data.end(), gr_complex(0, 0));

	// Init pilot sequence
	std::vector<gr_complex> pilot = generate_complex_sequence(fft_length,
			d_left_padding, 0, 0);
	d_pilot = pilot;

	// Init h_est
	d_h_est.resize(occupied_tones);
	std::fill(d_h_est.begin(), d_h_est.end(), gr_complex(1, 1));

	// Init h_est_sum
	d_h_est_sum.resize(occupied_tones);
	std::fill(d_h_est_sum.begin(), d_h_est_sum.end(), gr_complex(0, 0));

	d_block_sz = preamble_block_rep + pilot_block_rep
			+ data_block_sz * data_block_rep + silence_block_rep;

	d_data_block_total = data_block_sz * data_block_rep;
	/*
	 Init counter packet for coherence time estimation
	 */
	if (occupied_tones < 48)
	{
		throw std::runtime_error(
				"init::ofdm_equalizer::Occupied tones should be greater than 48\n");
	}

	if (occupied_tones % 8 != 0)
	{
		throw std::runtime_error(
				"init::ofdm_equalizer::Occupied tones should be multiple of 8\n");
	}

	if (data_block_rep % 2 != 1)
	{
		throw std::runtime_error(
				"init::ofdm_equalizer::data_block_rep should be an odd number [1 3 5 ... ]\n");
	}

	if (preamble_block_rep % 2 != 0 && preamble_block_rep != 1)
	{
		throw std::runtime_error(
				"init::ofdm_equalizer::preamble_block_rep should be an even number or 1 [1 2 4 ... ]\n");
	}

	set_output_multiple(data_block_rep);
}

/*
 * Our virtual destructor.
 */
ofdm_equalizer_impl::~ofdm_equalizer_impl()
{
}

std::vector<gr_complex> ofdm_equalizer_impl::generate_complex_sequence(
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

void ofdm_equalizer_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_data_block_rep))
			/ d_data_block_rep;
	ninput_items_required[0] = d_block_sz * coeff;
}

int ofdm_equalizer_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in_syms = (const gr_complex *) input_items[0];

	gr_complex *out_syms = (gr_complex *) output_items[0];
	gr_complex *out_h = (gr_complex *) output_items[1];
	float *out_snr = (float *) output_items[2];

	int i = 0;

	int no = 0; // number items output
	int ni = 0; // number items read from input
	int nidx = 0;
	int nodx = 0;
	int didx = 0;

	double temp_power = 0.0f;

	while (ni < ninput_items && no < noutput_items)
	{
		//printf("nin: %i, nout: %i \n",ninput_items,noutput_items);
		switch (d_state)
		{
		case ST_IDLE:
			//printf("Idle_");
			d_state = ST_PREAMBLE;
			break;
		case ST_PREAMBLE:
			//printf("Preamble_");
			if (d_preamble_block_idx < d_preamble_block_rep)
			{
				for (i = 0; i < d_occupied_tones; i++)
				{
					temp_power = pow(abs(in_syms[nidx + d_left_padding + i]),2);
					d_signal_power += temp_power;
					d_preamble_power += temp_power;
				}
				nidx += d_fft_length;
				d_preamble_block_idx++;
			}
			else
			{
				d_preamble_block_idx = 0;
				d_state = ST_PILOT;
			}
			break;
		case ST_PILOT:
			//printf("Pilot_");
		{
			if (d_pilot_block_idx < d_pilot_block_rep)
			{
				for (i = 0; i < d_occupied_tones; i++)
				{
					d_h_est_sum[i] += (in_syms[nidx + d_left_padding + i]
							/ d_pilot[d_left_padding + i]);
					temp_power = pow(abs(in_syms[nidx + d_left_padding + i]),
							2); // SNR purpose
					d_signal_power += temp_power;
					d_pilot_power += temp_power;
				}
				nidx += d_fft_length;
				d_pilot_block_idx++;
			}
			else
			{
				d_pilot_block_idx = 0;
				d_state = ST_PILOT_POST;
			}
			break;
		}
		case ST_PILOT_POST: // It's time to process after we receive all the pilot blocks!
			//printf("Pilot-post_");
		{
			for (i = 0; i < d_occupied_tones; i++)
			{
				d_h_est[i] = gr_complex(
						d_h_est_sum[i].real() / d_pilot_block_rep,
						d_h_est_sum[i].imag() / d_pilot_block_rep);
				d_h_est_sum[i] = gr_complex(TINY, TINY);
			}
			didx = 0;
			d_state = ST_DATA;
			break;
		}
		case ST_DATA:
			//printf("Data_");
		{
			// Is data block available?
			if (d_data_block_idx < d_data_block_total)
			{
				for (i = 0; i < d_occupied_tones; i++)
				{
					d_data[didx + i] = in_syms[nidx + d_left_padding + i]
							/ d_h_est[i];
					temp_power += pow(abs(in_syms[nidx + d_left_padding + i]),
							2); // SNR purpose
					d_signal_power += temp_power;
					d_data_power += temp_power;
				}
				nidx += d_fft_length;
				didx += d_occupied_tones;
				d_data_block_idx++;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_SILENCE;
			}
			break;
		}
		case ST_SILENCE:
			//printf("Silence_");
		{
			if (d_silence_block_idx < d_silence_block_rep)
			{
				for (i = 0; i < d_occupied_tones; i++)
				{
					gr_complex s1 = in_syms[nidx + d_left_padding + i];
					d_noise_power += pow(abs(s1), 2); // SNR purpose
				}
				nidx += d_fft_length;
				d_silence_block_idx++;
			}
			else
			{
				if (d_silence_block_rep > 0)
				{
					//d_est_SNR = 10*log10((d_signal_power/(d_preamble_block_rep + d_pilot_block_rep + 														d_data_block_sz*d_data_block_rep))
					//						/ (d_noise_power/ (d_silence_block_rep)));
					d_preamble_power = d_preamble_power
							/ (d_occupied_tones * d_preamble_block_rep);
					d_pilot_power = d_pilot_power
							/ (d_occupied_tones * d_pilot_block_rep);
					d_data_power = d_data_power
							/ (d_occupied_tones * d_data_block_sz
									* d_data_block_rep);
					d_noise_power = (d_noise_power + TINY)
							/ (d_occupied_tones * d_silence_block_rep);
					d_est_SNR = 10 * log10(d_preamble_power / d_noise_power);
					if (d_verbose == 1 && d_preamble_power > 1.0)
					{
						

						printf("\nOFDM Eq > ");
						printf("Pre: %4.2f dB, \t",
								10 * log10(d_preamble_power / d_noise_power));
						printf("Pil: %4.2f dB, \t",
								10 * log10(d_pilot_power / d_noise_power));
						printf("Dat: %4.2f dB, \t",
								10 * log10(d_data_power / d_noise_power));
						printf("Sil: %4.2f dB, \t",
								10 * log10(d_noise_power / d_noise_power));
						printf("Pre/Data = %4.2f dB",
								10 * log10(d_preamble_power / d_data_power));
					}

					d_signal_power = 0.0f;
					d_preamble_power = 0.0f;
					d_pilot_power = 0.0f;
					d_data_power = 0.0f;
					d_noise_power = 0.0f;
				}

				d_silence_block_idx = 0;
				didx = 0;
				d_state = ST_OUT;
			}
			break;
		}
		case ST_OUT:
		{
			if (d_data_block_idx < d_data_block_rep)
			{
				// Send data symbols to output
				for (i = 0; i < d_data_block_sz * d_occupied_tones; i++)
				{
					out_syms[nodx + i] = d_data[didx + i];
				}

				// Send estimated channel to output
				for (i = 0; i < d_occupied_tones; i++)
				{
					out_h[nodx + i] = d_h_est[i];
				}

				// Send estimated SNR to output
				out_snr[no] = (float) d_est_SNR;

				nodx += (d_occupied_tones * d_data_block_sz);
				didx += (d_occupied_tones * d_data_block_sz);
				d_data_block_idx++;
				d_state = ST_OUT;
			}
			else
			{
				ni += d_block_sz;
				no += d_data_block_rep;
				d_data_block_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		}
		default:
			throw std::invalid_argument("ofdm_equalizer::invalid state");
			break;
		}
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

