/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_QAM4_MAPPER_H
#define INCLUDED_S4A_QAM4_MAPPER_H

#include <s4a/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace s4a
{

/**
 * @brief QAM4 constellation mapper
 *		  
 * Takes the bits from the input buffer and converts them into 
 * the QAM4 symbols.
 *
 * @author Ejder Bastug <ejder.bastug@gmail.com>
 * @version 0.3
 * @date Tuesday, 9 July 2013, 00:57:21 CEST
 */
class S4A_API qam4_mapper: virtual public gr::block
{
public:
	typedef boost::shared_ptr<qam4_mapper> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of s4a::qam4_mapper.
	 *
	 * To avoid accidental use of raw pointers, s4a::qam4_mapper's
	 * constructor is in a private implementation
	 * class. s4a::qam4_mapper::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();
};

} // namespace s4a
} // namespace gr

#endif /* INCLUDED_S4A_QAM4_MAPPER_H */

