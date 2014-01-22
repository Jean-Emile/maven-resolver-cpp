#include <network/http/impl/BoostHTTPClient.h>
#include <network/http/api/HTTPRequest.h>

using namespace network::http::impl;

int main(int argc, char **argv) {

	BoostHTTPClient * client = new BoostHTTPClient();

	network::http::api::HTTPRequest request;
	request.addHeader("User-Agent", "Kevoree");
	request.setUrl("http://www.google.fr");


	network::http::api::HTTPResponse * response = client->doGet(request);
//	std::cout << response->getContentAsString() << std::endl;

	delete response;

//	request.setUrl(
//			"http://oss.sonatype.org/service/local/repositories/releases/content/org/kevoree/org.kevoree.api/3.1.0/org.kevoree.api-3.1.0.pom");
//	response = client->doGet(request);
//	std::cout << response->getContentAsString() << std::endl;
//
//	delete response;

	delete client;

	/*request.setUrl(
	 "http://oss.sonatype.org/service/local/repositories/releases/content/org/kevoree/org.kevoree.api/3.1.0/org.kevoree.api-3.1.0.jar");
	 response = client->doGet(request);
	 std::cout << response->getContentAsString() << std::endl;*/

}
