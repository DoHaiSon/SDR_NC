/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_DELAY_PACKET_IMPL_H
#define INCLUDED_DNC_DELAY_PACKET_IMPL_H

#include <DNC/delay_packet.h>

namespace gr {
  namespace DNC {

    class delay_packet_impl : public delay_packet
    {
     private:
	enum state_t
	{
		ST_ADD_DELAY,
		ST_HEADER,
		ST_CHECK,
		ST_SLEEP,
		ST_DATA
	};
	state_t d_state;
	long int d_clock;
	long int d_clock_diff;
	int d_data_size;
	int d_data_index;
	int d_header_index;
	int d_delay_index;
	int d_delay_val;
	int d_max_delay;
	char d_packet_number;
	int d_packet_no;
	int d_count;

     public:
      delay_packet_impl(int data_size, int max_delay);
      ~delay_packet_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_DELAY_PACKET_IMPL_H */

