/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_FRAME_GENERATION_H
#define INCLUDED_S4A_FRAME_GENERATION_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM frame generation
 *
 * Generates an OFDM frame. Basically, it prepares the 
 * frame which is made of preamble, data, pilot and silence blocks. 
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Mon 12 Mar 2012 03:32:13 PM CET
 */
class S4A_API frame_generation: virtual public gr::block
{
public:
	typedef boost::shared_ptr<frame_generation> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::frame_generation.
	 *
	 * To avoid accidental use of raw pointers, s4a::frame_generation's
	 * constructor is in a private implementation
	 * class. s4a::frame_generation::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int preamble_block_rep,
			int pilot_block_rep, int data_block_sz, int data_block_rep,
			int silence_block_rep);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_FRAME_GENERATION_H */

