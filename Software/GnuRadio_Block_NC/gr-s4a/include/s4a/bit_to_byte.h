/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_BIT_TO_BYTE_H
#define INCLUDED_S4A_BIT_TO_BYTE_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Bit to Byte converter
 *		  
 * Generates byte stream from the input bit stream.
 * Input values should be 0 or 1.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 22:22:53 CEST
 */
class S4A_API bit_to_byte: virtual public gr::block
{
public:
	typedef boost::shared_ptr<bit_to_byte> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::bit_to_byte.
	 *
	 * To avoid accidental use of raw pointers, s4a::bit_to_byte's
	 * constructor is in a private implementation
	 * class. s4a::bit_to_byte::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_BIT_TO_BYTE_H */

