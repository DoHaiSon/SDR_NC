/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CSI_CONVERTER_H
#define INCLUDED_S4A_CSI_CONVERTER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Channel state information (CSI) converter
 *		
 * The inputs of this block need to be data symbols and 
 * channel transfer function respectively. If the channel 
 * transfer function is valid, then, it converts the transfer 
 * function to impulse response (in time domain) and also 
 * redirects the input symbols to the output. This block is 
 * useful when we want to feed after VFDM kind of 
 * precoders that requires channel impulse response as a 
 * feedback parameter. If the channel transfer function is 
 * not valid, it does not produce any output item such that 
 * the precoder block is not executed by the GNURadio scheduler.  
 *  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 23:33:27 CEST
 */
class S4A_API csi_converter: virtual public gr::block
{
public:
	typedef boost::shared_ptr<csi_converter> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::csi_converter.
	 *
	 * To avoid accidental use of raw pointers, s4a::csi_converter's
	 * constructor is in a private implementation
	 * class. s4a::csi_converter::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int data_syms_length, int channel_size);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CSI_CONVERTER_H */

