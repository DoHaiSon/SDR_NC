/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_TRANSMITTER_CONTROL_P2P_IMPL_H
#define INCLUDED_DNC_TRANSMITTER_CONTROL_P2P_IMPL_H

#include <DNC/transmitter_control_p2p.h>

namespace gr {
  namespace DNC {

    class transmitter_control_p2p_impl : public transmitter_control_p2p
    {
     private:
	enum state_tx
	{
		ST_WAIT,
		ST_BUFFER_MANAGEMENT,
		ST_TRANSMITTER_ID_TRANS,
		ST_PACKET_NO_TRANS,
		ST_DATA_TRANS,
		ST_GUARD_INTERVAL_TRANS
	};
	enum state_rx
	{
		ST_IDLE,
		ST_CHECK_RECEIVER_ID,
		ST_CHECK_PACKET_NO_CONFIRM,
		ST_CHECK_SESSION_NUMBER
	};
	state_tx d_tx_state;
	state_rx d_rx_state;

	int d_packet_size;
	int d_guard_interval;
	int d_transmitter_id;
	int d_receiver_id;

	int d_buffer_size;
	int d_load_packet_index;
	int d_load_data_index;
	int d_number_of_session_packets;
	unsigned char d_load_packet_number;
	bool d_load_data;
	bool d_loaded_data;
	int d_tx_buffer_index;
	int d_header_index;
	int d_data_index;
	int d_guard_index;
	int d_check_rx_id_count;
	int d_check_rx_id_index;
	int d_check_recv_packet_number_index;
	unsigned char d_expected_session_number;
	int d_check_session_index;
	int d_check_session_count;

	std::vector<unsigned char> d_transmitted_data_buffer;
	std::vector<unsigned char> d_loaded_packet_number;
	std::vector<unsigned char> d_transmitted_packet_number;
	std::vector<unsigned char> d_confirm_packet_number;
	unsigned char d_rx_packet_number[3];
	void reset();

     public:
      transmitter_control_p2p_impl(int packet_size, int guard_interval, unsigned char transmitter_id, unsigned char receiver_id);
      ~transmitter_control_p2p_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_TRANSMITTER_CONTROL_P2P_IMPL_H */

