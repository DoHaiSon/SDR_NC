
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_PNC4TWRN_RELAY_PNC_CONTROLLER_IMPL_H
#define INCLUDED_PNC4TWRN_RELAY_PNC_CONTROLLER_IMPL_H

#include <PNC4TWRN/Relay_PNC_Controller.h>
#include <gnuradio/fft/fft.h>
#include <vector>

namespace gr {
  namespace PNC4TWRN {

    class Relay_PNC_Controller_impl : public Relay_PNC_Controller
    {
     private:
	enum state_rx0
	{
		
	};

	enum state_tx
	{
		ST_TX_ZEROS_TRANS,
		ST_TX_BEACON
	};
	state_tx d_tx_state;
	state_rx0 d_rx0_state;

	fft::fft_complex *d_ifft;
	fft::fft_complex *d_fft;

	int d_fft_length;
	int d_occupied_tones;
	int d_cp_length;
	int d_left_padding;

	float d_preamble_coeff;
	float d_pilot_coeff;
	float d_data_coeff;

	int d_tx_beacon_index;
	int d_tx_beacon_ctrl;
	int d_beacon_frame_length;

	std::vector<gr_complex> d_preamble;
	std::vector<gr_complex> d_pilot;
	std::vector<gr_complex> d_beacon;
	std::vector<gr_complex> d_beacon_frame;
	std::vector<gr_complex> d_xor_data_frame;
     public:
        Relay_PNC_Controller_impl(int fft_length, int occupied_tones, int cp_length);
        ~Relay_PNC_Controller_impl();

	std::vector<gr_complex> generate_complex_sequence(int seq_size, int left_padding, int zeros_period, int seed);
	std::vector<gr_complex> FFT_Func(std::vector<gr_complex> input, bool forward);
	std::vector<gr_complex> CP_Insertion(std::vector<gr_complex> symbols_in);
	std::vector<gr_complex> CP_Removal(std::vector<gr_complex> symbols_in);
	std::vector<unsigned char> Byte_to_Bit(unsigned char byte_in);
	unsigned char Bit_to_Byte(std::vector<unsigned char> bits_in);
	std::vector<gr_complex> BPSK_Mapper(std::vector<unsigned char> bits_in);
	unsigned char BPSK_Demapper(std::vector<gr_complex> symbols_in);

        // Where all the action really happens
        void forecast (int noutput_items, gr_vector_int &ninput_items_required);

        int general_work(int noutput_items,
		         gr_vector_int &ninput_items,
		         gr_vector_const_void_star &input_items,
		         gr_vector_void_star &output_items);
    };

  } // namespace PNC4TWRN
} // namespace gr

#endif /* INCLUDED_PNC4TWRN_RELAY_PNC_CONTROLLER_IMPL_H */

