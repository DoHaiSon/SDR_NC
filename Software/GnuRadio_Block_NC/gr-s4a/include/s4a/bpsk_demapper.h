/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_BPSK_DEMAPPER_H
#define INCLUDED_S4A_BPSK_DEMAPPER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief BPSK constellation demapper
 *		  
 * Takes complex BPSK symbols from the input
 * and convert them to the bits. In particular,
 * It checks the sign of the complex input symbols
 * and makes a hard decision.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Monday, 8 July 2013, 22:55:14 CEST
 *
 */
class S4A_API bpsk_demapper: virtual public gr::block
{
public:
	typedef boost::shared_ptr<bpsk_demapper> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::bpsk_demapper.
	 *
	 * To avoid accidental use of raw pointers, s4a::bpsk_demapper's
	 * constructor is in a private implementation
	 * class. s4a::bpsk_demapper::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_BPSK_DEMAPPER_H */

