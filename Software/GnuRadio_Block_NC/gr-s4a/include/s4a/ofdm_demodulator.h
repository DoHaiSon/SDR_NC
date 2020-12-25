/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_DEMODULATOR_H
#define INCLUDED_S4A_OFDM_DEMODULATOR_H

#include <s4a/api.h>
#include <gnuradio/hier_block2.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM demodulator (hiearachical)
 *	
 * Hierarchical block for OFDM demodulator. 
 * It is made of \emph{OFDM synchronization}, 
 * \emph{Serial to Parallel}, \emph{Cyclic Prefix Removal}, 
 * \emph{Fast Fourier Transform} and \emph{OFDM Equalizer}.
 *	  
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Monday, 8 July 2013, 23:58:35 CEST
 */
class S4A_API ofdm_demodulator: virtual public gr::hier_block2
{
public:
	typedef boost::shared_ptr<ofdm_demodulator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::ofdm_demodulator.
	 *
	 * To avoid accidental use of raw pointers, s4a::ofdm_demodulator's
	 * constructor is in a private implementation
	 * class. s4a::ofdm_demodulator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, int timeout_ms,
			int cfo_sync_method, int verbose);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_DEMODULATOR_H */

