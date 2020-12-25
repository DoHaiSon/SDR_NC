
/* -*- c++ -*- */
/* 
   Network Coding project at: Signals and Systems Laboratory, VNU-UET.
   written by: Van-Ly Nguyen
   email: lynguyenvan.uet@gmail.com 
*/

#ifndef INCLUDED_DNC_PACKET_ARRANGEMENT_IMPL_H
#define INCLUDED_DNC_PACKET_ARRANGEMENT_IMPL_H

#include <DNC/packet_arrangement.h>

namespace gr {
  namespace DNC {

    class packet_arrangement_impl : public packet_arrangement
    {
     private:
	enum state_rx
	{
		ST_RX_IDLE,
		ST_RX_PACKET_NUMBER,
		ST_RX_PACKET_DATA
	};
	enum state_tx
	{
		ST_TX_CHECK_RECEIVED_PACKET,
		ST_TX_PACKET_DATA_OUT
	};
	state_tx d_tx_state;
	state_rx d_rx_state;

	int d_buffer_size;
	int d_packet_size;
	unsigned char d_nodeA_id;
	unsigned char d_nodeB_id;
	unsigned char d_node;

	bool d_break;
	unsigned char d_rx_node_id;
	unsigned char d_rx_packet_number;
	int d_rx_packet_index;
	int d_rx_data_index;

	int d_tx_packet_index;
	int d_tx_data_index;

	std::vector<unsigned char> d_packet_number;
	std::vector<unsigned char> d_packet_data;

     public:
      packet_arrangement_impl(int buffer_size, int packet_size, unsigned char nodeA_id, unsigned char nodeB_id, int node);
      ~packet_arrangement_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace DNC
} // namespace gr

#endif /* INCLUDED_DNC_PACKET_ARRANGEMENT_IMPL_H */

