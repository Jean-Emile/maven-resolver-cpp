#ifndef MAVENVERSIONRESOLVER_H_
#define MAVENVERSIONRESOLVER_H_

#include <string>
#include <fstream>

#include "maven-resolver/api/MavenArtefact.h"
#include "maven-resolver/api/MavenVersionResult.h"
#include "network/http/impl/BoostHTTPClient.h"

#define PATH_SEPARATOR "/" 

const std::string release = "release";
const std::string latest = "latest";

const std::string buildLatestTag = "<latest>";
const std::string buildEndLatestTag = "</latest>";

const std::string buildReleaseTag = "<release>";
const std::string buildEndreleaseTag = "</release>";

const std::string buildMavenTag = "<buildNumber>";
const std::string buildEndMavenTag = "</buildNumber>";

const std::string timestampMavenTag = "<timestamp>";
const std::string timestampEndMavenTag = "</timestamp>";

const std::string lastUpdatedMavenTag = "<lastUpdated>";
const std::string lastUpdatedEndMavenTag = "</lastUpdated>";

const std::string snapshotVersionClassifierMavenTag = "<classifier>";
const std::string snapshotVersionClassifierEndMavenTag = "</classifier>";

const std::string snapshotVersionExtensionMavenTag = "<extension>";
const std::string snapshotVersionExtensionEndMavenTag = "</extension>";

const std::string snapshotVersionValueMavenTag = "<value>";
const std::string snapshotVersionValueEndMavenTag = "</value>";

const std::string snapshotVersionUpdatedMavenTag = "<updated>";
const std::string snapshotVersionUpdatedEndMavenTag = "</updated>";

const std::string metaFile = "maven-metadata.xml";
const std::string localmetaFile = "maven-metadata-local.xml";

class MavenVersionResolver {
private:
	std::string cachePath;
	network::http::impl::BoostHTTPClient * client;
	maven_resolver::impl::MavenDownloader * mavenDownloader;

public:
	MavenVersionResolver();
	virtual ~MavenVersionResolver();

	std::string getCachePath();
	void setCachePath(std::string value);

	MavenVersionResult * resolveVersion(MavenArtefact artefact, std::string basePath, bool localDeploy);
	std::string foundRelevantVersion(MavenArtefact artefact, std::string basePath, std::string remoteURL, bool localDeploy);
	std::string buildCacheFilePath(MavenArtefact artefact, std::string basePath, std::string remoteURL);
};

#endif /*MAVENVERSIONRESOLVER_H_*/
