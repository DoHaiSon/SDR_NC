/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_SYNC_H
#define INCLUDED_S4A_VFDM_SYNC_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM timing and frequency synchronization
 *		
 * Timing and frequency synchronization for VFDM. 
 * The working mechanism is identical to OFDM 
 * synchronization block.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 02:50:04 CEST
 */
class S4A_API vfdm_sync: virtual public gr::block
{
public:
	typedef boost::shared_ptr<vfdm_sync> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_sync.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_sync's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_sync::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int preamble_block_rep, int pilot_block_t_sz,
			int pilot_block_rep, int data_block_sz, int data_block_rep,
			int silence_block_rep, int cfo_next_preamble_distance, int time_out,
			int cfo_sync_method);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_SYNC_H */

