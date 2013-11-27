#ifndef MAVENRESOLVER_H_
#define MAVENRESOLVER_H_

#include <string>
#include <stdio.h>
#include <list>
#include "MavenVersionResolver.h"
#include <fstream>
  
class MavenResolver
{
public:
	MavenResolver();
	virtual ~MavenResolver();
	std::string resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls);
	
private:
	MavenVersionResolver *versionResolver;
	std::string basePath;
};

#endif /*MAVENRESOLVER_H_*/
