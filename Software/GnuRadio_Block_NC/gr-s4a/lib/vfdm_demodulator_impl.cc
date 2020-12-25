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
#include "vfdm_demodulator_impl.h"
#include <iostream>
#include <cstdio>

namespace gr
{
namespace s4a
{

vfdm_demodulator::sptr vfdm_demodulator::make(int fft_length,
		int occupied_tones, int cp_length, int precoder_length,
		int preamble_block_rep, int pilot_block_t_sz, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep, int time_out,
		int cfo_sync_method, int verbose)
{
	return gnuradio::get_initial_sptr(
			new vfdm_demodulator_impl(fft_length, occupied_tones, cp_length,
					precoder_length, preamble_block_rep, pilot_block_t_sz,
					pilot_block_rep, data_block_sz, data_block_rep,
					silence_block_rep, time_out, cfo_sync_method, verbose));
}

/*
 * The private constructor
 */
vfdm_demodulator_impl::vfdm_demodulator_impl(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int preamble_block_rep,
		int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
		int data_block_rep, int silence_block_rep, int cfo_sync_method, int time_out,
		int verbose) :
		gr::hier_block2("vfdm_demodulator",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make3(3, 3,
						data_block_sz * precoder_length * sizeof(gr_complex),
						(fft_length * precoder_length) * sizeof(gr_complex),
						sizeof(float)))
{
	int cfo_next_preamble_distance = 1;
	printf("vfdm_demodulator::fft_length %d\n", fft_length);
	printf("vfdm_demodulator::occupied_tones %d\n", occupied_tones);
	printf("vfdm_demodulator::cp_length %d\n", cp_length);
	printf("vfdm_demodulator::precoder_length %d\n", precoder_length);
	printf("vfdm_demodulator::preamble_block_rep %d\n", preamble_block_rep);
	printf("vfdm_demodulator::pilot_block_t_sz %d\n", pilot_block_t_sz);
	printf("vfdm_demodulator::pilot_block_rep %d\n", pilot_block_rep);
	printf("vfdm_demodulator::data_block_sz %d\n", data_block_sz);
	printf("vfdm_demodulator::data_block_rep %d\n", data_block_rep);
	printf("vfdm_demodulator::silence_block_rep %d\n", silence_block_rep);
	printf("vfdm_demodulator::cfo_next_preamble_distance %d\n",
			cfo_next_preamble_distance);
	printf("vfdm_demodulator:: cfo_sync_method %d\n", cfo_sync_method);

	// Init Synchronizer
	d_sync = s4a::vfdm_sync::make(fft_length, occupied_tones, cp_length,
			precoder_length, preamble_block_rep, pilot_block_t_sz,
			pilot_block_rep, data_block_sz, data_block_rep, silence_block_rep,
			cfo_next_preamble_distance, time_out, cfo_sync_method);

	// Init Serial to parallel
	d_serial_to_parallel = serial_to_parallel::make(fft_length + cp_length);

	// Init Cyclic Prefix Removal
	d_cyclic_prefix_removal = cyclic_prefix_removal::make(fft_length,
			cp_length);

	// Init FFT
	const std::vector<float> d_fft_window;
	d_fft = d_fft = fft::fft_vcc::make(fft_length, true, d_fft_window, true);

	// Init Equalizer
	d_equalizer = vfdm_equalizer::make(fft_length, occupied_tones, cp_length,
			precoder_length, preamble_block_rep, pilot_block_t_sz,
			pilot_block_rep, data_block_sz, data_block_rep, silence_block_rep,
			verbose);

	// Connect blocks
	connect(self(), 0, d_sync, 0);
	connect(d_sync, 0, d_serial_to_parallel, 0);
	connect(d_serial_to_parallel, 0, d_cyclic_prefix_removal, 0);
	connect(d_cyclic_prefix_removal, 0, d_fft, 0);
	connect(d_fft, 0, d_equalizer, 0);
	connect(d_equalizer, 0, self(), 0);
	connect(d_equalizer, 1, self(), 1);
	connect(d_equalizer, 2, self(), 2);
}

/*
 * Our virtual destructor.
 */
vfdm_demodulator_impl::~vfdm_demodulator_impl()
{
}

} /* namespace s4a */
} /* namespace gr */

