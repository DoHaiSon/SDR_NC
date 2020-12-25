/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_MODULATOR_H
#define INCLUDED_S4A_VFDM_MODULATOR_H

#include <s4a/api.h>
#include <gnuradio/hier_block2.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM modulator (hierarchical)
 *		
 * Hierarchical block for VFDM modulator. 
 * It is made of \emph{Serial to Parallel},
 * \emph{VFDM Precoder}, \emph{VFDM Power Scaling} 
 * and \emph{Parallel to Serial} blocks.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.1
 * @date Thu 05 Jan 2012 02:48:41 PM CET
 */
class S4A_API vfdm_modulator: virtual public gr::hier_block2
{
public:
	typedef boost::shared_ptr<vfdm_modulator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_modulator.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_modulator's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_modulator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int occupied_tones, int cp_length,
			int precoder_length, int channel_size, int preamble_block_rep,
			int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep, float preamble_coeff,
			float pilot_coeff, float data_coeff);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_MODULATOR_H */

