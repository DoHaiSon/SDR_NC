/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef INCLUDED_S4A_API_H
#define INCLUDED_S4A_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_s4a_EXPORTS
#  define S4A_API __GR_ATTR_EXPORT
#else
#  define S4A_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_S4A_API_H */
