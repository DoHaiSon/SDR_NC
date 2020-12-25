/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_RANDOM_BIT_GENERATOR_H
#define INCLUDED_S4A_RANDOM_BIT_GENERATOR_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Random bit generator
 *	
 * Generates a pseudo-random bit sequence under the given 
 * initial seed. The purpose of the block is to generate complex 
 * symbol patterns for BER purposes. Since the pattern is known 
 * at the transmitter and the receiver, the BER can be 
 * calculated. It is better than defining custom sequences by 
 * hand on both sides.
 *  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 01:27:56 CEST
 */
class S4A_API random_bit_generator: virtual public gr::block
{
public:
	typedef boost::shared_ptr<random_bit_generator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::random_bit_generator.
	 *
	 * To avoid accidental use of raw pointers, s4a::random_bit_generator's
	 * constructor is in a private implementation
	 * class. s4a::random_bit_generator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int sequence_size, int seed, bool repeat);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_RANDOM_BIT_GENERATOR_H */

