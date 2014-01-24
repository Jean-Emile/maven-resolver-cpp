/*
 * BoostHTTPClient.cpp
 *
 *  Created on: 13 déc. 2013
 *      Author: edaubert
 */
#ifndef BOOSTHTTPCLIENT_INCLUDED
#define BOOSTHTTPCLIENT_INCLUDED

#include <boost/asio.hpp>

#include <network/http/api/HTTPClient.h>

namespace network {
namespace http {

class BoostHTTPClient: public HTTPClient {
private:
	boost::asio::io_service io_service;

public:
	BoostHTTPClient();
	~BoostHTTPClient();
	bool isHTTPURL(std::string const& url);
	HTTPResponse * doGet(HTTPRequest const& request);

private:
	std::string * parseURL(std::string const& url, std::string parsedResults[]);
};
}
}

#endif // BOOSTHTTPCLIENT_INCLUDED
