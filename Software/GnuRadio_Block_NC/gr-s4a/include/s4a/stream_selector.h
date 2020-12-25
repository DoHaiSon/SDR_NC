/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_STREAM_SELECTOR_H
#define INCLUDED_S4A_STREAM_SELECTOR_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Strem Selector
 *		  
 * Pushes the input stream to the output based on the
 * low and maximum magnitude criteria.
 * 
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 01:39:39 CEST
 */
class S4A_API stream_selector: virtual public gr::block
{
public:
	typedef boost::shared_ptr<stream_selector> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::stream_selector.
	 *
	 * To avoid accidental use of raw pointers, s4a::stream_selector's
	 * constructor is in a private implementation
	 * class. s4a::stream_selector::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int magnitude_min, int magnitude_max,
			int input_stream_length, int output_stream_length);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_STREAM_SELECTOR_H */

