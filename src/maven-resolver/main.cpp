/*
 * main.cpp
 *
 *  Created on: 6 janv. 2014
 *      Author: edaubert
 */

#include <maven-resolver/api/MavenResolver.h>

#include <iostream>
#include <iterator>
#include <string>
#include <boost/regex.hpp>

int main(int argc, char **argv) {
	maven::resolver::MavenResolver resolver;

	std::list<std::string> urls;
	urls.push_back("http://oss.sonatype.org/service/local/repositories/releases/content/");
	urls.push_back("http://oss.sonatype.org/content/repositories/snapshots");

	while (true) {
		std::cout << resolver.resolve("org.kevoree", "org.kevoree.api", "3.1.0", "jar", urls) << std::endl;
		std::cout << resolver.resolve("org.kevoree", "org.kevoree.api", "release", "jar", urls) << std::endl;

		std::cout << resolver.resolve("org.kevoree", "org.kevoree.api", "3.1.6-SNAPSHOT", "jar", urls) << std::endl;
		std::cout << resolver.resolve("org.kevoree.komponents", "http-webbit", "1.1.0-SNAPSHOT", "jar", urls) << std::endl;
		std::cout << resolver.resolve("org.kevoree.komponents", "http-webbit", "latest", "jar", urls) << std::endl;
	}
}

