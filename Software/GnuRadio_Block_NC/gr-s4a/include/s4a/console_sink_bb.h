/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CONSOLE_SINK_BB_H
#define INCLUDED_S4A_CONSOLE_SINK_BB_H

#include <s4a/api.h>
#include <gnuradio/block.h>
#include <vector>
#include <sys/time.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Console sink
 *	
 * Prints the input stream to the console (terminal). 
 * In some cases, using file sinks is impractical. 
 * Hence, we can simply print the input stream to the 
 * console with a specified format. On the other hand, 
 * file sinks only saves if  a certain amount of data 
 * is buffered. In our case, it is not mandatory.
 *  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 23:25:49 CEST
 */
class S4A_API console_sink_bb: virtual public gr::block
{
public:
	typedef boost::shared_ptr<console_sink_bb> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::console_sink_bb.
	 *
	 * To avoid accidental use of raw pointers, s4a::console_sink_bb's
	 * constructor is in a private implementation
	 * class. s4a::console_sink_bb::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(std::string sequence_name, int sequence_length,
			int print_rate);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CONSOLE_SINK_BB_H */

