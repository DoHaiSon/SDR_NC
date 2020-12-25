/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_BPSK_MAPPER_H
#define INCLUDED_S4A_BPSK_MAPPER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief BPSK constellation mapper
 *		  
 * Takes bits from its input buffer and converts
 * into complex BPSK symbols. Basically, if the
 * input bit is 0, it returns 1+0i. If the
 * input bit is 1, it then returns -1+0i as result.
 * The alphabet C = {1+0i, -1+0i} is defined
 * arbitrary and can be changed programmatically.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 22:59:46 CEST
 */
class S4A_API bpsk_mapper: virtual public gr::block
{
public:
	typedef boost::shared_ptr<bpsk_mapper> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::bpsk_mapper.
	 *
	 * To avoid accidental use of raw pointers, s4a::bpsk_mapper's
	 * constructor is in a private implementation
	 * class. s4a::bpsk_mapper::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_BPSK_MAPPER_H */

