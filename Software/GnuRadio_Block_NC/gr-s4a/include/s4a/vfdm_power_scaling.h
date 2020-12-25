/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_VFDM_POWER_SCALING_H
#define INCLUDED_S4A_VFDM_POWER_SCALING_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief VFDM power scaling
 *		
 * Normalizes the power of the frame. Since the frame is made 
 * of the \emph{preamble}, \emph{pilot}, \emph{data} and 
 * \emph{silence blocks}, it multiplies each part by the given 
 * coefficients.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 02:32:16 CEST
 */
class S4A_API vfdm_power_scaling: virtual public gr::block
{
public:
	typedef boost::shared_ptr<vfdm_power_scaling> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::vfdm_power_scaling.
	 *
	 * To avoid accidental use of raw pointers, s4a::vfdm_power_scaling's
	 * constructor is in a private implementation
	 * class. s4a::vfdm_power_scaling::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, int precoder_length, float preamble_coeff,
			float pilot_coeff, float data_coeff, int preamble_block_rep,
			int pilot_block_t_sz, int pilot_block_rep, int data_block_sz,
			int data_block_rep, int silence_block_rep);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_VFDM_POWER_SCALING_H */

