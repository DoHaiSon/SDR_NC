/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_EQUALIZER_H
#define INCLUDED_S4A_VFDM_EQUALIZER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM equalizer
 *	
 * Applies zero forcing equalizer.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 01:50:07 CEST
 */
class S4A_API vfdm_equalizer: virtual public gr::block
{
public:
	typedef boost::shared_ptr<vfdm_equalizer> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_equalizer.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_equalizer's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_equalizer::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int preamble_block_rep, int pilot_block_t_sz,
			int pilot_block_rep, int data_block_sz, int data_block_rep,
			int silence_block_rep, int verbose);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_EQUALIZER_H */

