/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_EQUALIZER_IMPL_H
#define INCLUDED_S4A_OFDM_EQUALIZER_IMPL_H

#include <s4a/ofdm_equalizer.h>
#include <vector>
#include <sys/time.h>
#include <boost/thread.hpp>
#include <cstdio>

namespace gr
{
namespace s4a
{

class ofdm_equalizer_impl: public ofdm_equalizer
{
private:
	enum state_t
	{
		ST_IDLE,
		ST_PREAMBLE,
		ST_PILOT,
		ST_PILOT_POST,
		ST_DATA,
		ST_SILENCE,
		ST_OUT
	};
	state_t d_state;

	int d_fft_length;
	int d_occupied_tones;
	int d_left_padding;
	int d_cp_length;

	int d_preamble_block_rep;
	int d_preamble_block_idx;

	int d_pilot_block_rep;
	int d_pilot_block_idx;

	int d_data_block_sz;
	int d_data_block_rep;
	int d_data_block_total;
	int d_data_block_idx;

	int d_silence_block_rep;
	int d_silence_block_idx;

	int d_block_idx;
	int d_block_sz;

	int d_verbose;

	/* SNR Calculation */
	float d_snr_threshold;
	double d_est_SNR;
	double d_signal_power;
	double d_preamble_power;
	double d_pilot_power;
	double d_data_power;
	double d_noise_power;

	/* Vectors for channel estimation */
	std::vector<gr_complex> d_data;
	std::vector<gr_complex> d_pilot;
	std::vector<gr_complex> d_h_est;
	std::vector<gr_complex> d_h_est_sum;

public:
	static std::vector<gr_complex>
	generate_complex_sequence(int seq_size, int left_padding, int zeros_period,
			int seed);

	ofdm_equalizer_impl(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, int verbose);
	~ofdm_equalizer_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_EQUALIZER_IMPL_H */

