/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_PRIMARY_USER_RX_CONTROLLER_H
#define INCLUDED_S4A_PRIMARY_USER_RX_CONTROLLER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Primary user Rx controller
 *
 *  1- Primary user (Lisenced user)
 *  --------------------    h11     --------------------
 *  |OFDM transmitter  | ---------> |  OFDM  receiver   |
 *  -------------------- \  / h21    --------------------
 *                        \/
 *                        /\
 *                       /  \ h12
 *  --------------------     \      --------------------
 *  |VFDM transmitter  | ---------> |  VFDM receiver   |
 *  --------------------    h22     --------------------
 *
 *  2- Secondary user (Unlisenced cognitive user)
 *
 *
 * Controls the OFDM transmitter and receiver 
 * blocks inside Primary user \RX node. Indeed,
 * this node could only do reception. But for experimental
 * purposes, we would like to make transmission such that
 * secondary user TX can estimate $h^(21)$ channel
 * and build the precoder. Apart from message sinks,
 * sources and some Python glue logic, the current
 * GNURadio/GRC blocks does not support the discrete
 * messaging with other blocks natively. Since we want
 * to implement a pure GNURadio C++ block implementation
 * with GRC support, we implement this block to
 * connect the OFDM receiver and transmitter to
 * this controller such that we can handle the
 * transmission/reception flow.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 00:41:38 CEST
 */
class S4A_API primary_user_rx_controller: virtual public gr::block
{
public:
	typedef boost::shared_ptr<primary_user_rx_controller> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::primary_user_rx_controller.
	 *
	 * To avoid accidental use of raw pointers, s4a::primary_user_rx_controller's
	 * constructor is in a private implementation
	 * class. s4a::primary_user_rx_controller::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int channel_size, int required_data_bytes_on_each_burst,
			int working_mode, int symbol_flow, int node_id, int remote_node_id,
			int response_timeout_ms, int wrong_packet_discard_limit,
			int tx_packet_repetition, float snr_limit,
			int snr_console_print_rate);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_PRIMARY_USER_RX_CONTROLLER_H */

