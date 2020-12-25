/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_ZERO_PADDING_H
#define INCLUDED_S4A_ZERO_PADDING_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Zero Padding
 *		  
 * Adds a certain amount of zeros at the end of incoming 
 * input symbols. The difference between the length of the 
 * input and output stream gives the amount of zeros needs 
 * to be inserted. This block is useful especially in bursty 
 * transmissions. It avoids keeping the symbols in the internal 
 * buffers of the SDR device such as USRP. Therefore, 
 * we are sure that the symbols are going to be transmitted.
 * 
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 02:58:09 CEST
 */
class S4A_API zero_padding: virtual public gr::block
{
public:
	typedef boost::shared_ptr<zero_padding> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::zero_padding.
	 *
	 * To avoid accidental use of raw pointers, s4a::zero_padding's
	 * constructor is in a private implementation
	 * class. s4a::zero_padding::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int input_stream_length, int output_stream_length);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_ZERO_PADDING_H */

