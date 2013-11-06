#ifndef MAVENRESOLVER_H_
#define MAVENRESOLVER_H_

#include <string>
#include <stdio.h>
#include <vector>
#include "MavenVersionResolver.h"



  
  
class MavenResolver
{
public:
	MavenResolver();
	virtual ~MavenResolver();
	FILE * resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::vector<std::string> urls);
	
private:
	MavenVersionResolver *versionResolver;
	std::string basePath;
};

#endif /*MAVENRESOLVER_H_*/
