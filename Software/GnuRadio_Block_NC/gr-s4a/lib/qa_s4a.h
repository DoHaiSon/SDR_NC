/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifndef _QA_S4A_H_
#define _QA_S4A_H_

#include <gnuradio/attributes.h>
#include <cppunit/TestSuite.h>

//! collect all the tests for the gr-s4a directory

class __GR_ATTR_EXPORT qa_s4a
{
public:
	//! return suite of tests for all of gr-s4a directory
	static CppUnit::TestSuite *suite();
};

#endif /* _QA_S4A_H_ */
