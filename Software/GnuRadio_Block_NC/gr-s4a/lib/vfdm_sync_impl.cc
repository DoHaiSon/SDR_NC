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
#include "vfdm_sync_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <complex.h>
#include <math.h>
#include <time.h>

#define VERBOSE 0

#define M_TWOPI (2*M_PI)
#define TINY    1.0e-20
#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr
{
namespace s4a
{

vfdm_sync::sptr vfdm_sync::make(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int preamble_block_rep,
		int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep,
		int cfo_next_preamble_distance, int time_out, int cfo_sync_method)
{
	return gnuradio::get_initial_sptr(
			new vfdm_sync_impl(fft_length, occupied_tones, cp_length,
					precoder_length, preamble_block_rep, pilot_block_t_sz,
					pilot_block_rep, data_block_sz, data_block_rep,
					silence_block_rep, cfo_next_preamble_distance, time_out,
					cfo_sync_method));
}

/*
 * The private constructor
 */
vfdm_sync_impl::vfdm_sync_impl(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int preamble_block_rep,
		int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep,
		int cfo_next_preamble_distance, int time_out, int cfo_sync_method) :
		gr::block("vfdm_sync", gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex))), d_fft_length(
				fft_length), d_occupied_tones(occupied_tones), d_cp_length(
				cp_length), d_precoder_length(precoder_length), d_preamble_block_idx(
				0), d_preamble_block_rep(preamble_block_rep), d_pilot_block_idx(
				0), d_pilot_block_t_sz(pilot_block_t_sz), d_pilot_block_rep(
				pilot_block_rep), d_data_block_idx(0), d_data_block_sz(
				data_block_sz), d_data_block_rep(data_block_rep), d_silence_block_idx(
				0), d_silence_block_rep(silence_block_rep), d_cfo_next_preamble_distance(
				cfo_next_preamble_distance), d_cfo_sync_method(cfo_sync_method), d_window_sz(
				fft_length / 2), d_rx_preamble_t_idx(0), d_symbol_count(0), d_state(
				ST_IDLE), d_phase_shift(0.0f), d_est_SNR(0.0f), d_signal_power(
				1.0f), d_noise_power(1.0f), d_cfo_idx(0), d_cfo_arg(0), d_cfo_val(
				gr_complex(0, 0)), ax(0.0f), ay(0.0f), yt(0.0f), xt(0.0f), syy(
				0.0f), sxy(0.0f), sxx(0.0f), d_timeout_ms(time_out), d_avg_phase_shift(0.0f),
				d_phase_shift_idx(0), ni_init(0)
{
	int i = 0;
	printf("vfdm_sync::fft_length %d\n", fft_length);
	printf("vfdm_sync::occupied_tones %d\n", occupied_tones);
	printf("vfdm_sync::cp_length %d\n", cp_length);
	printf("vfdm_sync::precoder_length %d\n", precoder_length);
	printf("vfdm_sync::preamble_block_rep %d\n", preamble_block_rep);
	printf("vfdm_sync::pilot_block_t_sz %d\n", pilot_block_t_sz);
	printf("vfdm_sync::pilot_block_rep %d\n", pilot_block_rep);
	printf("vfdm_sync::data_block_sz %d\n", data_block_sz);
	printf("vfdm_sync::data_block_rep %d\n", data_block_rep);
	printf("vfdm_sync::silence_block_rep %d\n", silence_block_rep);
	printf("vfdm_sync:: cfo_sync_method %d\n", cfo_sync_method);
	printf("vfdm_sync::cfo_next_preamble_distance %d\n",
			cfo_next_preamble_distance);

	d_frame_length = (preamble_block_rep + pilot_block_t_sz * pilot_block_rep
			+ data_block_sz * data_block_rep + silence_block_rep)
			* (fft_length + precoder_length);
	d_preamble_block_total = preamble_block_rep* (fft_length + precoder_length);
	d_pilot_block_total = pilot_block_t_sz * pilot_block_rep* (fft_length + precoder_length);
	d_data_block_total = data_block_sz * data_block_rep* (fft_length + precoder_length);
	d_silence_block_total = silence_block_rep * (fft_length + precoder_length);

	if (preamble_block_rep % 2 != 0 && preamble_block_rep != 1)
	{
		printf("init::vfdm_sybc::preamble_block_rep");
		printf("should be an even number ");
		printf("or 1 [1 2 4 ... ]\n");
		throw std::runtime_error("");
	}

	if (cfo_next_preamble_distance % 2 != 0
			&& cfo_next_preamble_distance > preamble_block_rep)
	{
		printf("init::vfdm_sybc::cfo_next_preamble_distance");
		printf("mod 2 != 0 && cfo_next_preamble_distance");
		printf("> preamble_block_rep]\n");
		throw std::runtime_error("");
	}

	d_cfo_preamble_jump = d_preamble_block_rep / cfo_next_preamble_distance;
	d_cfo_step_sz = (d_preamble_block_rep) / d_cfo_preamble_jump;
	d_cfo_window_sz = cfo_next_preamble_distance * d_window_sz;

	d_left_padding = floor((fft_length - occupied_tones) / 2);

	// Load FFTW lib for IFFT
	d_ifft = new fft::fft_complex(fft_length, false);

	// Load FFTW lib for FFT (If Schmidl-Cox method for CFO is enabled)
	if (cfo_sync_method == 0)
	{
		d_fft = new fft::fft_complex(fft_length / 2, true);
		d_rx_preamble_f1 = new gr_complex[fft_length / 2];
		d_rx_preamble_f2 = new gr_complex[fft_length / 2];
		g_span1 = -4;
		g_span2 = +4;
		g = 1;
		B1 = 0.0;
		B2 = 0.0;
		B = new double[4 + 4 + 1];
	}

	// Init CFO vector
	d_cfo.resize(d_frame_length);
	std::fill(d_cfo.begin(), d_cfo.end(), 0);

	// Init TX preamble sequence
	std::vector<gr_complex> preamble = generate_complex_sequence(fft_length,
			d_left_padding, 1, 0, false);
	d_tx_preamble_f = preamble;

	std::vector<gr_complex> preamble_t = generate_complex_sequence_in_time(
			fft_length, d_left_padding, 1, 0, false);
	d_tx_preamble_t = preamble_t;

	// Init RX preamble sequence
	d_rx_preamble_t.resize(preamble_block_rep * fft_length);
	std::fill(d_rx_preamble_t.begin(), d_rx_preamble_t.end(), 0);

	// Calculate mean of preamble sequence
	ax = 0.0f;
	for (i = 0; i < fft_length; i++)
	{
		ax += abs(preamble_t[i]);
	}
	ax /= fft_length;
	
	// Set timeout values
	d_timeout_syms_total = d_frame_length;
	d_timeout_syms_idx = 0;
	d_timeout_ref_sym = gr_complex(TINY, TINY);
	d_clock = clock();
	d_clock_diff = d_clock;

	printf("mean(ax) = %.4f\n", ax);

	printf("init::vfdm_sync\n");
	set_output_multiple(d_frame_length);
}

