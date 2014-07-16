maven-resolver-cpp
==================

maven URL resolver in C++

A minimal but efficient and portable (self-contained) maven URL resolver in C++,
it is a child project of  : https://github.com/dukeboard/basic-maven-resolver

Continuous integration : https://drone.io/github.com/Jean-Emile/maven-resolver-cpp



Getting started : 

MavenResolver resolver;

std::list<std::string> urls;
urls.push_back("http://oss.sonatype.org/service/local/repositories/releases/content/");
urls.push_back("http://oss.sonatype.org/content/repositories/snapshots");

resolver.resolve("org.kevoree", "org.kevoree.api", "3.1.0", "jar", urls);
