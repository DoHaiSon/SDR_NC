/******************************************************************************
 *
 * 							SDR4All Toolbox (a.k.a Tools4SDR)
 *
 * 							http://www.flexible-radio.com
 *
 ******************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>

#include <gnuradio/unittests.h>
#include "qa_s4a.h"
#include <iostream>

int main(int argc, char **argv)
{
	CppUnit::TextTestRunner runner;
	std::ofstream xmlfile(get_unittest_path("s4a.xml").c_str());
	CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter(&runner.result(),
			xmlfile);

	runner.addTest(qa_s4a::suite());
	runner.setOutputter(xmlout);

	bool was_successful = runner.run("", false);

	return was_successful ? 0 : 1;
}
