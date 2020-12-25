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
#include "vfdm_modulator_impl.h"

namespace gr
{
namespace s4a
{

vfdm_modulator::sptr vfdm_modulator::make(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int channel_size,
		int preamble_block_rep, int pilot_block_t_sz, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		float preamble_coeff, float pilot_coeff, float data_coeff)
{
	return gnuradio::get_initial_sptr(
			new vfdm_modulator_impl(fft_length, occupied_tones, cp_length,
					precoder_length, channel_size, preamble_block_rep,
					pilot_block_t_sz, pilot_block_rep, data_block_sz,
					data_block_rep, silence_block_rep, preamble_coeff,
					pilot_coeff, data_coeff));
}

/*
 * The private constructor
 */
vfdm_modulator_impl::vfdm_modulator_impl(int fft_length, int occupied_tones,
		int cp_length, int precoder_length, int channel_size,
		int preamble_block_rep, int pilot_block_t_sz, int pilot_block_rep,
		int data_block_sz, int data_block_rep, int silence_block_rep,
		float preamble_coeff, float pilot_coeff, float data_coeff) :
		gr::hier_block2("vfdm_modulator",
				gr::io_signature::make2(2, 2, sizeof(gr_complex),
						channel_size * sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
	// Init Serial to Parallel
	d_serial_to_parallel = serial_to_parallel::make(precoder_length);

	// Init VFDM precoder
	d_precoder = vfdm_precoder::make(fft_length, occupied_tones, cp_length,
			precoder_length, channel_size, preamble_block_rep, pilot_block_t_sz,
			pilot_block_rep, data_block_sz, data_block_rep, silence_block_rep);

	// Init Power Scaling
	d_power_scaling = vfdm_power_scaling::make(fft_length, precoder_length,
			preamble_coeff, pilot_coeff, data_coeff, preamble_block_rep,
			pilot_block_t_sz, pilot_block_rep, data_block_sz, data_block_rep,
			silence_block_rep);

	// Init Parallel to Serial
	d_parallel_to_serial = parallel_to_serial::make(
			fft_length + precoder_length);

	// Connect blocks
	connect(self(), 0, d_serial_to_parallel, 0);
	connect(self(), 1, d_precoder, 1);
	connect(d_serial_to_parallel, 0, d_precoder, 0);
	connect(d_precoder, 0, d_power_scaling, 0);
	connect(d_power_scaling, 0, d_parallel_to_serial, 0);
	connect(d_parallel_to_serial, 0, self(), 0);
}

/*
 * Our virtual destructor.
 */
vfdm_modulator_impl::~vfdm_modulator_impl()
{
}

} /* namespace s4a */
} /* namespace gr */

