#ifndef MAVENRESOLVER_H_
#define MAVENRESOLVER_H_

#include <string>
#include <list>

#include <maven-resolver/api/MavenArtefact.h>
#include <maven-resolver/api/MavenVersionComparator.h>
#include <maven-resolver/impl/MavenVersionResolver.h>
#include <maven-resolver/impl/MavenDownloader.h>

class MavenResolver {
public:
	MavenResolver();
	virtual ~MavenResolver();
	std::string resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls);
	std::string resolve(maven_resolver::api::MavenArtefact artefact, std::list<std::string> urls);
	std::string getArtefactLocalBasePath(maven_resolver::api::MavenArtefact artefact);

private:
	maven_resolver::impl::MavenVersionResolver *versionResolver;
	maven_resolver::api::MavenVersionComparator * versionComparator;
	std::string basePath;
	maven_resolver::impl::MavenDownloader * mavenDownloader;
};

#endif /*MAVENRESOLVER_H_*/
