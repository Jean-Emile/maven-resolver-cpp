#ifndef MAVENVERSIONRESOLVER_H_
#define MAVENVERSIONRESOLVER_H_

#include "MavenArtefact.h"
#include "MavenVersionResult.h"
#include <string>

#define buildLatestTag  "<latest>"
#define buildEndLatestTag  "</latest>"

#define buildReleaseTag  "<release>"
#define buildEndreleaseTag  "</release>"

#define buildMavenTag  "<buildNumber>"
#define buildEndMavenTag  "</buildNumber>"

#define timestampMavenTag  "<timestamp>"
#define timestampEndMavenTag  "</timestamp>"

#define lastUpdatedMavenTag  "<lastUpdated>"
#define lastUpdatedEndMavenTag  "</lastUpdated>"

#define snapshotVersionClassifierMavenTag  "<classifier>"
#define snapshotVersionClassifierEndMavenTag  "</classifier>"

#define snapshotVersionExtensionMavenTag  "<extension>"
#define snapshotVersionExtensionEndMavenTag  "</extension>"

#define snapshotVersionValueMavenTag  "<value>"
#define snapshotVersionValueEndMavenTag  "</value>"

#define snapshotVersionUpdatedMavenTag  "<updated>"
#define snapshotVersionUpdatedEndMavenTag  "</updated>"

#define metaFile "maven-metadata.xml"
#define localmetaFile  "maven-metadata-local.xml"


#define PATH_SEPARATOR "/" 



class MavenVersionResolver
{
public:
	MavenVersionResolver();
	virtual ~MavenVersionResolver();
	MavenVersionResult* resolveVersion(MavenArtefact artefact, std::string basePath, bool localDeploy);
	std::string foundRelevantVersion(MavenArtefact artefact, std::string basePath, bool localDeploy);
};

#endif /*MAVENVERSIONRESOLVER_H_*/
