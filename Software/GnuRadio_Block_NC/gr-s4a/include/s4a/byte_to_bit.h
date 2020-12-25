/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_BYTE_TO_BIT_H
#define INCLUDED_S4A_BYTE_TO_BIT_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Byte to Bit converter
 *		  
 * Generates byte stream from the input bit stream.
 * Input values should be 0 or 1.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 23:03:40 CEST
 */
class S4A_API byte_to_bit: virtual public gr::block
{
public:
	typedef boost::shared_ptr<byte_to_bit> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::byte_to_bit.
	 *
	 * To avoid accidental use of raw pointers, s4a::byte_to_bit's
	 * constructor is in a private implementation
	 * class. s4a::byte_to_bit::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_BYTE_TO_BIT_H */

