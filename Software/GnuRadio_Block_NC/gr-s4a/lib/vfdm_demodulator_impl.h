/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_DEMODULATOR_IMPL_H
#define INCLUDED_S4A_VFDM_DEMODULATOR_IMPL_H

#include <s4a/vfdm_demodulator.h>
#include <s4a/vfdm_sync.h>
#include <s4a/serial_to_parallel.h>
#include <s4a/cyclic_prefix_removal.h>
#include <gnuradio/fft/fft_vcc.h>
#include <s4a/vfdm_equalizer.h>

namespace gr
{
namespace s4a
{

class vfdm_demodulator_impl: public vfdm_demodulator
{
private:
	// Blocks
	vfdm_sync::sptr d_sync;
	serial_to_parallel::sptr d_serial_to_parallel;
	cyclic_prefix_removal::sptr d_cyclic_prefix_removal;
	fft::fft_vcc::sptr d_fft;
	vfdm_equalizer::sptr d_equalizer;

public:
	vfdm_demodulator_impl(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int preamble_block_rep, int pilot_block_t_sz,
			int pilot_block_rep, int data_block_sz, int data_block_rep,
			int silence_block_rep, int time_out, int cfo_sync_method, int verbose);
	~vfdm_demodulator_impl();

	// Where all the action really happens
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_DEMODULATOR_IMPL_H */

