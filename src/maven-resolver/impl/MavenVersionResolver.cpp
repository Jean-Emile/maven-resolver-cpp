#include "maven-resolver/api/MavenVersionResolver.h"

#include <boost/regex.hpp>
#include <fstream>

#include "maven-resolver/api/Utilities.h"
#include "network/http/api/HTTP.h"
#include "network/http/api/Status.h"
namespace maven {
namespace resolver {

MavenVersionResolver::MavenVersionResolver() {
	mavenDownloader = new MavenDownloader();
}

MavenVersionResolver::~MavenVersionResolver() {
	delete mavenDownloader;
}

MavenVersionResult* MavenVersionResolver::resolveVersion(MavenArtefact artefact, std::string cachePath,
		std::string remoteURL, bool localDeploy) {
	MavenVersionResult *result = NULL;
	std::string fileContent;

	std::string filePath = buildCacheFilePath(artefact, cachePath, remoteURL, localDeploy);

	if (!localDeploy) {
		if (std::fstream(filePath.c_str()).fail()) {
		//	std::cout << "Creating directories to store metadata file: " << filePath << std::endl;
			mkdirs(filePath);
		}
		std::list<std::string> repo;
		repo.push_back(remoteURL);
	//	std::cout << "Trying to resolve metaData \"" << filePath << "\" on " << remoteURL << std::endl;
		mavenDownloader->downloadMetadata(filePath, artefact, artefact.getVersion(), repo);
	}
	//std::cout << "Metadata file to read: " << filePath << std::endl;

	fileContent = readFile(filePath);

//	std::cout << fileContent << std::endl;

	if (fileContent.compare("") != 0) {
		if (localDeploy) {
//			std::cout << "Trying the resolution from local information" << std::endl;
			result = resolveVersionFromLocal(artefact, fileContent, false, false);
		} else {
//			std::cout << "Trying the resolution from remote information" << std::endl;
			result = resolveVersionFromRemote(artefact, fileContent, false, false);
		}
	}

	if (result != NULL) {
		if (localDeploy) {
			result->setUrl_origin(cachePath);
			result->setNotDeployed(localDeploy);
		} else {
			result->setUrl_origin(remoteURL);
			result->setNotDeployed(localDeploy);
		}
//		std::cout << "Return a resolved version: " << result->getValue() << " on " << result->getUrl_origin() << std::endl;
	}

//	std::cout << "Is there a result ? " << result << std::endl;

	return result;
}

MavenVersionResult* MavenVersionResolver::foundRelevantVersion(MavenArtefact artefact, std::string cachePath,
		std::string remoteURL, bool localDeploy) {
	std::string askedVersion = toLower(artefact.getVersion());

	bool release = false;
	bool latest = false;
	if (askedVersion.compare("release") == 0) {
		release = true;
	}
	if (askedVersion.compare("latest") == 0) {
		latest = true;
	}
	if (!release && !latest) {
		return NULL;
	}

	std::string cacheFilePath = buildCacheFilePath(artefact, cachePath, remoteURL, localDeploy);
	std::fstream cacheFile(cacheFilePath.c_str());
	if (cacheFile.fail()) {
		mkdirs(cacheFilePath);
	}
	std::string buffer;
	if (!localDeploy) {
		std::list<std::string> repoUrl;
		repoUrl.push_back(remoteURL);
//		std::cout << "Trying to resolve metaData \"" << cacheFilePath << "\" on " << remoteURL << std::endl;
		mavenDownloader->downloadMetadata(cacheFilePath, artefact, askedVersion, repoUrl);
	}

//	std::cout << "Metadata file to read: " << cacheFilePath << std::endl;

	buffer = readFile(cacheFilePath);

//	std::cout << buffer << std::endl;

	MavenVersionResult* version = NULL;
	if (buffer.length() != 0) {
		if (localDeploy) {
			version = resolveVersionFromLocal(artefact, buffer, release, latest);
		} else {
			version = resolveVersionFromRemote(artefact, buffer, release, latest);
			if (version == NULL) {
				// unable to resolve from remote, try local resolution
				version = resolveVersionFromLocal(artefact, buffer, release, latest);
			}
		}
	}
	return version;
}

std::string MavenVersionResolver::buildCacheFilePath(MavenArtefact artefact, std::string basePath, std::string remoteURL,
		bool localDeploy) {
	std::string filePath;
	filePath.append(basePath);
	filePath.append(PATH_SEPARATOR);

	std::string newString = artefact.getGroup();
	newString = replace(newString, ".", fileSeparator);

	filePath.append(newString);
	filePath.append(PATH_SEPARATOR);
	filePath.append(artefact.getName());
	filePath.append(PATH_SEPARATOR);

	std::string askedVersion = toLower(artefact.getVersion());
	if (askedVersion.compare("release") != 0 && askedVersion.compare("latest") != 0) {
		filePath.append(artefact.getVersion());
		filePath.append(PATH_SEPARATOR);
	}

	if (localDeploy) {
		filePath.append(localmetaFile);
	} else {
		filePath.append(metaFile);
		filePath.append("-");

		newString = remoteURL;
		newString = replace(newString, "/", "_");
		newString = replace(newString, ":", "_");
		newString = replace(newString, ".", "_");
		filePath.append(newString);
	}

//	std::cout << "cacheFilePath to use: " << filePath << std::endl;
	return filePath;
}

MavenVersionResult* MavenVersionResolver::resolveVersionFromLocal(MavenArtefact artefact,
		std::string metaFileContent, bool release, bool latest) {
	MavenVersionResult* version = NULL;

	std::string versionString = "<version>(\\d+(\\.\\d+)?(\\.\\d+)?(-\\w+)?)</version>";
	boost::regex versionRegex;

	versionRegex = versionString;

	std::string::const_iterator startVersion = metaFileContent.begin();
	std::string::const_iterator endVersion = metaFileContent.end();
	boost::match_results<std::string::const_iterator> whatVersion;
	boost::match_flag_type flags = boost::match_default;

	while (boost::regex_search(startVersion, endVersion, whatVersion, versionRegex, flags)) {
		std::string loopVersion = whatVersion[1].str();

//		std::cout << loopVersion << std::endl;

		trim(loopVersion);
		if (release) {
			if (toLower(loopVersion).find(SNAPSHOT_VERSION_END) == std::string::npos) {
				if (version == NULL) {
					version = new MavenVersionResult();
					version->setValue("0");
				}
//				std::cout << "Setting value between " << version->getValue() << " and " << loopVersion << std::endl;
				version->setValue(versionComparator.max(version->getValue(), loopVersion));
			}
		} else {
			if (version == NULL) {
				version = new MavenVersionResult();
				version->setValue("0");
			}
//			std::cout << "Setting value between " << version->getValue() << " and " << loopVersion << std::endl;
			version->setValue(versionComparator.max(version->getValue(), loopVersion));
		}
		startVersion = whatVersion[0].second;
	}

//	if (version != NULL) {
//		std::cout << "Returning version from local: " << version << " " << version->getValue() << std::endl;
//	}
	return version;
}

MavenVersionResult* MavenVersionResolver::resolveVersionFromRemote(MavenArtefact artefact,
		std::string metaFileContent, bool release, bool latest) {
	MavenVersionResult* version = NULL;
	if (release) {
		int buildReleaseTagIndex = metaFileContent.find(buildReleaseTag);
		int buildEndreleaseTagIndex = metaFileContent.find(buildEndreleaseTag);
		if (buildReleaseTagIndex != std::string::npos && buildEndreleaseTagIndex != std::string::npos) {
			version = new MavenVersionResult();
			version->setValue(
					metaFileContent.substr(buildReleaseTagIndex + buildReleaseTag.length(),
							buildEndreleaseTagIndex - (buildReleaseTagIndex + buildReleaseTag.length())));
		}
	} else if (latest) {
		int buildLatestTagIndex = metaFileContent.find(buildLatestTag);
		int buildEndLatestTagIndex = metaFileContent.find(buildEndLatestTag);
		if (buildLatestTagIndex != std::string::npos && buildEndLatestTagIndex != std::string::npos) {
			version = new MavenVersionResult();
			version->setValue(
					metaFileContent.substr(buildLatestTagIndex + buildLatestTag.length(),
							buildEndLatestTagIndex - (buildLatestTagIndex + buildLatestTag.length())));
		}
	} else {
		std::string snapshotVersionsString = "<snapshotVersion>(\\s|(?!(</snapshotVersion>)).)*</snapshotVersion>";
		boost::regex snapshotVersionsRegex;
		snapshotVersionsRegex = snapshotVersionsString;

		std::string::const_iterator snapshotVersionStart = metaFileContent.begin();
		std::string::const_iterator snapshotVersionEnd = metaFileContent.end();
		boost::match_results<std::string::const_iterator> whatSnapshotVersions;
		boost::match_flag_type flags = boost::match_default;
		while (boost::regex_search(snapshotVersionStart, snapshotVersionEnd, whatSnapshotVersions, snapshotVersionsRegex, flags) && version == NULL) {
			std::string snapshotVersion = trim(whatSnapshotVersions[0].str());

//			std::cout << std::endl << std::endl << snapshotVersion << std::endl << std::endl;

			int snapshotVersionClassifierMavenTagIndex = snapshotVersion.find(snapshotVersionClassifierMavenTag);
			int snapshotVersionClassifierEndMavenTagIndex = snapshotVersion.find(snapshotVersionClassifierEndMavenTag);
			int snapshotVersionValueMavenTagIndex = snapshotVersion.find(snapshotVersionValueMavenTag);
			int snapshotVersionValueEndMavenTagIndex = snapshotVersion.find(snapshotVersionValueEndMavenTag);
			int snapshotVersionUpdatedMavenTagIndex = snapshotVersion.find(snapshotVersionUpdatedMavenTag);
			int snapshotVersionUpdatedEndMavenTagIndex = snapshotVersion.find(snapshotVersionUpdatedEndMavenTag);
			int snapshotVersionExtensionMavenTagIndex = snapshotVersion.find(snapshotVersionExtensionMavenTag);
			int snapshotVersionExtensionEndMavenTagIndex = snapshotVersion.find(snapshotVersionExtensionEndMavenTag);

			if ((snapshotVersionClassifierMavenTagIndex == std::string::npos
					|| toLower("sources").compare(
							snapshotVersion.substr(snapshotVersionClassifierMavenTagIndex + snapshotVersionClassifierMavenTag.length(),
									snapshotVersion.find(snapshotVersionClassifierEndMavenTag)
											- (snapshotVersionClassifierMavenTagIndex + snapshotVersionClassifierMavenTag.length()))) == 0)
					&& snapshotVersionValueMavenTagIndex != std::string::npos && snapshotVersionUpdatedMavenTagIndex != std::string::npos
					&& (snapshotVersionExtensionMavenTagIndex == std::string::npos
							|| artefact.getExtension().compare(
									snapshotVersion.substr(snapshotVersionExtensionMavenTagIndex + snapshotVersionExtensionMavenTag.length(),
											snapshotVersion.find(snapshotVersionExtensionEndMavenTag)
													- (snapshotVersionExtensionMavenTagIndex + snapshotVersionExtensionMavenTag.length()))) == 0)) {

				version = new MavenVersionResult();
				version->setValue(
						snapshotVersion.substr(snapshotVersionValueMavenTagIndex + snapshotVersionValueMavenTag.length(),
								snapshotVersionValueEndMavenTagIndex - (snapshotVersionValueMavenTagIndex + snapshotVersionValueMavenTag.length())));
				version->setLastUpdate(
						snapshotVersion.substr(snapshotVersionUpdatedMavenTagIndex + snapshotVersionUpdatedMavenTag.length(),
								snapshotVersionUpdatedEndMavenTagIndex - (snapshotVersionUpdatedMavenTagIndex + snapshotVersionUpdatedMavenTag.length())));
			}

			snapshotVersionStart = whatSnapshotVersions[0].second;
		}
		if (version == NULL) {
			int timestampMavenTagIndex = metaFileContent.find(timestampMavenTag);
			int timestampEndMavenTagIndex = metaFileContent.find(timestampEndMavenTag);
			int buildMavenTagIndex = metaFileContent.find(buildMavenTag);
			int buildEndMavenTagIndex = metaFileContent.find(buildEndMavenTag);
			int lastUpdatedMavenTagIndex = metaFileContent.find(lastUpdatedMavenTag);
			int lastUpdatedEndMavenTagIndex = metaFileContent.find(lastUpdatedEndMavenTag);

			if (timestampMavenTagIndex != std::string::npos && timestampEndMavenTagIndex != std::string::npos && buildMavenTagIndex != std::string::npos
					&& buildEndMavenTagIndex != std::string::npos && lastUpdatedMavenTagIndex != std::string::npos && lastUpdatedEndMavenTagIndex != std::string::npos) {

				version = new MavenVersionResult();
				version->setValue(
						metaFileContent.substr(timestampMavenTagIndex + timestampMavenTag.length(),
								timestampEndMavenTagIndex - (timestampMavenTagIndex + timestampMavenTag.length())) + "-"
								+ metaFileContent.substr(buildMavenTagIndex + buildMavenTag.length(),
										buildEndMavenTagIndex - (buildMavenTagIndex + buildMavenTag.length())));
				version->setLastUpdate(
						metaFileContent.substr(lastUpdatedMavenTagIndex + lastUpdatedMavenTag.length(),
								lastUpdatedEndMavenTagIndex - (lastUpdatedMavenTagIndex + lastUpdatedMavenTag.length())));

			}
		}
	}
//	std::cout << "Returning version from Remote: " << version << " " << version->getValue() << std::endl;
	return version;
}

}
}
