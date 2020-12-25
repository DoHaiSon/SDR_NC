/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_COHERENCE_TIME_ESTIMATOR_H
#define INCLUDED_S4A_COHERENCE_TIME_ESTIMATOR_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Coherence time estimator
 *
 * Estimates the coherence time of the channel in
 * seconds. Takes channel information from the input \emph{h} 
 * and calculates Pearson correlation coefficient each 
 * time by comparing with the previous channel. If the 
 * coefficient is under a certain threshold, then it 
 * checks \ac{SNR} and header of the packet. When all 
 * these conditions are satisfied, it reads the counter 
 * to take difference between the current and previous one. 
 * The difference between two counters (the previous and 
 * the actual one) gives a distance metric in terms of number 
 * of symbols. Since the RF bandwidth is known, it calculates 
 * coherence time.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 23:20:16 CEST
 */
class S4A_API coherence_time_estimator: virtual public gr::block
{
public:
	typedef boost::shared_ptr<coherence_time_estimator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::coherence_time_estimator.
	 *
	 * To avoid accidental use of raw pointers, s4a::coherence_time_estimator's
	 * constructor is in a private implementation
	 * class. s4a::coherence_time_estimator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int pearson_correlation, int output_mode, int data_sz,
			int channel_sz, int data_block_rep, int check_sz,
			float r_differentiation, float snr_threshold, int signal_bw,
			int frame_sz, int packet_header_sz, int packet_header_val,
			int packet_counter_sz, int packet_rep, int packet_zero_padding);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_COHERENCE_TIME_ESTIMATOR_H */

