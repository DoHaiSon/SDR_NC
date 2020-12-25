/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_SYNC_IMPL_H
#define INCLUDED_S4A_OFDM_SYNC_IMPL_H

#include <s4a/ofdm_sync.h>
#include <gnuradio/fft/fft.h>
#include <vector>

namespace gr
{
namespace s4a
{

class ofdm_sync_impl: public ofdm_sync
{
private:
	enum state_t
	{
		ST_IDLE,
		ST_COARSE_SYNC,
		ST_FINE_SYNC,
		ST_PREAMBLE,
		ST_CFO,
		ST_CFO_SCHMIDL_COX,
		ST_PILOT,
		ST_DATA,
		ST_SILENCE,
		ST_TIMEOUT
	};
	fft::fft_complex *d_ifft;
	fft::fft_complex *d_fft;

	int d_fft_length;
	int d_occupied_tones;
	int d_left_padding;
	int d_cp_length;

	int d_preamble_block_idx;
	int d_preamble_block_rep;
	int d_preamble_block_total;

	int d_pilot_block_idx;
	int d_pilot_block_rep;
	int d_pilot_block_total;

	int d_data_block_idx;
	int d_data_block_sz;
	int d_data_block_rep;
	int d_data_block_total;

	int d_silence_block_idx;
	int d_silence_block_rep;
	int d_silence_block_total;

	int d_timeout_ms;

	int d_cfo_sync_method;

	/* timeout variables */
	int d_timeout_syms_total;
	int d_timeout_syms_idx;
	gr_complex d_timeout_ref_sym;
	long int d_clock;
	long int d_clock_diff;

	int d_cfo_next_preamble_distance;
	int d_cfo_preamble_jump;
	int d_cfo_step_sz;
	int d_cfo_window_sz;

	int d_frame_length;
	int d_window_sz;

	int d_cfo_idx;
	double d_cfo_arg;
	gr_complex d_cfo_val;

	std::vector<gr_complex> d_tx_preamble_f;
	std::vector<gr_complex> d_tx_preamble_t;
	std::vector<gr_complex> d_rx_preamble_t;

	/* For Schmidl-Cox CFO method, we may
	 need to transfrom the first and second half
	 of the preamble to the frequency domain */
	gr_complex* d_rx_preamble_f1;
	gr_complex* d_rx_preamble_f2;
	int g_span1;
	int g_span2;
	int g;
	int g_est;
	gr_complex B1;
	gr_complex B2;
	double Bmax;
	int Bidx;
	double* B;

	int d_rx_preamble_t_idx;

	std::vector<gr_complex> d_cfo;
	int
	estimate_cfo();

	int
	estimate_cfo_schmidl_cox();

	double d_phase_shift;

	double d_est_SNR;
	double d_signal_power;
	double d_noise_power;

	state_t d_state;
	int d_symbol_count;

	/* sync variables */
	int idx_coarse;
	int idx_fine;

	/* coarse sync variables */
	gr_complex P;
	double R;
	double M;

	/* variables for correlation (for fine timing sync) */
	std::vector<double> x;
	std::vector<double> y;
	double ax;
	double ay;

	double yt;
	double xt;

	double syy;
	double sxy;
	double sxx;

public:
	static std::vector<gr_complex>
	generate_complex_sequence(int seq_size, int left_padding, int zeros_period,
			int seed, bool verbose);
	std::vector<gr_complex>
	generate_complex_sequence_in_time(int seq_size, int left_padding,
			int zeros_period, int seed, bool verbose);

	ofdm_sync_impl(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep,
			int cfo_next_preamble_distance, int timeout_ms,
			int cfo_sync_method);
	~ofdm_sync_impl();

	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items_v,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_SYNC_IMPL_H */

