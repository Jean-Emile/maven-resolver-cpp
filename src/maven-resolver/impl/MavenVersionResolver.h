#ifndef MAVENVERSIONRESOLVER_H_
#define MAVENVERSIONRESOLVER_H_

#include <string>
#include <fstream>

#include "maven-resolver/api/MavenConstants.h"
#include "maven-resolver/api/MavenArtefact.h"
#include "maven-resolver/api/MavenVersionResult.h"
#include "maven-resolver/api/MavenVersionComparator.h"
#include "maven-resolver/impl/MavenDownloader.h"

namespace maven_resolver {
namespace impl {

class MavenVersionResolver {
private:
	maven_resolver::impl::MavenDownloader * mavenDownloader;
	maven_resolver::api::MavenVersionComparator versionComparator;

	maven_resolver::api::MavenVersionResult* resolveVersionFromLocal(maven_resolver::api::MavenArtefact artefact, std::string metaFileContent, bool release, bool latest);
	maven_resolver::api::MavenVersionResult* resolveVersionFromRemote(maven_resolver::api::MavenArtefact artefact, std::string metaFileContent, bool release, bool latest);

public:
	MavenVersionResolver();
	virtual ~MavenVersionResolver();

	std::string getCachePath();
	void setCachePath(std::string value);

	maven_resolver::api::MavenVersionResult * resolveVersion(maven_resolver::api::MavenArtefact artefact, std::string cachePath, std::string remoteURL, bool localDeploy);
	maven_resolver::api::MavenVersionResult* foundRelevantVersion(maven_resolver::api::MavenArtefact artefact, std::string basePath, std::string remoteURL, bool localDeploy);
	std::string buildCacheFilePath(maven_resolver::api::MavenArtefact artefact, std::string basePath, std::string remoteURL, bool localDeploy);
};
}
}

#endif /*MAVENVERSIONRESOLVER_H_*/
