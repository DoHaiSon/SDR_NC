/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_FRAME_GENERATION_IMPL_H
#define INCLUDED_S4A_FRAME_GENERATION_IMPL_H

#include <s4a/frame_generation.h>
#include <vector>

namespace gr
{
namespace s4a
{

class frame_generation_impl: public frame_generation
{
private:
	enum state_t
	{
		ST_IDLE,
		ST_DATA_STORE,
		ST_DATA,
		ST_PILOT,
		ST_PREAMBLE,
		ST_SILENCE,
		ST_TIMEOUT
	};

	int d_fft_length;
	int d_occupied_tones;
	int d_left_padding;

	int d_preamble_block_rep;
	int d_preamble_block_idx;

	int d_pilot_block_rep;
	int d_pilot_block_idx;

	int d_data_block_sz;
	int d_data_block_rep;
	int d_data_block_idx;

	int d_silence_block_rep;
	int d_silence_block_idx;

	std::vector<gr_complex> d_preamble;
	std::vector<gr_complex> d_pilot;
	std::vector<gr_complex> d_data;
	std::vector<gr_complex> d_zeros;

	state_t d_state;
	int d_count;
	int d_block_sz;

	/* timeout syms */
	int d_timeout_syms_total;
	int d_timeout_syms_idx;
	gr_complex d_timeout_ref_sym;

public:
	static std::vector<gr_complex>
	generate_complex_sequence(int seq_size, int left_padding, int zeros_period,
			int seed);

	frame_generation_impl(int fft_length, int occupied_tones,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep);
	~frame_generation_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_FRAME_GENERATION_IMPL_H */

