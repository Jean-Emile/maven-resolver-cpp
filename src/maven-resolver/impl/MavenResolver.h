#ifndef MAVENRESOLVER_H_
#define MAVENRESOLVER_H_

#include <string>
#include <list>
#include "maven-resolver/api/MavenVersionComparator.h"
#include "maven-resolver/impl/MavenVersionResolver.h"
#include <maven-resolver/impl/MavenDownloader.h>
  
const std::string SNAPSHOT_VERSION_END = "-SNAPSHOT";
class MavenResolver
{
public:
	MavenResolver();
	virtual ~MavenResolver();
	std::string resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls);
	std::string getArtefactLocalBasePath(MavenArtefact artefact);
	
private:
	MavenVersionResolver *versionResolver;
	maven_resolver::api::MavenVersionComparator * versionComparator;
	std::string basePath;
	maven_resolver::impl::MavenDownloader * mavenDownloader;
};

#endif /*MAVENRESOLVER_H_*/
