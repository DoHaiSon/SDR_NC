
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "End_Node_PNC_Controller_impl.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <complex.h>
#include <math.h>
#include <time.h>

#define M_TWOPI (2*M_PI)
#define TINY    1.0e-20
#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000)

namespace gr {
  namespace PNC4TWRN {

    End_Node_PNC_Controller::sptr
    End_Node_PNC_Controller::make(int fft_length, int occupied_tones, int cp_length, int node)
    {
      return gnuradio::get_initial_sptr
        (new End_Node_PNC_Controller_impl(fft_length, occupied_tones, cp_length,node));
    }

    /*
     * The private constructor
     */
    End_Node_PNC_Controller_impl::End_Node_PNC_Controller_impl(int fft_length, int occupied_tones, int cp_length, int node)
      : gr::block("End_Node_PNC_Controller",
              gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(unsigned char)),
              gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(unsigned char))),
		d_fft_length(fft_length), d_occupied_tones(occupied_tones), d_cp_length(cp_length),
		d_window_sz(fft_length / 2), d_phase_shift(0.0f), d_cfo_val(gr_complex(0, 0)),
		d_preamble_block_idx(0), d_pilot_block_idx(0), d_data_block_idx(0), d_rx_preamble_t_idx(0),
		d_timeout_ms(1000),  ax(0.0f), ay(0.0f), yt(0.0f), xt(0.0f), syy(0.0f), sxy(0.0f), sxx(0.0f),
		d_cfo_idx(0), d_state(ST_IDLE)
    {
		d_left_padding = floor((fft_length - occupied_tones) / 2);
		int i = 0;

		d_preamble_block_total = fft_length + cp_length;
		d_pilot_block_total = fft_length + cp_length;
		d_data_block_total = fft_length + cp_length;

		// Init FFT
		d_fft = new gr::fft::fft_complex(fft_length, true);
		d_ifft = new gr::fft::fft_complex(fft_length, false);

		// Init TX preamble sequence
		std::vector<gr_complex> preamble = generate_complex_sequence(fft_length,
				d_left_padding, 1, 0);
		d_tx_preamble_f = preamble;

		std::vector<gr_complex> preamble_t = generate_complex_sequence_in_time(
				fft_length, d_left_padding, 1, 0);
		d_tx_preamble_t = preamble_t;

		// Init pilot sequence
		std::vector<gr_complex> pilot = generate_complex_sequence(fft_length,
				d_left_padding, 0, 0);
		d_tx_pilot = pilot;

		// Init RX preamble, pilot, and header sequence
		d_rx_preamble_t.resize(fft_length);
		std::fill(d_rx_preamble_t.begin(), d_rx_preamble_t.end(), gr_complex(0,0));

		d_rx_pilot_t.resize(fft_length);
		std::fill(d_rx_pilot_t.begin(), d_rx_pilot_t.end(), gr_complex(0,0));

		d_rx_header_t.resize(fft_length);
		std::fill(d_rx_header_t.begin(), d_rx_header_t.end(), gr_complex(0,0));

		d_rx_pilot_f.resize(fft_length);
		std::fill(d_rx_pilot_f.begin(), d_rx_pilot_f.end(), gr_complex(0,0));

		d_rx_header_f.resize(fft_length);
		std::fill(d_rx_header_f.begin(), d_rx_header_f.end(), gr_complex(0,0));

		d_rx_header.resize(occupied_tones);
		std::fill(d_rx_header.begin(), d_rx_header.end(), gr_complex(0,0));

		// Init h_est
		d_h_est.resize(occupied_tones);
		std::fill(d_h_est.begin(), d_h_est.end(), gr_complex(1, 1));

		// Calculate mean of preamble sequence
		ax = 0.0f;
		for (i = 0; i < fft_length; i++)
		{
			ax += abs(preamble_t[i]);
		}
		ax /= fft_length;

		// For Schmidl-Cox sync method
		d_fft_4sync = new fft::fft_complex(fft_length / 2, true);
		d_rx_preamble_f1 = new gr_complex[fft_length / 2];
		d_rx_preamble_f2 = new gr_complex[fft_length / 2];
		g_span1 = -4;
		g_span2 = +4;
		g = 1;
		B1 = 0.0;
		B2 = 0.0;
		B = new double[4 + 4 + 1];

		// Set timeout values
		d_timeout_syms_total = 3*(fft_length+cp_length);
		d_timeout_syms_idx = 0;
		d_timeout_ref_sym = gr_complex(TINY, TINY);
		d_clock = clock();
		d_clock_diff = d_clock;
    }

    /* Generate Complex Sequence */
    std::vector<gr_complex> End_Node_PNC_Controller_impl::generate_complex_sequence(
			int seq_size, int left_padding, int zeros_period, int seed)
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

   /* Generate Complex Sequence in Time */
   std::vector<gr_complex> End_Node_PNC_Controller_impl::generate_complex_sequence_in_time(
			int seq_size, int left_padding, int zeros_period, int seed)
   {
		std::vector<gr_complex> result = generate_complex_sequence(
				seq_size, left_padding, zeros_period, seed);

		std::vector<gr_complex> result_t(seq_size, gr_complex(0, 0));

		memcpy(d_ifft->get_inbuf(), &result[0], d_fft_length * sizeof(gr_complex));

		// Copy symbols to input buffer (with FFT shift)
		gr_complex *dst = d_ifft->get_inbuf();
		unsigned int len = (unsigned int) (floor(d_fft_length / 2.0));
		memcpy(&dst[0], &result[len], sizeof(gr_complex) * (d_fft_length - len));
		memcpy(&dst[d_fft_length - len], &result[0], sizeof(gr_complex) * len);

		// Execute FFT
		d_ifft->execute();

		// Get results
		memcpy(&result_t[0], d_ifft->get_outbuf(), sizeof(gr_complex) * d_fft_length);

		return result_t;
	}

   /* FFT */
   std::vector<gr_complex> End_Node_PNC_Controller_impl::FFT_Func(std::vector<gr_complex> input, bool forward)
   {
		std::vector<gr_complex> out(d_fft_length, gr_complex(0, 0));
		if(!forward)
		{
			// Copy symbols to input buffer (with IFFT shift)
			memcpy(d_ifft->get_inbuf(), &input[0], d_fft_length * sizeof(gr_complex));
			gr_complex *dst = d_ifft->get_inbuf();
			unsigned int len = (unsigned int) (floor(d_fft_length/2.0));
			memcpy(&dst[0], &input[len], sizeof(gr_complex) * (d_fft_length - len));
			memcpy(&dst[d_fft_length - len], &input[0], sizeof(gr_complex) * len);

			// Execute IFFT
			d_ifft->execute();

			// Get results
			memcpy(&out[0], d_ifft->get_outbuf(), sizeof(gr_complex) * d_fft_length);
		}
		else
		{
			// Copy symbols to input buffer
			memcpy(d_fft->get_inbuf(), &input[0], d_fft_length * sizeof(gr_complex));

			// Execute FFT
			d_fft->execute();

			// Get results (with FFT shift)
			unsigned int len = (unsigned int)(ceil(d_fft_length/2.0));
			memcpy(&out[0], &d_fft->get_outbuf()[len], sizeof(gr_complex)*(d_fft_length - len));
			memcpy(&out[d_fft_length - len], &d_fft->get_outbuf()[0], sizeof(gr_complex)*len);
		}
		return out;
    }

    /* CP Insertion */
    std::vector<gr_complex> End_Node_PNC_Controller_impl::CP_Insertion(std::vector<gr_complex> symbols_in)
    {
		int out_length = d_fft_length + d_cp_length;
		std::vector<gr_complex> symbols_out(out_length, gr_complex(0, 0));
		memcpy(&symbols_out[0], &symbols_in[d_fft_length - d_cp_length], d_cp_length * sizeof(gr_complex));
		memcpy(&symbols_out[d_cp_length], &symbols_in[0], d_fft_length * sizeof(gr_complex));
		return symbols_out;
    }

    /* CP Removal */
    std::vector<gr_complex> End_Node_PNC_Controller_impl:: CP_Removal(std::vector<gr_complex> symbols_in)
    {
		std::vector<gr_complex> symbols_out(d_fft_length, gr_complex(0, 0));
		memcpy(&symbols_out[0], &symbols_in[d_cp_length], d_fft_length * sizeof(gr_complex));
		return symbols_out;
    }

    /* Byte-to-Bit Conversion */
    std::vector<unsigned char> End_Node_PNC_Controller_impl::Byte_to_Bit(unsigned char byte_in)
    {
		std::vector<unsigned char> bits_out(8, 0x00);
		for (int i = 0; i < 8; i++)
		{
			bits_out[i] = ((byte_in >> i) & 0x1);
		}
		return bits_out;
    }

    /* Bit-to-Byte Conversion */
    unsigned char End_Node_PNC_Controller_impl::Bit_to_Byte(std::vector<unsigned char> bits_in)
    {
		unsigned char byte_out;
		for (int i = 0; i<8; i++)
		{
			byte_out |= ((bits_in[i] & 0x1) << i);
		}
		return byte_out;
    }

    /* BPSK Mapper*/
    std::vector<gr_complex> End_Node_PNC_Controller_impl::BPSK_Mapper(std::vector<unsigned char> bits_in)
    {
		std::vector<gr_complex> symbols_out(8, gr_complex(0, 0));
		std::vector<gr_complex> symbols(2, gr_complex(0, 0));
		symbols[0] = gr_complex(-1, 0); 
		symbols[1] = gr_complex(+1, 0);
		for (int i = 0; i<8; i++)
		{
			int bit = bits_in[i] & 0x1;
			symbols_out[i] = symbols[bit];
		}
		return symbols_out;
    }

    /* BPSK Demapper */
    unsigned char End_Node_PNC_Controller_impl::BPSK_Demapper(std::vector<gr_complex> symbols_in)
    {
		unsigned char byte_out = 0x00;
		for (int i = 0; i<8; i++)
		{
			if (symbols_in[i].real() > 0)
			{
				byte_out |= (0x1 << i);
			}
		}
		return byte_out;
    }

	/* Schmidl-Cox Synchronization Method */
	int End_Node_PNC_Controller_impl::estimate_cfo_schmidl_cox()
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

			// FFT for 1st half of the preamble
			unsigned int len = (unsigned int) (ceil(d_window_sz / 2.0));

			// Copy symbols to the input buffer
			memcpy(d_fft_4sync->get_inbuf(), &d_rx_preamble_t[0],
					d_window_sz * sizeof(gr_complex));
			d_fft_4sync->execute();
			memcpy(&d_rx_preamble_f1[0], d_fft_4sync->get_outbuf(),
					sizeof(gr_complex) * d_window_sz);
			memcpy(&d_rx_preamble_f1[0], &d_fft_4sync->get_outbuf()[len],
					sizeof(gr_complex) * (d_window_sz - len));
			memcpy(&d_rx_preamble_f1[d_window_sz - len], &d_fft_4sync->get_outbuf()[0],
					sizeof(gr_complex) * len);

			// FFT for 2nd half of the preamble
			memcpy(d_fft_4sync->get_inbuf(), &d_rx_preamble_t[d_window_sz],
					d_window_sz * sizeof(gr_complex));
			d_fft_4sync->execute();
			memcpy(&d_rx_preamble_f2[0], d_fft_4sync->get_outbuf(),
					sizeof(gr_complex) * d_window_sz);
			memcpy(&d_rx_preamble_f2[0], &d_fft_4sync->get_outbuf()[len],
					sizeof(gr_complex) * (d_window_sz - len));
			memcpy(&d_rx_preamble_f2[d_window_sz - len], &d_fft_4sync->get_outbuf()[0],
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

			d_phase_shift = (-2.0 * M_TWOPI)
					* ((arg(P_est) / (double(d_fft_length) * M_TWOPI))
							+ ((2 * double(g_est)) / double(d_fft_length)));

			throw std::runtime_error(
					"ofdm_sync:: This part is not implemented fully: abs(arg(P_est)) > M_P!");
		}

		return 0;
    }

    /*
     * Our virtual destructor.
     */
    End_Node_PNC_Controller_impl::~End_Node_PNC_Controller_impl()
    {
    }

    void
    End_Node_PNC_Controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    End_Node_PNC_Controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
		int nInputItems = ninput_items[0];
		int nOutputItems = noutput_items;

        const gr_complex *in0 = (const gr_complex *) input_items[0];
        const unsigned char *in1 = (const unsigned char *) input_items[1];
        gr_complex *out0 = (gr_complex *) output_items[0];
        unsigned char *out1 = (unsigned char *) output_items[1];

		int ni0 = 0;
		int ni1 = 0;
		int no = 0;

		int next_turn = 0;
		int d = 0;
		int m = 0;

		int window_max = 0;
		bool coarse_ok = false;

		double phase_diff = 0.0f;
		double phase_diff_max = 0.0f;

		double phase_shift = 0.0f;

		double ps_arg1 = 0.0f;
		double ps_arg2 = 0.0f;

		// Correlation coefficient (for fine timing)
		double rc = 0.0f;

		// Best correlation coefficient  (for fine timing)
		double rc_best = 0.0f;

		while(ni0<nInputItems && next_turn == 0)
		{
			switch (d_state)
			{
				case ST_IDLE:
				{
					window_max = nInputItems - ni0;
					// We need to wait until we receive enough symbols
					if (window_max > d_fft_length)
					{
						d_state = ST_COARSE_SYNC;
					}
					else
					{
						next_turn = 1;
						d_state = ST_IDLE;
					}
					break;
				}
					/*****************************************************
					 Coarse Syncronization (using Schimdl-Cox Method)
					 ****************************************************/
				case ST_COARSE_SYNC:
				{
					coarse_ok = false;
					idx_coarse = ni0;
					for (d = 0; d < d_window_sz; d++)
					{
						P = gr_complex(0,0);
						R = 0.0;
						M = 0.0;
						for (m = 0; m < d_window_sz; m++)
						{
							gr_complex s1 = in0[ni0 + d + m];
							gr_complex s2 = in0[ni0 + d + m + d_window_sz];
							P += conj(s1) * s2;
							R += abs(s2) * abs(s2);
						}
						M = abs(P) * abs(P) / (R * R + 1);
						/*
						 If we find the threshold, we don't need to process rest of
						 symbols for coarse estimation. We basically escape in order
						 to start fine sychronization process as soon as possible.
						 */
						if (M > 0.8)
						{
							ni0 = ni0 + d;
							coarse_ok = true;
							break;
						}
					}

					if (coarse_ok == true)
					{
						std::cout<<"coarse sync ok\n";
						d_state = ST_FINE_SYNC;
					}
					else
					{
						ni0 += d_window_sz;

						// Check timeout
						d_clock_diff = (clock() - d_clock) / CLOCKS_PER_MSEC;
						if (d_clock_diff > d_timeout_ms)
						{
							d_state = ST_TIMEOUT;
						}
						else
						{
							d_state = ST_IDLE;
						}
					}
					break;
				}
					/*****************************************************
					 Fine Timing Synhronization (using Phase correlation)
					 ****************************************************/
				case ST_FINE_SYNC:
				{
					idx_fine = ni0;

					// Calculate mean of sequence
					ay = 0.0f;
					rc = 0.0f;
					rc_best = 0.0f;

					for (d = 0; d < d_fft_length; d++)
					{
						ay += abs(in0[ni0 + d]);
					}
					ay /= d_fft_length;

					// Find the best correlation coefficient
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
							yt = abs(in0[ni0 + d + m]) - ay;
							sxx = sxx + xt * xt;
							syy = syy + yt * yt;
							sxy = sxy + xt * yt;
						}
						rc = sxy / (sqrt(sxx * syy) + TINY);
						rc = abs(rc);
						if (rc > rc_best)
						{
							rc_best = rc;
							idx_fine = ni0 + d;
						}
					}

					ni0 = idx_fine;

					// We lost first cyclic prefix part because of sync. Let's
					// send some redundant silence symbols for next block
					// -cyclic prefix removal- in order to keep it working.
					for (d = 0; d < d_cp_length; d++)
					{
						//out[no] = gr_complex(0, 0);
						//no++;
					}
					d_phase_shift = 0;

					ni0 = idx_fine;
					d_preamble_block_idx = d_cp_length;
					d_rx_preamble_t_idx = 0;
					//d_symbol_count = d_cp_length;
					d_state = ST_PREAMBLE;
					break;
				}
					/*****************************************************
					 Preamble
					 ****************************************************/
				case ST_PREAMBLE:
				{
					if (d_preamble_block_idx < d_preamble_block_total)
					{
						//d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
						//out[no] = d_cfo_val * in[ni]; // Send it to output

						if ((d_preamble_block_idx % (d_fft_length + d_cp_length))
								>= d_cp_length)
						{
							// Save value for CFO estimation
							d_rx_preamble_t[d_rx_preamble_t_idx] = in0[ni0];
							d_rx_preamble_t_idx++;
						}

						//d_symbol_count++;
						d_cfo_idx++;
						ni0++;
						//no++;
						d_preamble_block_idx++;
						d_state = ST_PREAMBLE;
					}
					else
					{
						d_preamble_block_idx = 0;
						d_state = ST_CFO_SCHMIDL_COX;
					}
					break;
				}
					/*****************************************************
					 Carrier Frequency Offset Estimation (Schmidl-Cox)
					 ****************************************************/
				case ST_CFO_SCHMIDL_COX:
				{
					estimate_cfo_schmidl_cox();
					d_state = ST_PILOT;
					break;
				}
					/*****************************************************
					 Pilot
					 ****************************************************/
				case ST_PILOT:
				{
					if (d_pilot_block_idx < d_pilot_block_total)
					{
						//d_cfo_arg = d_cfo_idx * d_phase_shift;
						d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
						gr_complex pil_sym = d_cfo_val * in0[ni0];
						if(d_pilot_block_idx>=d_cp_length)
						{
							d_rx_pilot_t[d_pilot_block_idx-d_cp_length] = pil_sym;
						}
						//d_symbol_count++;
						d_cfo_idx++;
						ni0++;
						//no++;
						d_pilot_block_idx++;
						d_state = ST_PILOT;
					}
					else
					{
						d_rx_pilot_f = FFT_Func(d_rx_pilot_t, true);
						d_pilot_block_idx = 0;
						d_state = ST_DATA;
					}
					break;
				}
					/*****************************************************
					 Data
					 ****************************************************/
				case ST_DATA:
				{
					if (d_data_block_idx < d_data_block_total)
					{
						//d_cfo_arg = d_cfo_idx * d_phase_shift;
						d_cfo_val = std::polar(1.0, d_cfo_idx * d_phase_shift);
						gr_complex header_sym = d_cfo_val * in0[ni0];
						if(d_data_block_idx>=d_cp_length)
						{
							d_rx_header_t[d_data_block_idx-d_cp_length] = header_sym;
						}
						//d_symbol_count++;
						d_cfo_idx++;
						ni0++;
						//no++;
						d_data_block_idx++;
						d_state = ST_DATA;
					}
					else
					{
						d_rx_header_f = FFT_Func(d_rx_header_t, true);
						d_data_block_idx = 0;
						d_state = ST_EQUALIZATION;
					}
					break;
				}
				case ST_EQUALIZATION:
				{
					std::vector<gr_complex> symbols(8, gr_complex(0, 0));
					int sym_idx = 0;
					for (int i = 0; i < d_occupied_tones; i++)
					{
						d_h_est[i] = (d_rx_pilot_f[d_left_padding + i]
								   / d_tx_pilot[d_left_padding + i]);
						//std::cout<<d_h_est[i]<<"; ";
						d_rx_header[i] = d_rx_header_f[d_left_padding + i]
													/ d_h_est[i];
						symbols[sym_idx] = d_rx_header[i];
						sym_idx++;
						if(sym_idx==8)
						{
							unsigned char sym = BPSK_Demapper(symbols);
							std::cout<<(int) sym<<", ";
							sym_idx = 0;
						}
					}
					std::cout<<std::endl;
					d_state = ST_IDLE;
					break;
				}
				case ST_TIMEOUT:
				{
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
					throw std::invalid_argument("ofdm_sync::invalid state");
					break;
			}
		}
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume (0, ni0);
        consume (1, ni1);

        // Tell runtime system how many output items we produced.
        return no;
    }

  } /* namespace PNC4TWRN */
} /* namespace gr */

