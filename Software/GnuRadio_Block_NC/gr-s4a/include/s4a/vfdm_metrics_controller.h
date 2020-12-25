/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_METRICS_CONTROLLER_H
#define INCLUDED_S4A_VFDM_METRICS_CONTROLLER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM metrics controller
 *
 * Parses the metrics of the previous controller blocks and 
 * generates data symbols for OFDM or VFDM. This block 
 * is useful to transform the channel state information from 
 * frequency domain to time domain for VFDM. Moreover, it 
 * enables only one stream in a given time such that VFDM 
 * and OFDM modulators do not generate frames simultaneously. 
 * Apart from the constellated data symbols, the metrics at the 
 * input port needs to be in this order:
 *
 *  1- The timestamp in ms.
 *  2- The estimated SNR from the last reception.
 *  3- The node ID.
 *  4- The remote node ID
 *  5- The transmission mode that can be:
 *  		ZEROS: $0+0i$
 *  		OFDM: $1+0i$
 *  		VFDM: $2+0i$	
 *  6- The estimated channel vector in frequency.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 01:59:31 CEST
 */
class S4A_API vfdm_metrics_controller: virtual public gr::block
{
public:
	typedef boost::shared_ptr<vfdm_metrics_controller> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_metrics_controller.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_metrics_controller's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_metrics_controller::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int data_syms_length, int channel_size, int normalize_h);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_METRICS_CONTROLLER_H */

