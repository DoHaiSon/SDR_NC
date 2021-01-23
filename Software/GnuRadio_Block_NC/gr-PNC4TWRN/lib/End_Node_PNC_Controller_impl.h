
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_PNC4TWRN_END_NODE_PNC_CONTROLLER_IMPL_H
#define INCLUDED_PNC4TWRN_END_NODE_PNC_CONTROLLER_IMPL_H

#include <PNC4TWRN/End_Node_PNC_Controller.h>
#include <gnuradio/fft/fft.h>
#include <vector>

namespace gr {
  namespace PNC4TWRN {

    class End_Node_PNC_Controller_impl : public End_Node_PNC_Controller
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
			ST_EQUALIZATION,
			ST_TIMEOUT
		};
		state_t d_state;

		fft::fft_complex *d_ifft;
		fft::fft_complex *d_fft;
		fft::fft_complex *d_fft_4sync;

		int d_fft_length;
		int d_occupied_tones;
		int d_cp_length;
		int d_left_padding;

		int d_preamble_block_idx;
		int d_preamble_block_total;

		int d_pilot_block_idx;
		int d_pilot_block_total;

		int d_data_block_idx;
		int d_data_block_sz;
		int d_data_block_total;

		/* For Schmidl-Cox Sync Method */
		int d_window_sz;
		double d_phase_shift;
		gr_complex d_cfo_val;
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
		int d_cfo_idx;

		/* sync variables */
		int idx_coarse;
		int idx_fine;

		/* coarse sync variables */
		gr_complex P;
		double R;
		double M;

		/* timeout variables */
		int d_timeout_syms_total;
		int d_timeout_syms_idx;
		gr_complex d_timeout_ref_sym;
		long int d_clock;
		long int d_clock_diff;
		int d_timeout_ms;

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

		std::vector<gr_complex> d_tx_preamble_f;
		std::vector<gr_complex> d_tx_preamble_t;
		std::vector<gr_complex> d_rx_preamble_t;
		std::vector<gr_complex> d_rx_pilot_t;
		std::vector<gr_complex> d_rx_header_t;
		std::vector<gr_complex> d_rx_pilot_f;
		std::vector<gr_complex> d_rx_header_f;
		std::vector<gr_complex> d_rx_header;
		std::vector<gr_complex> d_tx_pilot;
		std::vector<gr_complex> d_h_est;

     public:
      End_Node_PNC_Controller_impl(int fft_length, int occupied_tones, int cp_length, int node);
      ~End_Node_PNC_Controller_impl();

		std::vector<gr_complex> generate_complex_sequence(int seq_size, int left_padding, int zeros_period, int seed);
		std::vector<gr_complex> generate_complex_sequence_in_time(int seq_size, int left_padding, int zeros_period, int seed);
		std::vector<gr_complex> FFT_Func(std::vector<gr_complex> input, bool forward);
		std::vector<gr_complex> CP_Insertion(std::vector<gr_complex> symbols_in);
		std::vector<gr_complex> CP_Removal(std::vector<gr_complex> symbols_in);
		std::vector<unsigned char> Byte_to_Bit(unsigned char byte_in);
		unsigned char Bit_to_Byte(std::vector<unsigned char> bits_in);
		std::vector<gr_complex> BPSK_Mapper(std::vector<unsigned char> bits_in);
		unsigned char BPSK_Demapper(std::vector<gr_complex> symbols_in);
		int estimate_cfo_schmidl_cox();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace PNC4TWRN
} // namespace gr

#endif /* INCLUDED_PNC4TWRN_END_NODE_PNC_CONTROLLER_IMPL_H */

