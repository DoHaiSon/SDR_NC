/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_DEMODULATOR_H
#define INCLUDED_S4A_VFDM_DEMODULATOR_H

#include <s4a/api.h>
#include <gnuradio/hier_block2.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM demodulator (Hierarchical)
 *		  
 * Hierarchical block for VFDM demodulator. It contains 
 * \emph{VFDM Synchronization}, \emph{Serial to Parallel}, 
 * \emph{Cyclic Prefix Removal}, \emph{Fast Fourier Transform}, 
 * \emph{VFDM Synchronization}, \emph{VFDM Equalizer} and 
 * \emph{Parallel to Serial} blocks.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 01:44:11 CEST
 */
class S4A_API vfdm_demodulator: virtual public gr::hier_block2
{
public:
	typedef boost::shared_ptr<vfdm_demodulator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_demodulator.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_demodulator's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_demodulator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int preamble_block_rep, int pilot_block_t_sz,
			int pilot_block_rep, int data_block_sz, int data_block_rep,
			int silence_block_rep, int time_out, int cfo_sync_method, int verbose);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_DEMODULATOR_H */

