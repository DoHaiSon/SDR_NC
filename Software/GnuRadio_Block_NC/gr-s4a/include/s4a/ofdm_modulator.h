/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_MODULATOR_H
#define INCLUDED_S4A_OFDM_MODULATOR_H

#include <s4a/api.h>
#include <gnuradio/hier_block2.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM modulator
 *		
 * Hierarchical block for the OFDM modulator. 
 * It contains Serial to Parallel, 
 * OFDM Frame Generation,
 * Inverse Fast Fourier Transform, 
 * Cyclic Prefix Insertion, and 
 * Parallel to Serial blocks.
 * 
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 00:15:21 CEST
 */
class S4A_API ofdm_modulator: virtual public gr::hier_block2
{
public:
	typedef boost::shared_ptr<ofdm_modulator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::ofdm_modulator.
	 *
	 * To avoid accidental use of raw pointers, s4a::ofdm_modulator's
	 * constructor is in a private implementation
	 * class. s4a::ofdm_modulator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, float preamble_coeff,
			float pilot_coeff, float data_coeff);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_MODULATOR_H */