/*
 * Our virtual destructor.
 */
vfdm_sync_impl::~vfdm_sync_impl()
{
}

std::vector<gr_complex> vfdm_sync_impl::generate_complex_sequence(int seq_size,
		int left_padding, int zeros_period, int seed = 0, bool verbose = false)
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

	// Print results
	if (verbose == true)
	{
		printf("sync_preamble_f = [");
		for (i = 0; i < seq_size; i++)
		{
			gr_complex sym = result[i];
			printf("%.4f+1i*(%.4f), ", sym.real(), sym.imag());
		}
		printf("]\n\n");
	}
	return result;
}

std::vector<gr_complex> vfdm_sync_impl::generate_complex_sequence_in_time(
		int seq_size, int left_padding, int zeros_period, int seed = 0,
		bool verbose = false)
{
	std::vector<gr_complex> result = generate_complex_sequence(seq_size,
			left_padding, zeros_period, seed, verbose);

	std::vector<gr_complex> result_t(seq_size, gr_complex(0, 0));

	memcpy(d_ifft->get_inbuf(), &result[0], d_fft_length * sizeof(gr_complex));

	// Copy symbols to input buffer (with FFT shift)
	gr_complex *dst = d_ifft->get_inbuf();
	// half length of complex array
	unsigned int len = (unsigned int) (floor(d_fft_length / 2.0));
	memcpy(&dst[0], &result[len], sizeof(gr_complex) * (d_fft_length - len));
	memcpy(&dst[d_fft_length - len], &result[0], sizeof(gr_complex) * len);

	// Execute FFT
	d_ifft->execute();

	// Get results
	memcpy(&result_t[0], d_ifft->get_outbuf(),
			sizeof(gr_complex) * d_fft_length);

	// Print results
	if (verbose == true)
	{
		int i = 0;
		printf("sync_preamble_t = [");
		for (i = 0; i < d_fft_length; i++)
		{
			gr_complex sym = result_t[i];
			printf("%.4f+1i*(%.4f), ", sym.real(), sym.imag());
		}
		printf("]\n\n");
	}
	return result_t;
}

