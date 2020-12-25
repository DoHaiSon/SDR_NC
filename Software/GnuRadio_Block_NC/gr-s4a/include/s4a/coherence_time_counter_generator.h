/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_COHERENCE_TIME_COUNTER_GENERATOR_H
#define INCLUDED_S4A_COHERENCE_TIME_COUNTER_GENERATOR_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief Counter packet generator for coherence time estimation.
 *		  
 * This block generates counter packets for coherence time 
 * estimation purposes. In order to estimate coherence time of 
 * the channel, the transmitter needs to generate a counter 
 * packet. Then, at the receiver, this counter packet is 
 * decoded by the coherence time estimator block. The coherence 
 * time estimator block checks correlation between previous 
 * estimated channel and actual channel. If the correlation 
 * coefficient (Pearson correlation  coefficient) is under 
 * the specified threshold value, it then computes   
 * difference of the counters. Since the transmitter sends 
 * frame  continuously and the number of symbols in a frame 
 * is known, the  difference between two counters gives us 
 * a distance metric in terms of  number of symbols. With 
 * this metric, coherence time can be calculated  if the 
 * RF bandwidth is given.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 23:13:47 CEST
 */
class S4A_API coherence_time_counter_generator: virtual public gr::block
{
public:
	typedef boost::shared_ptr<coherence_time_counter_generator> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::coherence_time_counter_generator.
	 *
	 * To avoid accidental use of raw pointers, s4a::coherence_time_counter_generator's
	 * constructor is in a private implementation
	 * class. s4a::coherence_time_counter_generator::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int packet_header_sz, int packet_header_val,
			int packet_counter_sz, int packet_rep, int packet_zero_padding);
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_COHERENCE_TIME_COUNTER_GENERATOR_H */

