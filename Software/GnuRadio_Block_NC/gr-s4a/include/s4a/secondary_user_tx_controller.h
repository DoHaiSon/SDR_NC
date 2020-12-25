/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_SECONDARY_USER_TX_CONTROLLER_H
#define INCLUDED_S4A_SECONDARY_USER_TX_CONTROLLER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Secondary user Tx controller
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
 * Controls the OFDM/VFDM transmitters and 
 * OFDM receiver blocks inside the Secondary user 
 * TX node. This controller is similar to 
 * \emph{Primary user RX controller} but controls 
 * VFDM transmitter in addition.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 01:31:43 CEST
 */
class S4A_API secondary_user_tx_controller: virtual public gr::block
{
public:
	typedef boost::shared_ptr<secondary_user_tx_controller> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::secondary_user_tx_controller.
	 *
	 * To avoid accidental use of raw pointers, s4a::secondary_user_tx_controller's
	 * constructor is in a private implementation
	 * class. s4a::secondary_user_tx_controller::make is the public interface for
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

#endif /* INCLUDED_S4A_SECONDARY_USER_TX_CONTROLLER_H */