void vfdm_sync_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required)
{
	int coeff = (noutput_items - (noutput_items % d_frame_length))
		/ d_frame_length;
	ninput_items_required[0] = d_fft_length * coeff;
}

int vfdm_sync_impl::estimate_cfo()
{

	double phase_shift = 0.0f;
	double ps_arg1 = 0.0f;
	double ps_arg2 = 0.0f;

	int i = 0;
	int j = 0;
	int d = 0;
	int e = 0;
	int idx1 = 0;
	int idx2 = 0;

	d_phase_shift = 0;
	for (i = 0; i < d_cfo_preamble_jump; i++)
	{
		for (j = 0; j < d_cfo_step_sz; j++)
		{
			idx1 = i * d_fft_length + j * d_window_sz;
			idx2 = idx1 + d_cfo_next_preamble_distance * d_window_sz;

			phase_shift = 0.0f;
			for (d = 0; d < d_window_sz; d++)
			{
				ps_arg2 = arg(d_rx_preamble_t[idx2 + d]);

				phase_shift += abs(arg((d_rx_preamble_t[idx2 + d])*
					conj(d_rx_preamble_t[idx1 + d])))/ ((double) d_window_sz);
			}
			d_phase_shift += phase_shift;
			e++;
		}
	}
	d_phase_shift /= (double) (e * d_window_sz);
	return 0;
}

