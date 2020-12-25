/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/

/*
 * This class gathers together all the test cases for the gr-s4a
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_s4a.h"

CppUnit::TestSuite *
qa_s4a::suite()
{
	CppUnit::TestSuite *s = new CppUnit::TestSuite("s4a");

	return s;
}
