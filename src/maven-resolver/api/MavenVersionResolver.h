#ifndef MAVENVERSIONRESOLVER_H_
#define MAVENVERSIONRESOLVER_H_

#include <string>
#include <fstream>

#include "maven-resolver/api/MavenConstants.h"
#include "maven-resolver/api/MavenArtefact.h"
#include "maven-resolver/api/MavenVersionResult.h"
#include "maven-resolver/api/MavenVersionComparator.h"
#include "maven-resolver/api/MavenDownloader.h"

namespace maven {
namespace resolver {

class MavenVersionResolver {
private:
	MavenDownloader * mavenDownloader;
	MavenVersionComparator versionComparator;

	MavenVersionResult* resolveVersionFromLocal(MavenArtefact artefact, std::string metaFileContent, bool release, bool latest);
	MavenVersionResult* resolveVersionFromRemote(MavenArtefact artefact, std::string metaFileContent, bool release, bool latest);

public:
	MavenVersionResolver();
	virtual ~MavenVersionResolver();

	std::string getCachePath();
	void setCachePath(std::string value);

	MavenVersionResult * resolveVersion(MavenArtefact artefact, std::string cachePath, std::string remoteURL, bool localDeploy);
	MavenVersionResult* foundRelevantVersion(MavenArtefact artefact, std::string basePath, std::string remoteURL, bool localDeploy);
	std::string buildCacheFilePath(MavenArtefact artefact, std::string basePath, std::string remoteURL, bool localDeploy);
};
}
}

#endif /*MAVENVERSIONRESOLVER_H_*/