int vfdm_sync_impl::estimate_cfo_schmidl_cox()
{
	int m = 0;
	gr_complex P_est = gr_complex(0, 0);
	double cfo_coarse = 0.0f;
	for (m = 0; m < d_window_sz; m++)
	{
		gr_complex s1 = d_rx_preamble_t[m];
		gr_complex s2 = d_rx_preamble_t[m + d_window_sz];
		P_est += conj(s1) * s2;
	}
	//printf("VFDM d_rx_preamble_t[m/2] = %.4f + i*%.4f \n",d_rx_preamble_t[m/2].real(),d_rx_preamble_t[m/2].imag());
	//printf("VFDM m = %i, d_window_sz = %i \n",m,d_window_sz);
	//printf("VFDM P_est = %.4f + i*%.4f\n ",P_est.real(),P_est.imag());
	d_phase_shift = ((-2.0l * arg(P_est)) / ((double) d_fft_length));
	
	/*
	 If abs(arg(P_est)) > PI, we should estimate also the
	 integer "z"

	 See also Eq. 40 in the paper:
	 Timothy M. Schmidl and Donald,
	 "Robust Frequency and TimingSynchronization for OFDM",
	 IEEE TRANSACTIONS ON COMMUNICATIONS, VOL. 45, NO. 12,
	 DECEMBER 1997
	 */
	if (abs(arg(P_est)) > M_PI)
	{
		int i = 0;
		int j = 0;

		// Eliminate the coarse frequency shift
		for (i = 0; i < d_fft_length; i++)
		{
			d_cfo_val = std::polar(1.0, double(i) * d_phase_shift);
			d_rx_preamble_t[i] = d_cfo_val * d_rx_preamble_t[i];
		}

		/* FFT for 1st half of the preamble */
		unsigned int len = (unsigned int) (ceil(d_window_sz / 2.0));

		// Copy symbols to the input buffer
		memcpy(d_fft->get_inbuf(), &d_rx_preamble_t[0],
				d_window_sz * sizeof(gr_complex));
		d_fft->execute();
		memcpy(&d_rx_preamble_f1[0], d_fft->get_outbuf(),
				sizeof(gr_complex) * d_window_sz);
		memcpy(&d_rx_preamble_f1[0], &d_fft->get_outbuf()[len],
				sizeof(gr_complex) * (d_window_sz - len));
		memcpy(&d_rx_preamble_f1[d_window_sz - len], &d_fft->get_outbuf()[0],
				sizeof(gr_complex) * len);

		/* FFT for 2nd half of the preamble */
		memcpy(d_fft->get_inbuf(), &d_rx_preamble_t[d_window_sz],
				d_window_sz * sizeof(gr_complex));
		d_fft->execute();
		memcpy(&d_rx_preamble_f2[0], d_fft->get_outbuf(),
				sizeof(gr_complex) * d_window_sz);
		memcpy(&d_rx_preamble_f2[0], &d_fft->get_outbuf()[len],
				sizeof(gr_complex) * (d_window_sz - len));
		memcpy(&d_rx_preamble_f2[d_window_sz - len], &d_fft->get_outbuf()[0],
				sizeof(gr_complex) * len);

		int jstart = d_left_padding / 2 - 1;
		int jend = jstart + d_occupied_tones / 2;
		g = 0;
		Bmax = 0;
		g_est = 0;
		for (i = g_span1; i < g_span2 + 1; i++)
		{
			B1 = 0.0f;
			B2 = 0.0f;
			for (j = jstart; j < jend; j += 2)
			{
				B1 += (conj(d_rx_preamble_f1[j + i]) * d_rx_preamble_f2[j + i]);
				B2 += abs(d_rx_preamble_f1[j] * d_rx_preamble_f1[j]);
			}
			B[g] = (abs(B1) * abs(B1)) / (2 * (abs(B2) * abs(B2)));
			g++;
		}

		for (i = 0; i < 9; i++)
		{
			if (B[i] > Bmax)
			{
				Bmax = B[i];
				Bidx = i;
			}
		}
		g_est = i + g_span1 - 5;

		printf("B = [");
		for (i = 0; i < 9; i++)
		{
			printf("%.4f, ", B[i]);
		}
		printf("];\n");

		printf("Bmax = %.4f, g_est = %d \n", Bmax, g_est);

		printf("Scmidl-Cox CFO: %.8f ps_c = %.4f, ", arg(P_est), d_phase_shift);
		d_phase_shift = (-2.0 * M_TWOPI)
				* ((arg(P_est) / (double(d_fft_length) * M_TWOPI))
						+ ((2 * double(g_est)) / double(d_fft_length)));
		printf(" ps_f = %.4f \n", d_phase_shift);

		printf("rx_preamble_t = [");
		for (i = 0; i < d_fft_length; i++)
		{
			gr_complex s1 = d_rx_preamble_t[i];
			printf("%.4f+1i*(%.4f), ", s1.real(), s1.imag());
		}
		printf("]\n");

		printf("rx_preamble_f1 = [");
		for (i = 0; i < d_window_sz; i++)
		{
			gr_complex s1 = d_rx_preamble_f1[i];
			printf("%.4f+1i*(%.4f), ", s1.real(), s1.imag());
		}
		printf("]\n");

		printf("rx_preamble_f2 = [");
		for (i = 0; i < d_window_sz; i++)
		{
			gr_complex s1 = d_rx_preamble_f2[i];
			printf("%.4f+1i*(%.4f), ", s1.real(), s1.imag());
		}
		printf("]\n");

		throw std::runtime_error(
				"ofdm_sync:: This part is not implemented fully: abs(arg(P_est)) > M_P!");
	}
	//printf("VFDM Scmidl-Cox CFO: %.8f ps = %.4f \n ",arg(P_est),d_phase_shift);
	return 0;
}

