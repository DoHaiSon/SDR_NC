/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_SERIAL_TO_PARALLEL_H
#define INCLUDED_S4A_SERIAL_TO_PARALLEL_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Serial to parallel conversion
 *	
 * Reshapes the parallel complex stream into serial form.
 *	  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 01:36:02 CEST
 */
class S4A_API serial_to_parallel: virtual public gr::block
{
public:
	typedef boost::shared_ptr<serial_to_parallel> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::serial_to_parallel.
	 *
	 * To avoid accidental use of raw pointers, s4a::serial_to_parallel's
	 * constructor is in a private implementation
	 * class. s4a::serial_to_parallel::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int parallel_size);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_SERIAL_TO_PARALLEL_H */

