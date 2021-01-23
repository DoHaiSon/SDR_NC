
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "Relay_PNC_Controller_impl.h"
#include <gnuradio/fft/fft.h>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <complex.h>
#include <math.h>
#include <time.h>

namespace gr {
  namespace PNC4TWRN {

    Relay_PNC_Controller::sptr
    Relay_PNC_Controller::make(int fft_length, int occupied_tones, int cp_length)
    {
      return gnuradio::get_initial_sptr
        (new Relay_PNC_Controller_impl(fft_length, occupied_tones, cp_length));
    }

    /*
     * The private constructor
     */
    Relay_PNC_Controller_impl::Relay_PNC_Controller_impl(int fft_length, int occupied_tones, int cp_length)
      : gr::block("Relay_PNC_Controller",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))), 
		d_fft_length(fft_length), d_occupied_tones(occupied_tones), d_cp_length(cp_length),
		d_tx_beacon_index(0), d_beacon_frame_length(0), d_tx_beacon_ctrl(0),
		d_preamble_coeff(1/9.6571), d_pilot_coeff(1/13.9017), d_data_coeff(1/48.0),
		d_tx_state(ST_TX_ZEROS_TRANS)
    {
		d_left_padding = floor((fft_length - occupied_tones) / 2);

		// Init preamble sequence
		std::vector<gr_complex> preamble = generate_complex_sequence(fft_length, d_left_padding, 1, 0);
		d_preamble = preamble;

		// Init pilot sequence
		std::vector<gr_complex> pilot = generate_complex_sequence(fft_length, d_left_padding, 0, 0);
		d_pilot = pilot;

		// Beacon
		d_beacon.resize(fft_length);
		std::fill(d_beacon.begin(), d_beacon.end(), gr_complex(0, 0));
		for(int i = 0; i<d_occupied_tones; i++)
		{
			d_beacon[d_left_padding + i] = gr_complex(1, 0);
		}

		// Init FFT
		d_fft = new gr::fft::fft_complex(fft_length, true);
		d_ifft = new gr::fft::fft_complex(fft_length, false);

		// Init beacon frame
		d_beacon_frame_length = 3*(fft_length+d_cp_length);
		d_beacon_frame.resize(d_beacon_frame_length);

		std::vector<gr_complex> temp_seq1(fft_length, gr_complex(0,0));
		std::vector<gr_complex> temp_seq2(fft_length+d_cp_length, gr_complex(0,0));

		temp_seq1 = FFT_Func(d_preamble, false);
		temp_seq2 = CP_Insertion(temp_seq1);
		for(int i = 0; i<(fft_length+d_cp_length); i++)
		{
			temp_seq2[i] = d_preamble_coeff * temp_seq2[i];
		}
		memcpy(&d_beacon_frame[0], &temp_seq2[0], sizeof(gr_complex) * (fft_length+d_cp_length));

		temp_seq1 = FFT_Func(d_pilot, false);
		temp_seq2 = CP_Insertion(temp_seq1);
		for(int i = 0; i<(fft_length+d_cp_length); i++)
		{
			temp_seq2[i] = d_pilot_coeff * temp_seq2[i];
		}
		memcpy(&d_beacon_frame[fft_length+d_cp_length], &temp_seq2[0], sizeof(gr_complex) * (fft_length+d_cp_length));

		temp_seq1 = FFT_Func(d_beacon, false);
		temp_seq2 = CP_Insertion(temp_seq1);
		for(int i = 0; i<(fft_length+d_cp_length); i++)
		{
			temp_seq2[i] = d_data_coeff * temp_seq2[i];
		}
		memcpy(&d_beacon_frame[2*(fft_length+d_cp_length)], &temp_seq2[0], sizeof(gr_complex) * (fft_length+d_cp_length));
    }

    /* Generate Complex Sequence */
    std::vector<gr_complex> Relay_PNC_Controller_impl::generate_complex_sequence(
			int seq_size, int left_padding, int zeros_period, int seed)
    {
		std::vector<gr_complex> result(seq_size, gr_complex(0, 0));

		int i = 0;
		int istart = left_padding;
		int istop = seq_size - left_padding;
		// Initialize seed
		srand(seed);

		std::vector<gr_complex> syms(2, gr_complex(0, 0));
		syms[0] = gr_complex(+0.7, +0.7);
		syms[1] = gr_complex(-0.7, -0.7);

		for (i = istart; i < istop; i += zeros_period + 1)
		{
			gr_complex sym = syms[rand() % 2];
			result[i] = sym;
		}
		return result;
   }

   /* FFT */
   std::vector<gr_complex> Relay_PNC_Controller_impl::FFT_Func(std::vector<gr_complex> input, bool forward)
   {
		std::vector<gr_complex> out(d_fft_length, gr_complex(0, 0));
		if(!forward)
		{
			// Copy symbols to input buffer (with IFFT shift)
			memcpy(d_ifft->get_inbuf(), &input[0], d_fft_length * sizeof(gr_complex));
			gr_complex *dst = d_ifft->get_inbuf();
			unsigned int len = (unsigned int) (floor(d_fft_length/2.0));
			memcpy(&dst[0], &input[len], sizeof(gr_complex) * (d_fft_length - len));
			memcpy(&dst[d_fft_length - len], &input[0], sizeof(gr_complex) * len);

			// Execute IFFT
			d_ifft->execute();

			// Get results
			memcpy(&out[0], d_ifft->get_outbuf(), sizeof(gr_complex) * d_fft_length);
		}
		else
		{
			// Copy symbols to input buffer
			memcpy(d_fft->get_inbuf(), &input[0], d_fft_length * sizeof(gr_complex));

			// Execute FFT
			d_fft->execute();

			// Get results (with FFT shift)
			unsigned int len = (unsigned int)(ceil(d_fft_length/2.0));
			memcpy(&out[0], &d_fft->get_outbuf()[len], sizeof(gr_complex)*(d_fft_length - len));
			memcpy(&out[d_fft_length - len], &d_fft->get_outbuf()[0], sizeof(gr_complex)*len);
		}
		return out;
    }

    /* CP Insertion */
    std::vector<gr_complex> Relay_PNC_Controller_impl::CP_Insertion(std::vector<gr_complex> symbols_in)
    {
		int out_length = d_fft_length + d_cp_length;
		std::vector<gr_complex> symbols_out(out_length, gr_complex(0, 0));
		memcpy(&symbols_out[0], &symbols_in[d_fft_length - d_cp_length], d_cp_length * sizeof(gr_complex));
		memcpy(&symbols_out[d_cp_length], &symbols_in[0], d_fft_length * sizeof(gr_complex));
		return symbols_out;
    }

    /* CP Removal */
    std::vector<gr_complex> Relay_PNC_Controller_impl:: CP_Removal(std::vector<gr_complex> symbols_in)
    {
		std::vector<gr_complex> symbols_out(d_fft_length, gr_complex(0, 0));
		memcpy(&symbols_out[0], &symbols_in[d_cp_length], d_fft_length * sizeof(gr_complex));
		return symbols_out;
    }

    /* Byte-to-Bit Conversion */
    std::vector<unsigned char> Relay_PNC_Controller_impl::Byte_to_Bit(unsigned char byte_in)
    {
		std::vector<unsigned char> bits_out(8, 0x00);
		for (int i = 0; i < 8; i++)
		{
			bits_out[i] = ((byte_in >> i) & 0x1);
		}
		return bits_out;
    }

    /* Bit-to-Byte Conversion */
    unsigned char Relay_PNC_Controller_impl::Bit_to_Byte(std::vector<unsigned char> bits_in)
    {
		unsigned char byte_out;
		for (int i = 0; i<8; i++)
		{
			byte_out |= ((bits_in[i] & 0x1) << i);
		}
		return byte_out;
    }

    /* BPSK Mapper*/
    std::vector<gr_complex> Relay_PNC_Controller_impl::BPSK_Mapper(std::vector<unsigned char> bits_in)
    {
		std::vector<gr_complex> symbols_out(8, gr_complex(0, 0));
		std::vector<gr_complex> symbols(2, gr_complex(0, 0));
		symbols[0] = gr_complex(-1, 0); 
		symbols[1] = gr_complex(+1, 0);
		for (int i = 0; i<8; i++)
		{
			int bit = bits_in[i] & 0x1;
			symbols_out[i] = symbols[bit];
		}
		return symbols_out;
    }

    /* BPSK Demapper */
    unsigned char Relay_PNC_Controller_impl::BPSK_Demapper(std::vector<gr_complex> symbols_in)
    {
		unsigned char byte_out = 0x00;
		for (int i = 0; i<8; i++)
		{
			if (symbols_in[i].real() > 0)
			{
				byte_out |= (0x1 << i);
			}
		}
		return byte_out;
    }

    /*
     * Our virtual destructor.
     */
    Relay_PNC_Controller_impl::~Relay_PNC_Controller_impl()
    {
    }

    void
    Relay_PNC_Controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    Relay_PNC_Controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

		int nInput_Items = ninput_items[0];
		int nOutput_Items = noutput_items;

		int ni = 0;
		int no = 0;
		/*
		while(ni<nInput_Items)
		{
		
		}*/

		while(no<nOutput_Items)// && d_tx_beacon_ctrl<3)
		{
			switch(d_tx_state)
			{
				case ST_TX_ZEROS_TRANS:
				{
					out[no] = gr_complex(0,0);
					no++;
					d_tx_beacon_index++;
					if(d_tx_beacon_index==d_beacon_frame_length)
					{
						d_tx_beacon_index = 0;
						d_tx_state = ST_TX_BEACON;
					}
					break;
				}
				case ST_TX_BEACON:
				{
					out[no] = d_beacon_frame[d_tx_beacon_index];
					no++;
					d_tx_beacon_index++;
					if(d_tx_beacon_index==d_beacon_frame_length)
					{
						d_tx_beacon_index = 0;
						d_tx_beacon_ctrl++;
						d_tx_state = ST_TX_BEACON;
					}
					break;
				}
			}
		}
		// Tell runtime system how many input items we consumed on
		// each input stream.
		consume_each (ni);

		// Tell runtime system how many output items we produced.
		return no;
    }

  } /* namespace PNC4TWRN */
} /* namespace gr */

