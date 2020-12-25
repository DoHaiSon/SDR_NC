/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_OFDM_POWER_SCALING_H
#define INCLUDED_S4A_OFDM_POWER_SCALING_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief OFDM power scaling
 *		  
 * Multiplies symbols in preamble, pilot and data blocks with 
 * the given constants respectively.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 00:20:01 CEST
 */
class S4A_API ofdm_power_scaling: virtual public gr::block
{
public:
	typedef boost::shared_ptr<ofdm_power_scaling> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::ofdm_power_scaling.
	 *
	 * To avoid accidental use of raw pointers, s4a::ofdm_power_scaling's
	 * constructor is in a private implementation
	 * class. s4a::ofdm_power_scaling::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int fft_length, float preamble_coeff, float pilot_coeff,
			float data_coeff, int preamble_block_rep, int pilot_block_rep,
			int data_block_sz, int data_block_rep, int silence_block_rep);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_OFDM_POWER_SCALING_H */

