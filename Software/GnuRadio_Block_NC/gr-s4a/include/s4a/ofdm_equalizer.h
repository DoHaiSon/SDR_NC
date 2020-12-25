/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_EQUALIZER_H
#define INCLUDED_S4A_OFDM_EQUALIZER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM equalizer
 *
 * Uses zero-forcing equalizer to remove the channel 
 * effect. In case of repeated pilot blocks, it estimates 
 * the zero forcing vector from each pilot block and takes 
 * the average.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 00:03:47 CEST
 */
class S4A_API ofdm_equalizer: virtual public gr::block
{
public:
	typedef boost::shared_ptr<ofdm_equalizer> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::ofdm_equalizer.
	 *
	 * To avoid accidental use of raw pointers, s4a::ofdm_equalizer's
	 * constructor is in a private implementation
	 * class. s4a::ofdm_equalizer::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, int verbose);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_EQUALIZER_H */

