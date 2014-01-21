/*
 * MavenConstants.h
 *
 *  Created on: 13 janv. 2014
 *      Author: edaubert
 */

#ifndef MAVENCONSTANTS_H_
#define MAVENCONSTANTS_H_

namespace maven_resolver {
namespace api {
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

const std::string SNAPSHOT_VERSION_END = "-SNAPSHOT";

}
}

#endif /* MAVENCONSTANTS_H_ */
