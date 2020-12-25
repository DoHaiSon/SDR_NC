/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_QAM4_DEMAPPER_H
#define INCLUDED_S4A_QAM4_DEMAPPER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief BPSK constellation demapper
 * 
 * Takes complex QAM4 symbols from the input and convert 
 * them into the bits.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.2
 * @date Tuesday, 9 July 2013, 00:52:38 CEST
 *
 */
class S4A_API qam4_demapper: virtual public gr::block
{
public:
	typedef boost::shared_ptr<qam4_demapper> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::qam4_demapper.
	 *
	 * To avoid accidental use of raw pointers, s4a::qam4_demapper's
	 * constructor is in a private implementation
	 * class. s4a::qam4_demapper::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_QAM4_DEMAPPER_H */

