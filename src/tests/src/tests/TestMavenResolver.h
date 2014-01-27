/*
 * TestMavenResolver.h
 *
 *  Created on: 27 janv. 2014
 *      Author: jed
 */

#ifndef TESTMAVENRESOLVER_H_
#define TESTMAVENRESOLVER_H_

#include <cppunit/extensions/HelperMacros.h>

#include <network/http/impl/BoostHTTPClient.h>
#include <network/http/api/HTTPRequest.h>

#include <maven-resolver/api/MavenResolver.h>

#include <iostream>
#include <iterator>
#include <string>
#include <boost/regex.hpp>

using namespace network::http;

class TestMavenResolver : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( TestMavenResolver );
	CPPUNIT_TEST(testResolver);
	CPPUNIT_TEST_SUITE_END();
public:
	TestMavenResolver();
	virtual ~TestMavenResolver();
	void testResolver();
};

#endif /* TESTMAVENRESOLVER_H_ */
