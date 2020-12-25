/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CYCLIC_PREFIX_REMOVAL_H
#define INCLUDED_S4A_CYCLIC_PREFIX_REMOVAL_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Cyclic prefix removal
 *	
 * Removes the cyclic prefix extension from the input 
 * symbols and copies then the result to the output.
 *	  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Monday, 8 July 2013, 23:49:23 CEST
 */
class S4A_API cyclic_prefix_removal: virtual public gr::block
{
public:
	typedef boost::shared_ptr<cyclic_prefix_removal> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::cyclic_prefix_removal.
	 *
	 * To avoid accidental use of raw pointers, s4a::cyclic_prefix_removal's
	 * constructor is in a private implementation
	 * class. s4a::cyclic_prefix_removal::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int cp_length);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CYCLIC_PREFIX_REMOVAL_H */

