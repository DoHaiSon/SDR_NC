/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_CYCLIC_PREFIX_INSERTION_H
#define INCLUDED_S4A_CYCLIC_PREFIX_INSERTION_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Cyclic prefix insertion
 *		
 * Copies the last part of the input symbols to the
 * beginning of the symbols with respect to the 
 * specified cyclic prefix length.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Monday, 8 July 2013, 23:39:07 CEST
 */
class S4A_API cyclic_prefix_insertion: virtual public gr::block
{
public:
	typedef boost::shared_ptr<cyclic_prefix_insertion> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::cyclic_prefix_insertion.
	 *
	 * To avoid accidental use of raw pointers, s4a::cyclic_prefix_insertion's
	 * constructor is in a private implementation
	 * class. s4a::cyclic_prefix_insertion::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int cp_length);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_CYCLIC_PREFIX_INSERTION_H */

