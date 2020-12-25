
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_LC_DECODING_IMPL_H
#define INCLUDED_DNC_LC_DECODING_IMPL_H

#include <DNC/LC_decoding.h>

namespace gr {
  namespace DNC {

    class LC_decoding_impl : public LC_decoding
    {
     private:
	int d_image_width;
	int d_image_height;

	int d_rx0_pointer;
	int d_rx1_pointer;
	int d_tx_pointer;
	int d_rx0_index;
	int d_rx1_index;
	int d_tx_index;

	int d_delay;

	std::vector<int> d_check_rows_based;
	std::vector<int> d_check_rows_enhanced;
	std::vector<int> d_check_rows_origin;

	std::vector<float> d_based_data;
	std::vector<float> d_enhanced_data;
	std::vector<float> d_original_data;
     public:
      LC_decoding_impl(int image_width, int image_height);
      ~LC_decoding_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_LC_DECODING_IMPL_H */

