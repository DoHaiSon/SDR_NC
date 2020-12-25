/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_METRICS_CONTROLLER_H
#define INCLUDED_S4A_OFDM_METRICS_CONTROLLER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM metrics controller
 *
 * Parses the metrics of the previous controller 
 * blocks and generates data symbols for 
 * \emph{OFDM transmitter}. More detailed can be found 
 * in \emph{VFDM metrics controller} which has similar 
 * functionality to this block.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 00:07:50 CEST
 */
class S4A_API ofdm_metrics_controller: virtual public gr::block
{
public:
	typedef boost::shared_ptr<ofdm_metrics_controller> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::ofdm_metrics_controller.
	 *
	 * To avoid accidental use of raw pointers, s4a::ofdm_metrics_controller's
	 * constructor is in a private implementation
	 * class. s4a::ofdm_metrics_controller::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int data_syms_length, int channel_size);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_METRICS_CONTROLLER_H */

