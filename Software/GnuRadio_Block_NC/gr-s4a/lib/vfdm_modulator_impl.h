/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_MODULATOR_IMPL_H
#define INCLUDED_S4A_VFDM_MODULATOR_IMPL_H

#include <s4a/vfdm_modulator.h>
#include <s4a/serial_to_parallel.h>
#include <s4a/vfdm_precoder.h>
#include <s4a/vfdm_power_scaling.h>
#include <s4a/parallel_to_serial.h>

namespace gr
{
namespace s4a
{

class vfdm_modulator_impl: public vfdm_modulator
{
private:
	// Blocks
	serial_to_parallel::sptr d_serial_to_parallel;
	vfdm_precoder::sptr d_precoder;
	vfdm_power_scaling::sptr d_power_scaling;
	parallel_to_serial::sptr d_parallel_to_serial;

public:
	vfdm_modulator_impl(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int channel_size, int preamble_block_rep,
			int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, float preamble_coeff,
			float pilot_coeff, float data_coeff);

	~vfdm_modulator_impl();

	// Where all the action really happens
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_MODULATOR_IMPL_H */

