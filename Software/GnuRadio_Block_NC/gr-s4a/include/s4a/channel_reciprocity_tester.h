/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CHANNEL_RECIPROCITY_TESTER_H
#define INCLUDED_S4A_CHANNEL_RECIPROCITY_TESTER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Channel reciprocity tester
 *
 * Prepares a packet to send to the remote node and then
 * receives reply packets. The idea is to estimate channels 
 * at both sides in ping-pong manner such that we can see if 
 * the channel is reciprocal or not. This block uses OFDM
 * transmitter and receiver chains. In other words, an OFDM 
 * transmitter and receiver need to be connected to this block.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 23:06:22 CEST
 */
class S4A_API channel_reciprocity_tester: virtual public gr::block
{
public:
	typedef boost::shared_ptr<channel_reciprocity_tester> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::channel_reciprocity_tester.
	 *
	 * To avoid accidental use of raw pointers, s4a::channel_reciprocity_tester's
	 * constructor is in a private implementation
	 * class. s4a::channel_reciprocity_tester::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int channel_size, int required_data_bytes_on_each_burst,
			float power_current, float power_step, float power_max,
			float power_min, int working_mode, int node_id, int remote_node_id,
			int response_timeout_ms, int wrong_packet_discard_limit,
			int tx_packet_repetition, float snr_limit,
			int snr_console_print_rate);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CHANNEL_RECIPROCITY_TESTER_H */

