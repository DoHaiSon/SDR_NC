/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_POWER_SCALING_IMPL_H
#define INCLUDED_S4A_VFDM_POWER_SCALING_IMPL_H

#include <s4a/vfdm_power_scaling.h>
#include <vector>

namespace gr
{
namespace s4a
{

class vfdm_power_scaling_impl: public vfdm_power_scaling
{
private:
	enum state_t
	{
		ST_IDLE, ST_DATA, ST_PILOT, ST_PREAMBLE, ST_SILENCE
	};

	int d_fft_length;
	int d_precoder_length;
	float d_preamble_coeff;
	float d_pilot_coeff;
	float d_data_coeff;
	state_t d_state;

	int d_preamble_block_rep;
	int d_preamble_block_idx;

	int d_pilot_block_t_sz;
	int d_pilot_block_rep;
	int d_pilot_block_idx;
	int d_pilot_block_total;

	int d_data_block_sz;
	int d_data_block_rep;
	int d_data_block_idx;
	int d_data_block_total;

	int d_silence_block_rep;
	int d_silence_block_idx;

	int d_count;

	struct timeval t_begin,t_end;

	unsigned long mtime, seconds, useconds;

public:
	vfdm_power_scaling_impl(int fft_length, int precoder_length,
			float preamble_coeff, float pilot_coeff, float data_coeff,
			int preamble_block_rep, int pilot_block_t_sz, int pilot_block_rep,
			int data_block_sz, int data_block_rep, int silence_block_rep);
	~vfdm_power_scaling_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_POWER_SCALING_IMPL_H */

