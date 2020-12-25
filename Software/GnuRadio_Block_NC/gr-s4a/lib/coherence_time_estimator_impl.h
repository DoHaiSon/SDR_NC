/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_COHERENCE_TIME_ESTIMATOR_IMPL_H
#define INCLUDED_S4A_COHERENCE_TIME_ESTIMATOR_IMPL_H

#include <s4a/coherence_time_estimator.h>

namespace gr
{
namespace s4a
{

class coherence_time_estimator_impl: public coherence_time_estimator
{
private:
	int d_data_sz;
	int d_channel_sz;

	int d_pearson_correlation;

	int d_output_mode;

	int d_data_block_idx;
	int d_data_block_rep;

	int d_check_idx;
	int d_check_sz;

	float d_r_differentiation;

	float d_snr;
	float d_snr_threshold;

	int d_signal_bw;
	int d_frame_sz;

	bool d_is_changed;
	bool d_is_out;

	enum state_t
	{
		ST_IDLE,
		ST_CHECK_SNR,
		ST_ESTIMATE_COEFFICIENT,
		ST_DEMODULATE_DATA,
		ST_PACKET_DECODE,
		ST_VALIDATE,
		ST_FINALIZE
	};
	state_t d_state;

	// <coherence time estimation>
	void
	calculate_R_1D(const gr_complex *in, int idx);
	void
	calculate_R_2D(const gr_complex *in, int idx);

	std::vector<gr_complex> d_previous_h;
	std::vector<gr_complex> d_current_h;
	double R;
	double R_old;
	double R_diff;

	double R_1;
	double yt_1;
	double xt_1;
	double syy_1;
	double sxy_1;
	double sxx_1;
	double ax_1;
	double ay_1;
	double mx_1;
	double my_1;

	double R_2;
	double yt_2;
	double xt_2;
	double syy_2;
	double sxy_2;
	double sxx_2;
	double ax_2;
	double ay_2;
	double mx_2;
	double my_2;

	double d_symbol_time;
	double d_coherence_time;
	double d_coherence_distance;

	int d_previous_counter;
	int d_current_header;
	int d_current_counter;
	int d_current_zeros;

	/*
	 * Function pointer for correleation type (1D or 2D). It's better
	 * to use function pointer and decice which correleation type wil
	 *  be used. Otherwise, we would check each time what we need to
	 *  use in runtime with if statements.
	 */
	void
	(s4a::coherence_time_estimator_impl::*correlate)(const gr_complex*, int);

	// </coherence time estimation>

	// <counter packet>
	int d_packet_header_sz;
	int d_packet_header_val;
	int d_packet_counter_sz;
	int d_packet_rep;
	int d_packet_zero_padding;

	int d_packet_total_bits;

	unsigned char* d_packet;

	int d_bit_threshhold;
	// </counter packet>

public:
	coherence_time_estimator_impl(int pearson_correlation, int output_mode,
			int data_sz, int channel_sz, int data_block_rep, int check_sz,
			float r_differentiation, float snr_threshold, int signal_bw,
			int frame_sz, int packet_header_sz, int packet_header_val,
			int packet_counter_sz, int packet_rep, int packet_zero_padding);
	~coherence_time_estimator_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_COHERENCE_TIME_ESTIMATOR_IMPL_H */

