#ifndef MAVENRESOLVER_H_
#define MAVENRESOLVER_H_

#include <string>
#include <list>

#include <maven-resolver/api/MavenArtefact.h>
#include <maven-resolver/api/MavenVersionComparator.h>
#include <maven-resolver/api/MavenVersionResolver.h>
#include <maven-resolver/api/MavenDownloader.h>

namespace maven {
namespace resolver {
class MavenResolver {
public:
	MavenResolver();
	virtual ~MavenResolver();
	std::string resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls);
	std::string resolve(MavenArtefact artefact, std::list<std::string> urls);
	std::string getArtefactLocalBasePath(MavenArtefact artefact);
	void setBasePath(std::string path);

private:
	MavenVersionResolver *versionResolver;
	std::string basePath;
	MavenDownloader * mavenDownloader;
};
}
}

#endif /*MAVENRESOLVER_H_*/
