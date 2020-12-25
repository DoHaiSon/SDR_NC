/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_PARALLEL_TO_SERIAL_H
#define INCLUDED_S4A_PARALLEL_TO_SERIAL_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Parallel to serial conversion
 *
 * Takes complex symbols from its input and then convert 
 * the symbols into parallel stream. The block does not  
 * change behavior of symbols. It simply buffers the input 
 * and then gives the output with respect to the specified 
 * length.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 00:34:41 CEST
 */
class S4A_API parallel_to_serial: virtual public gr::block
{
public:
	typedef boost::shared_ptr<parallel_to_serial> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::parallel_to_serial.
	 *
	 * To avoid accidental use of raw pointers, s4a::parallel_to_serial's
	 * constructor is in a private implementation
	 * class. s4a::parallel_to_serial::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int parallel_size);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_PARALLEL_TO_SERIAL_H */

