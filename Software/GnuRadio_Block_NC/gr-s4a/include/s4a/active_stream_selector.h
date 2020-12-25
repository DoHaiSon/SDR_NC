/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_ACTIVE_STREAM_SELECTOR_H
#define INCLUDED_S4A_ACTIVE_STREAM_SELECTOR_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Active stream selector
 *		
 * Checks the input streams and pushes to the output the
 * one that has items in the buffer. If both the inputs
 * have symbols, it then interleaves with respect to the input
 * order.
 *  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 21:33:37 CEST
 */
class S4A_API active_stream_selector: virtual public gr::block
{
public:
	typedef boost::shared_ptr<active_stream_selector> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::active_stream_selector.
	 *
	 * To avoid accidental use of raw pointers, s4a::active_stream_selector's
	 * constructor is in a private implementation
	 * class. s4a::active_stream_selector::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int first_stream_length, int second_stream_length);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_ACTIVE_STREAM_SELECTOR_H */

