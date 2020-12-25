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
#include "coherence_time_estimator_impl.h"
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <sys/timex.h>
#include <string.h>

#define TINY 1.0e-20

#define VERBOSE 1

namespace gr
{
namespace s4a
{

coherence_time_estimator::sptr coherence_time_estimator::make(
		int pearson_correlation, int output_mode, int data_sz, int channel_sz,
		int data_block_rep, int check_sz, float r_differentiation,
		float snr_threshold, int signal_bw, int frame_sz, int packet_header_sz,
		int packet_header_val, int packet_counter_sz, int packet_rep,
		int packet_zero_padding)
{
	return gnuradio::get_initial_sptr(
			new coherence_time_estimator_impl(pearson_correlation, output_mode,
					data_sz, channel_sz, data_block_rep, check_sz,
					r_differentiation, snr_threshold, signal_bw, frame_sz,
					packet_header_sz, packet_header_val, packet_counter_sz,
					packet_rep, packet_zero_padding));
}

/*
 * The private constructor
 */
coherence_time_estimator_impl::coherence_time_estimator_impl(
		int pearson_correlation, int output_mode, int data_sz, int channel_sz,
		int data_block_rep, int check_sz, float r_differentiation,
		float snr_threshold, int signal_bw, int frame_sz, int packet_header_sz,
		int packet_header_val, int packet_counter_sz, int packet_rep,
		int packet_zero_padding) :
		gr::block("coherence_time_estimator",
				gr::io_signature::make3(3, 3, data_sz * sizeof(gr_complex),
						channel_sz * sizeof(gr_complex), sizeof(float)),
				gr::io_signature::make3(3, 3, sizeof(int),
						channel_sz * sizeof(gr_complex), sizeof(float))), d_pearson_correlation(
				pearson_correlation), d_output_mode(output_mode), d_data_sz(
				data_sz), d_channel_sz(channel_sz), d_data_block_idx(0), d_data_block_rep(
				data_block_rep), d_check_idx(0), d_check_sz(check_sz), d_r_differentiation(
				r_differentiation), d_snr(0.0f), d_snr_threshold(snr_threshold), d_frame_sz(
				frame_sz), d_signal_bw(signal_bw), d_packet_header_sz(
				packet_header_sz), d_packet_header_val(packet_header_val), d_packet_counter_sz(
				packet_counter_sz), d_packet_rep(packet_rep), d_packet_zero_padding(
				packet_zero_padding), yt_1(0), xt_1(0), syy_1(0), sxx_1(0), ax_1(
				0), ay_1(0), mx_1(0), my_1(0), R_1(0.f), yt_2(0), xt_2(0), syy_2(
				0), sxx_2(0), ax_2(0), ay_2(0), mx_2(0), my_2(0), R_2(0.f), R(
				0.f), R_old(0.f), R_diff(0.f), d_previous_counter(0), d_current_counter(
				0), d_current_header(0), d_current_zeros(0), d_coherence_time(
				0), d_coherence_distance(0), d_state(ST_IDLE), d_is_changed(
				false), d_is_out(false)
{
	// Select what kind of correleation we will use (1D or 2D)
	if (pearson_correlation == 1)
	{
		printf("init::coherence_time_estimator::1D Pearson Correlation\n");
		correlate = &s4a::coherence_time_estimator_impl::calculate_R_1D;
	}
	else
	{
		printf("init::coherence_time_estimator::2D Pearson Correlation\n");
		correlate = &s4a::coherence_time_estimator_impl::calculate_R_2D;
	}

	// Init "current h" vector
	d_current_h.resize(channel_sz);
	std::fill(d_current_h.begin(), d_current_h.end(), gr_complex(0, 0));

	// Init "previous h" vector
	d_previous_h.resize(channel_sz);
	std::fill(d_previous_h.begin(), d_previous_h.end(), gr_complex(0, 0));

	printf("coherence time coefficient differentiation = %.4f\n",
			r_differentiation);

	d_packet_total_bits = packet_rep * (packet_header_sz + packet_counter_sz)
			+ packet_zero_padding;

	d_bit_threshhold = (data_block_rep - (data_block_rep % 2)) / 2;

	printf(
			"init::coherence_time_estimator:: packet size: %d, header size: %d, counter size: %d, packet repetition: %d, zero padding: %d\n",
			d_packet_total_bits, packet_header_sz, packet_counter_sz,
			packet_rep, packet_zero_padding);

	if (data_block_rep * d_packet_total_bits > 0)
	{
		d_packet = (unsigned char*) malloc(d_packet_total_bits);
		memset(d_packet, 0x00, d_packet_total_bits);
	}

	d_symbol_time = 1 / ((double) signal_bw);
	printf(
			"init::coherence_time_estimator:: frame size: %d, symbol duration: %.6f s \n",
			frame_sz, d_symbol_time);
}

/*
 * Our virtual destructor.
 */
coherence_time_estimator_impl::~coherence_time_estimator_impl()
{
}

void coherence_time_estimator_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

void coherence_time_estimator_impl::calculate_R_1D(const gr_complex *in,
		int idx)
{
	int j = 0;
	yt_1 = 0;
	xt_1 = 0;
	syy_1 = 0;
	sxy_1 = 0;
	sxx_1 = 0;

	// Calculate mean of the new serie
	my_1 = 0;
	for (j = 0; j < d_channel_sz; j++)
	{
		my_1 += abs(in[idx + j]);
		d_current_h[j] = in[idx + j]; // Store the current value also
	}
	my_1 /= d_channel_sz;

	// Calculate coefficients

	for (j = 0; j < d_channel_sz; j++)
	{
		xt_1 = abs(d_previous_h[j]) - ax_1;
		yt_1 = abs(in[idx + j]) - ay_1;
		sxx_1 = sxx_1 + xt_1 * xt_1;
		syy_1 = syy_1 + yt_1 * yt_1;
		sxy_1 = sxy_1 + xt_1 * yt_1;
	}

	R = sxy_1 / (sqrt(sxx_1 * syy_1) + TINY);
}

void coherence_time_estimator_impl::calculate_R_2D(const gr_complex *in,
		int idx)
{
	int j = 0;
	yt_1 = 0;
	xt_1 = 0;
	syy_1 = 0;
	sxy_1 = 0;
	sxx_1 = 0;

	yt_2 = 0;
	xt_2 = 0;
	syy_2 = 0;
	sxy_2 = 0;
	sxx_2 = 0;

	// Calculate mean of the new serie
	my_1 = 0;
	my_2 = 0;
	for (j = 0; j < d_channel_sz; j++)
	{
		my_1 += (in[idx + j]).real();
		my_2 += (in[idx + j]).imag();
		d_current_h[j] = in[idx + j]; // Store the current value also
	}
	my_1 /= d_channel_sz;
	my_2 /= d_channel_sz;

	// Calculate coefficients
	for (j = 0; j < d_channel_sz; j++)
	{
		xt_1 = (d_previous_h[j]).real() - ax_1;
		xt_2 = (d_previous_h[j]).imag() - ax_2;

		yt_1 = (in[idx + j]).real() - ay_1;
		yt_2 = (in[idx + j]).imag() - ay_2;

		sxx_1 = sxx_1 + xt_1 * xt_1;
		sxx_2 = sxx_2 + xt_2 * xt_2;

		syy_1 = syy_1 + yt_1 * yt_1;
		syy_2 = syy_2 + yt_2 * yt_2;

		sxy_1 = sxy_1 + xt_1 * yt_1;
		sxy_2 = sxy_2 + xt_2 * yt_2;
	}

	R_1 = sxy_1 / (sqrt(sxx_1 * syy_1) + TINY);
	R_2 = sxy_2 / (sqrt(sxx_2 * syy_2) + TINY);
	R = (R_1 + R_2) / 2.0;
}

int coherence_time_estimator_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = std::min(ninput_items_v[0], ninput_items_v[1]);

