
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_MDC_DECODING_IMPL_H
#define INCLUDED_DNC_MDC_DECODING_IMPL_H

#include <DNC/MDC_decoding.h>

namespace gr {
  namespace DNC {

    class MDC_decoding_impl : public MDC_decoding
    {
     private:
	enum state_tx
	{
		ST1_CHECK_DATA_OUT,
		ST1_DECODING,
		ST1_DATA_OUT,
		ST1_FREE,
		ST1_DELAY,

		ST2_CHECK_DATA_OUT,
		ST2_DATA_OUT,
		ST2_FREE,
		ST2_DELAY
	};
	state_tx d_state_tx;

	int d_image_width;
	int d_image_height;
	int d_no_descriptions;

	int d_rx0_index;
	int d_rx1_index;
	int d_tx_index;

	int d_rx0_pointer;
	int d_rx1_pointer;
	int d_tx_pointer;

	int d_check_end;
	int d_delay;

	std::vector<int> d_check_rows;
	std::vector<unsigned char> d_data_in;
	std::vector<unsigned char> d_data_out;

     public:
      MDC_decoding_impl(int image_width, int image_height, int no_descriptions);
      ~MDC_decoding_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_MDC_DECODING_IMPL_H */

