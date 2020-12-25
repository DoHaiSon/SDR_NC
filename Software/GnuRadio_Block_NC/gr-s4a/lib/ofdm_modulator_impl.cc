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
#include "ofdm_modulator_impl.h"

namespace gr
{
namespace s4a
{

ofdm_modulator::sptr ofdm_modulator::make(int fft_length, int occupied_tones,
		int cp_length, int preamble_block_rep, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		float preamble_coeff, float pilot_coeff, float data_coeff)
{
	return gnuradio::get_initial_sptr(
			new ofdm_modulator_impl(fft_length, occupied_tones, cp_length,
					preamble_block_rep, pilot_block_rep, data_block_sz,
					data_block_rep, silence_block_rep, preamble_coeff,
					pilot_coeff, data_coeff));
}

/*
 * The private constructor
 */
ofdm_modulator_impl::ofdm_modulator_impl(int fft_length, int occupied_tones,
		int cp_length, int preamble_block_rep, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		float preamble_coeff, float pilot_coeff, float data_coeff) :
		gr::hier_block2("ofdm_modulator",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
	// Init Serial to Parallel
	d_serial_to_parallel = serial_to_parallel::make(occupied_tones);
	d_frame_generation = frame_generation::make(fft_length, occupied_tones,
			preamble_block_rep, pilot_block_rep, data_block_sz, data_block_rep,
			silence_block_rep);

	// Init IFFT
	const std::vector<float> d_ifft_window;
	d_ifft = fft::fft_vcc::make(fft_length, false, d_ifft_window, true);

	// Init Power Scaling
	d_power_scaling = ofdm_power_scaling::make(fft_length, preamble_coeff,
			pilot_coeff, data_coeff, preamble_block_rep, pilot_block_rep,
			data_block_sz, data_block_rep, silence_block_rep);

	// Init Cyclic Prefix Insertion
	d_cyclic_prefix_insertion = cyclic_prefix_insertion::make(fft_length,
			cp_length);

	// Init Parallel to Serial
	d_parallel_to_serial = parallel_to_serial::make(fft_length + cp_length);

	// Connect blocks
	connect(self(), 0, d_serial_to_parallel, 0);
	connect(d_serial_to_parallel, 0, d_frame_generation, 0);
	connect(d_frame_generation, 0, d_ifft, 0);
	connect(d_ifft, 0, d_power_scaling, 0);
	connect(d_power_scaling, 0, d_cyclic_prefix_insertion, 0);
	connect(d_cyclic_prefix_insertion, 0, d_parallel_to_serial, 0);
	connect(d_parallel_to_serial, 0, self(), 0);
}

/*
 * Our virtual destructor.
 */
ofdm_modulator_impl::~ofdm_modulator_impl()
{
}

} /* namespace s4a */
} /* namespace gr */

