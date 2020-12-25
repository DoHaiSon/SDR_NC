/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_MODULATOR_IMPL_H
#define INCLUDED_S4A_OFDM_MODULATOR_IMPL_H

#include <s4a/ofdm_modulator.h>
#include <s4a/serial_to_parallel.h>
#include <s4a/frame_generation.h>
#include <gnuradio/fft/fft_vcc.h>
#include <s4a/ofdm_power_scaling.h>
#include <s4a/cyclic_prefix_insertion.h>
#include <s4a/parallel_to_serial.h>

namespace gr
{
namespace s4a
{

class ofdm_modulator_impl: public ofdm_modulator
{
private:
	// Blocks
	serial_to_parallel::sptr d_serial_to_parallel;
	frame_generation::sptr d_frame_generation;
	fft::fft_vcc::sptr d_ifft;
	s4a::ofdm_power_scaling::sptr d_power_scaling;
	cyclic_prefix_insertion::sptr d_cyclic_prefix_insertion;
	parallel_to_serial::sptr d_parallel_to_serial;

public:
	ofdm_modulator_impl(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, float preamble_coeff,
			float pilot_coeff, float data_coeff);
	~ofdm_modulator_impl();

	// Where all the action really happens
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_MODULATOR_IMPL_H */

