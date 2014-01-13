/*
 * main.cpp
 *
 *  Created on: 6 janv. 2014
 *      Author: edaubert
 */

#include <maven-resolver/impl/MavenResolver.h>

int main(int argc, char **argv) {
	MavenResolver resolver;

	std::list<std::string> urls;
	urls.push_front("http://oss.sonatype.org/service/local/repositories/releases/content/");
	resolver.resolve("org.kevoree", "org.kevoree.api", "3.1.0", "jar", urls);
//		std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls
		"http://oss.sonatype.org/service/local/repositories/releases/content/org/kevoree/org.kevoree.api/3.1.0/org.kevoree.api-3.1.0.jar";
}

