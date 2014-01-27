/*
 * TestMavenResolver.cpp
 *
 *  Created on: 27 janv. 2014
 *      Author: jed
 */

#include "TestMavenResolver.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestMavenResolver );

TestMavenResolver::TestMavenResolver() {
	// TODO Auto-generated constructor stub

}

TestMavenResolver::~TestMavenResolver() {
	// TODO Auto-generated destructor stub
}



void TestMavenResolver::testResolver(){

	BoostHTTPClient * client = new BoostHTTPClient();

	network::http::HTTPRequest request;
	request.addHeader("User-Agent", "Kevoree");
	request.setUrl("http://www.google.fr");

	network::http::HTTPResponse * response = client->doGet(request);
	//std::cout << response->getContentAsString() << std::endl;
	CPPUNIT_ASSERT( response->getContentAsString() != "");

	delete response;


}