int vfdm_sync_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items_v, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	int ninput_items = ninput_items_v[0];
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	int ni = 0;
	int no = 0;
	int next_turn = 0;
	int d = 0;
	int m = 0;
	//int ni_init = 0;

	int window_max = 0;
	bool coarse_ok = false;

	double phase_diff = 0.0f;
	double phase_diff_max = 0.0f;

	double phase_shift = 0.0f;

	double ps_arg1 = 0.0f;
	double ps_arg2 = 0.0f;

	double rc = 0.0f; // Correlation coefficient (for fine timing)
	double rc_best = 0.0f; // Best correlation coefficient  (for fine timing)

	while (ni < ninput_items && no < noutput_items && next_turn == 0)
	{
		switch (d_state)
		{
		case ST_IDLE:
		{
			window_max = ninput_items - ni;
			// We need to wait until we receive enough symbols
			if (window_max > d_fft_length)
			{
				d_state = ST_COARSE_SYNC;
				//printf("Idle->Coarse\n");
			}
			else
			{
				next_turn = 1;
				d_state = ST_IDLE;
				//printf("Idle->Idle\n");
			}
			break;
		}
			/************************************************************
			 Coarse Synhronization (using Schmidl-Cox Method)
			 ************************************************************/
		case ST_COARSE_SYNC:
		{
			coarse_ok = false;
			idx_coarse = ni;
			for (d = 0; d < d_window_sz; d++)
			{
				P = 0.0;
				R = 0.0;
				M = 0.0;
				for (m = 0; m < d_window_sz; m++)
				{
					gr_complex s1 = in[ni + d + m];
					gr_complex s2 = in[ni + d + m + d_window_sz];
					P += conj(s1) * s2;
					R += abs(s2) * abs(s2);
				}
				M = abs(P) * abs(P) / (R * R + 1);
				/*
				 If we find the threshold, we don't need to process
				 rest of symbols for coarse estimation. We basically
				 escape in order to start fine sychronization process
				 as soon as possible. */
				if (M > 0.8)
				{
					ni = ni + d;
					ni_init = ni;
					coarse_ok = true;
					break;
				}
			}

			/*for (d = 0; d < d_fft_length; d++)
			{
				gr_complex s1 = in[ni + d];
			}*/

			if (coarse_ok == true)
			{
				//printf("M = %f, coarse time sync ok \n", M);
				d_state = ST_FINE_SYNC;
			}
			else
			{
				ni += d_window_sz;
				d_state = ST_IDLE;
				//Check timeout
				/*d_clock_diff = (clock() - d_clock) / CLOCKS_PER_MSEC;
				if (d_clock_diff > d_timeout_ms)
				{
					d_state = ST_TIMEOUT;
				}
				else
				{
					d_state = ST_IDLE;
				}*/
			}
			break;
		}
			/************************************************************
			 Fine Timing Synchronization (using Phase correlation)
			 ************************************************************/
		case ST_FINE_SYNC:
		{
			idx_fine = ni;

			// Calculate mean of sequence
			ay = 0.0f;
			rc = 0.0f;
			rc_best = 0.0f;

			for (d = 0; d < d_fft_length; d++)
			{
				ay += abs(in[ni + d]);
			}
			ay /= d_fft_length;

			// Find the best correlation coefficient
			//printf("fine_corr = [");
			for (d = 0; d < d_window_sz; d++)
			{
				yt = 0;
				xt = 0;
				syy = 0;
				sxy = 0;
				sxx = 0;

				for (m = 0; m < d_window_sz; m++)
				{
					xt = abs(d_tx_preamble_t[m]) - ax;
					yt = abs(in[ni + d + m]) - ay;
					sxx += xt * xt;
					syy += yt * yt;
					sxy += xt * yt;
				}
				rc = sxy / (sqrt(sxx * syy) + TINY);
				rc = abs(rc);
				//printf("%.4f, ", abs(rc));
				if (rc > rc_best)
				{
					rc_best = rc;
					idx_fine = ni + d;
				}
			}

			ni = idx_fine;

			/*for (d = 0; d < d_fft_length; d++)
			{
				gr_complex s1 = in[ni + d];
			}*/

			/*d_phase_shift = 0;
			d_preamble_block_idx = 0;
			d_rx_preamble_t_idx = 0;
			d_symbol_count = 0;
			d_state = ST_PREAMBLE;*/

			// We lost first cyclic prefix part because of sync. Let's
			// send some redundant silence symbols for next block
			// -cyclic prefix removal- in order to keep it working.
			for (d = 0; d < d_cp_length; d++)
			{
				out[no] = gr_complex(0, 0);
				no++;
			}
			d_phase_shift = 0;
			d_preamble_block_idx = d_cp_length;
			d_rx_preamble_t_idx = 0;
			d_symbol_count = d_cp_length;
			d_state = ST_PREAMBLE;
			break;
		}
			/************************************************************
			 Preamble
			 ************************************************************/
		case ST_PREAMBLE:
		{
			if (d_preamble_block_idx < (d_preamble_block_total))
			{
				d_cfo_arg = d_cfo_idx * d_phase_shift;
				//d_cfo_val = gr_complex(cos(d_cfo_arg), sin(d_cfo_arg));
				d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
				out[no] = d_cfo_val * in[ni]; // Send it to output
				

				
				if ((d_preamble_block_idx % (d_fft_length + d_precoder_length))
						>= d_cp_length)
				{
					// Save value for CFO estimation
					d_rx_preamble_t[d_rx_preamble_t_idx] = in[ni];
					d_rx_preamble_t_idx++;
					//gr_complex sym = d_cfo_val * in[ni];
					//printf("%i = (%.4f, %.4f), ", d_preamble_block_idx % (d_fft_length + d_precoder_length) - d_cp_length, sym.real(), sym.imag());
				}

				d_symbol_count++;
				d_cfo_idx++;
				ni++;
				no++;
				d_preamble_block_idx++;
				//if ((d_preamble_block_idx % (d_fft_length + d_precoder_length)) == 0) printf("}\n\n");
				d_state = ST_PREAMBLE;
			}
			else
			{
				d_preamble_block_idx = 0;
				if (d_cfo_sync_method == 0)
				{
					d_state = ST_CFO_SCHMIDL_COX;
				}
				else
				{
					d_state = ST_CFO;
				}
			}
			break;
		}
			/******************************************************************
			 Carrier Frequency Offset Estimation (Schmidl-Cox)
			 *****************************************************************/
		case ST_CFO_SCHMIDL_COX:
		{
			estimate_cfo_schmidl_cox();
			d_avg_phase_shift += d_phase_shift;
			d_phase_shift_idx ++;
			/*if (d_phase_shift_idx == 10)
			{
				printf("VFDM Scmidl-Cox Average Phase Shift = %.4f \n ",d_avg_phase_shift/10);
				d_phase_shift_idx = 0;
				d_avg_phase_shift = 0;
			}*/
			d_state = ST_PILOT;
			break;
		}
			/******************************************************************
			 Carrier Frequency Offset Estimation (Phase Averaging)
			 *****************************************************************/
		case ST_CFO:
		{
			estimate_cfo();
			d_state = ST_PILOT;
			break;
		}
			/************************************************************
			 Pilot
			 ***********************************************************/
		case ST_PILOT:
		{
			if (d_pilot_block_idx < d_pilot_block_total)
			{
				d_cfo_arg = d_cfo_idx * d_phase_shift;
				//d_cfo_val = gr_complex(cos(d_cfo_arg), sin(d_cfo_arg));
				d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
				out[no] = d_cfo_val * in[ni];

				d_symbol_count++;
				d_cfo_idx++;
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
		}
			/************************************************************
			 Data
			 ************************************************************/
		case ST_DATA:
		{
			if (d_data_block_idx < d_data_block_total)
			{
				d_cfo_arg = d_cfo_idx * d_phase_shift;
				//d_cfo_val = gr_complex(cos(d_cfo_arg), sin(d_cfo_arg));
				d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
				out[no] = d_cfo_val * in[ni];

				d_symbol_count++;
				d_cfo_idx++;
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
		}
			/************************************************************
			 Silence
			 ************************************************************/
		case ST_SILENCE:
		{
			if (d_silence_block_idx < d_silence_block_total)
			{
				d_cfo_arg = d_cfo_idx * d_phase_shift;
				//d_cfo_val = gr_complex(cos(d_cfo_arg), sin(d_cfo_arg));
				d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
				out[no] = d_cfo_val * in[ni];
				//printf("%.4f+%.4fj, ", in[ni].real(), in[ni].imag());

				d_symbol_count++;
				d_cfo_idx++;
				ni++;
				no++;
				d_silence_block_idx++;
				d_state = ST_SILENCE;
			}
			else
			{	d_clock = clock();
				d_symbol_count = 0;
				d_cfo_idx = 0;
				d_silence_block_idx = 0;
				//printf ("ni = %i, ni_init = %i\n",ni,ni_init);
				//ni = ni_init;
				d_state = ST_IDLE;
			}
			//printf("\n");
			break;
		}
			/************************************************************
			 Timeout
			 ************************************************************/
		case ST_TIMEOUT:
		{
			//printf("Time OUT\n");
			if (d_timeout_syms_idx < d_timeout_syms_total)
			{
				//out[no] = d_timeout_ref_sym;
				//no++;
				d_timeout_syms_idx++;
			}
			else
			{
				d_clock = clock();
				d_timeout_syms_idx = 0;
				d_state = ST_IDLE;
			}
			break;
		}
		default:
			throw std::invalid_argument("vfdm_sync::invalid state");
			break;

		}
	}
	consume_each(ni);
	return no;
}

} /* namespace s4a */
} /* namespace gr */
