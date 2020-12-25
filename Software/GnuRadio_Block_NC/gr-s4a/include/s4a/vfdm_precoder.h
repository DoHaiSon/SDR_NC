/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_PRECODER_H
#define INCLUDED_S4A_VFDM_PRECODER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM precoder
 *	
 * VFDM precoder using SVD from LAPACK library. 
 * It takes data symbols from the input port and the channel 
 * state information of the primary user from the \emph{h21} 
 * port. Then, it generates the frame by using the 
 * \emph{preamble}, \emph{pilot}, \emph{data} and \emph{silence} 
 * blocks and finally precodes all the symbols. Indeed, the 
 * preamble part is not precoded for experimental reasons.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 02:39:30 CEST
 */
class S4A_API vfdm_precoder: virtual public gr::block
{
public:
	typedef boost::shared_ptr<vfdm_precoder> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_precoder.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_precoder's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_precoder::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int channel_size, int preamble_block_rep,
			int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_PRECODER_H */

