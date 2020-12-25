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
#include "ofdm_demodulator_impl.h"

namespace gr
{
namespace s4a
{

ofdm_demodulator::sptr ofdm_demodulator::make(int fft_length,
		int occupied_tones, int cp_length, int preamble_block_rep,
		int pilot_block_rep, int data_block_sz, int data_block_rep,
		int silence_block_rep, int timeout_ms, int cfo_sync_method, int verbose)
{
	return gnuradio::get_initial_sptr(
			new ofdm_demodulator_impl(fft_length, occupied_tones, cp_length,
					preamble_block_rep, pilot_block_rep, data_block_sz,
					data_block_rep, silence_block_rep, timeout_ms,
					cfo_sync_method, verbose));
}

/*
 * The private constructor
 */
ofdm_demodulator_impl::ofdm_demodulator_impl(int fft_length, int occupied_tones,
		int cp_length, int preamble_block_rep, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		int timeout_ms, int cfo_sync_method, int verbose) :
		gr::hier_block2("ofdm_demodulator",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make3(3, 3,
						(occupied_tones * data_block_sz) * sizeof(gr_complex),
						occupied_tones * sizeof(gr_complex), sizeof(float)))
{
	int cfo_next_preamble_distance = 1;

	// Init Synchronizer
	d_sync = s4a::ofdm_sync::make(fft_length, occupied_tones, cp_length,
			preamble_block_rep, pilot_block_rep, data_block_sz, data_block_rep,
			silence_block_rep, cfo_next_preamble_distance, timeout_ms,
			cfo_sync_method);

	// Init Serial to parallel
	d_serial_to_parallel = s4a::serial_to_parallel::make(
			fft_length + cp_length);

	// Init Cyclic Prefix Removal
	d_cyclic_prefix_removal = s4a::cyclic_prefix_removal::make(fft_length,
			cp_length);

	// Init FFT
	const std::vector<float> d_fft_window;
	d_fft = fft::fft_vcc::make(fft_length, true, d_fft_window, true);

	// Init Equalizer
	d_equalizer = s4a::ofdm_equalizer::make(fft_length, occupied_tones,
			cp_length, preamble_block_rep, pilot_block_rep, data_block_sz,
			data_block_rep, silence_block_rep, verbose);

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
ofdm_demodulator_impl::~ofdm_demodulator_impl()
{
}

} /* namespace s4a */
} /* namespace gr */