	const gr_complex *in_syms = (const gr_complex *) input_items[0];
	const gr_complex *in_h = (const gr_complex *) input_items[1];
	const float *in_snr = (const float *) input_items[2];

	int *out_counter = (int *) output_items[0];
	gr_complex *out_h = (gr_complex *) output_items[1];
	float *out_r = (float *) output_items[2];

	int i = 0;
	int j = 0;

	int ni = 0;
	int ni_idx = 0;

	int no = 0;
	int no_idx = 0;

	unsigned char bit = 0;

	while (ni < ninput_items && no < noutput_items)
	{
		switch (d_state)
		{
		case ST_IDLE:
			d_state = ST_CHECK_SNR;
			break;
		case ST_CHECK_SNR:
			d_snr = in_snr[ni];
			d_state = ST_ESTIMATE_COEFFICIENT;
			break;
		case ST_ESTIMATE_COEFFICIENT:
		{
			ni_idx = ni * d_channel_sz;
			//correlate(&in_h[ni_idx], 0);
			(this
					->*((s4a::coherence_time_estimator_impl*) this)->s4a::coherence_time_estimator_impl::correlate)(
					&in_h[ni_idx], 0);
			memset(&d_packet[0], 0x00, d_packet_total_bits);
			d_state = ST_DEMODULATE_DATA;
			break;
		}
		case ST_DEMODULATE_DATA:
		{
			ni_idx = ni * d_data_sz;
			if (d_data_block_idx < d_data_block_rep)
			{
				//BPSK Demapping for counter packet
				for (i = 0; i < d_data_sz; i++)
				{
					if (in_syms[ni_idx + i].real() > 0)
					{
						d_packet[i] = d_packet[i] + 1;
					}
					else
					{

					}
				}
				ni++;
				d_data_block_idx++;
				d_state = ST_DEMODULATE_DATA;
			}
			else
			{
				d_data_block_idx = 0;
				d_state = ST_PACKET_DECODE;
			}
			break;
		}
		case ST_PACKET_DECODE:
		{
			// Get header
			d_current_header = 0;
			for (i = 0, j = 0; i < d_packet_header_sz; i++, j++)
			{
				bit = d_packet[i];
				if (bit > d_bit_threshhold)
				{
					d_current_header |= (0x1 << j);
				}
			}

			// Get counter
			d_current_counter = 0;
			for (i = d_packet_header_sz, j = 0;
					i < d_packet_total_bits - d_packet_zero_padding; i++, j++)
			{
				bit = d_packet[i];
				if (bit > d_bit_threshhold)
				{
					d_current_counter |= (0x1 << j);
				}
			}

			// Get zeros
			d_current_zeros = 0;
			for (i = d_packet_header_sz + d_packet_counter_sz, j = 0;
					i < d_packet_total_bits; i++, j++)
			{
				bit = d_packet[i];
				if (bit = d_packet[i] > d_bit_threshhold)
				{
					d_current_zeros |= (0x1 << j);
				}
			}
			d_state = ST_VALIDATE;
			break;
		}
		case ST_VALIDATE:
		{
			R_diff = abs(R - R_old);
			if (d_snr > d_snr_threshold
					&& d_current_header == d_packet_header_val
					&& R_diff >= d_r_differentiation)
			{
				for (i = 0; i < d_channel_sz; i++)
				{
					d_previous_h[i] = d_current_h[i];
				}

				d_coherence_distance = (double) ((d_current_counter
						- d_previous_counter) * d_frame_sz);
				d_coherence_time = d_coherence_distance * d_symbol_time;

				if (VERBOSE)
				{
					printf(" Z = %d", d_current_zeros);
					printf(" H = %d,", d_current_header);
					printf(" C = %d,", d_current_counter);
					printf(" est_R = %.4f, ", (float) R);
					printf(" est_Rdiff = %.4f, ", (float) R_diff);
					printf(" est_SNR = %.4f, ", d_snr);
					printf(" est_cdist = %.4f, ", d_coherence_distance);
					printf(" est_cdiff = %d, ",
							d_current_counter - d_previous_counter);
					printf(" est_ctime = %.4f secs ", d_coherence_time);
					printf("\n");
				}

				d_previous_counter = d_current_counter;
				R_old = R;
				d_is_changed = true;
			}
			else
			{
				d_is_changed = false;
			}

			d_state = ST_FINALIZE;
			break;
		}
		case ST_FINALIZE:
		{
			// Decide if we should send results to output
			if (d_output_mode == 1)
			{
				if (d_is_changed == true)
				{
					d_is_out = true;
				}
				else
				{
					d_is_out = false;
				}
			}
			else
			{
				d_is_out = true;
			}

			if (d_is_out == true)
			{
				no_idx = no * d_channel_sz;
				out_counter[no] = d_current_counter;
				out_r[no] = (float) R;
				for (i = 0; i < d_channel_sz; i++)
				{
					out_h[no_idx + i] = d_current_h[i];
				}
				no++;
			}
			d_state = ST_IDLE;
			break;
		}
		default:
			throw std::invalid_argument(
					"coherence_time_estimator::invalid state");
			break;
		}
	}

	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */

