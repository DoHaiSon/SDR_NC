/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_SYNC_H
#define INCLUDED_S4A_OFDM_SYNC_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM time and frequency synchronization
 *		  
 * Estimates the timing offset of the OFDM frame and suppresses 
 * the effect of the carrier frequency offset. The output symbols 
 * will be time and frequency offset corrected. In details, the 
 * timing synchronization is done in two stages. In the coarse 
 * stage, the block uses Schmidl-Cox method. In case of the fine 
 * tuning which means the second step, the block applies 
 * auto-correlation to received symbols with the known preamble 
 * window. Since the received symbols contain the preamble part 
 * in the frame, it picks the argument of the best peak value. 
 * After the timing synchronization, the carrier frequency offset 
 * is suppressed.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 00:24:56 CEST
 */
class S4A_API ofdm_sync: virtual public gr::block
{
public:
	typedef boost::shared_ptr<ofdm_sync> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::ofdm_sync.
	 *
	 * To avoid accidental use of raw pointers, s4a::ofdm_sync's
	 * constructor is in a private implementation
	 * class. s4a::ofdm_sync::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int preamble_block_rep, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep,
			int cfo_next_preamble_distance, int timeout_ms,
			int cfo_sync_method);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_SYNC_H */

