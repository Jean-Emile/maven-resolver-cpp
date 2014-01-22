#include "maven-resolver/impl/MavenVersionResolver.h"

#include <boost/regex.hpp>
#include <fstream>

#include "maven-resolver/api/Utilities.h"
#include "network/http/api/HTTP.h"
#include "network/http/api/Status.h"

maven_resolver::impl::MavenVersionResolver::MavenVersionResolver() {
	mavenDownloader = new maven_resolver::impl::MavenDownloader();
}

maven_resolver::impl::MavenVersionResolver::~MavenVersionResolver() {
	delete mavenDownloader;
}

maven_resolver::api::MavenVersionResult* maven_resolver::impl::MavenVersionResolver::resolveVersion(maven_resolver::api::MavenArtefact artefact, std::string cachePath,
		std::string remoteURL, bool localDeploy) {
	maven_resolver::api::MavenVersionResult *result = NULL;
	std::string fileContent;

	std::string filePath = buildCacheFilePath(artefact, cachePath, remoteURL, localDeploy);

	if (!localDeploy) {
		if (std::fstream(filePath.c_str()).fail()) {
//			std::cout << "Creating directories to store metadata file: " << filePath << std::endl;
			maven_resolver::api::mkdirs(filePath);
		}
		std::list<std::string> repo;
		repo.push_back(remoteURL);
//		std::cout << "Trying to resolve metaData \"" << filePath << "\" on " << remoteURL << std::endl;
		mavenDownloader->downloadMetadata(filePath, artefact, artefact.getVersion(), repo);
	}
//	std::cout << "Metadata file to read: " << filePath << std::endl;

	fileContent = maven_resolver::api::readFile(filePath);

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

maven_resolver::api::MavenVersionResult* maven_resolver::impl::MavenVersionResolver::foundRelevantVersion(maven_resolver::api::MavenArtefact artefact, std::string cachePath,
		std::string remoteURL, bool localDeploy) {
	std::string askedVersion = maven_resolver::api::toLower(artefact.getVersion());

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
		maven_resolver::api::mkdirs(cacheFilePath);
	}
	std::string buffer;
	if (!localDeploy) {
		std::list<std::string> repoUrl;
		repoUrl.push_back(remoteURL);
//		std::cout << "Trying to resolve metaData \"" << cacheFilePath << "\" on " << remoteURL << std::endl;
		mavenDownloader->downloadMetadata(cacheFilePath, artefact, askedVersion, repoUrl);
	}

//	std::cout << "Metadata file to read: " << cacheFilePath << std::endl;

	buffer = maven_resolver::api::readFile(cacheFilePath);

//	std::cout << buffer << std::endl;

	maven_resolver::api::MavenVersionResult* version = NULL;
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

std::string maven_resolver::impl::MavenVersionResolver::buildCacheFilePath(maven_resolver::api::MavenArtefact artefact, std::string basePath, std::string remoteURL,
		bool localDeploy) {
	std::string filePath;
	filePath.append(basePath);
	filePath.append(PATH_SEPARATOR);

	std::string newString = artefact.getGroup();
	newString = maven_resolver::api::replace(newString, ".", maven_resolver::api::fileSeparator);

	filePath.append(newString);
	filePath.append(PATH_SEPARATOR);
	filePath.append(artefact.getName());
	filePath.append(PATH_SEPARATOR);

	std::string askedVersion = maven_resolver::api::toLower(artefact.getVersion());
	if (askedVersion.compare("release") != 0 && askedVersion.compare("latest") != 0) {
		filePath.append(artefact.getVersion());
		filePath.append(PATH_SEPARATOR);
	}

	if (localDeploy) {
		filePath.append(maven_resolver::api::localmetaFile);
	} else {
		filePath.append(maven_resolver::api::metaFile);
		filePath.append("-");

		newString = remoteURL;
		newString = maven_resolver::api::replace(newString, "/", "_");
		newString = maven_resolver::api::replace(newString, ":", "_");
		newString = maven_resolver::api::replace(newString, ".", "_");
		filePath.append(newString);
	}

//	std::cout << "cacheFilePath to use: " << filePath << std::endl;
	return filePath;
}

maven_resolver::api::MavenVersionResult* maven_resolver::impl::MavenVersionResolver::resolveVersionFromLocal(maven_resolver::api::MavenArtefact artefact,
		std::string metaFileContent, bool release, bool latest) {
	maven_resolver::api::MavenVersionResult* version = NULL;

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

		maven_resolver::api::trim(loopVersion);
		if (release) {
			if (maven_resolver::api::toLower(loopVersion).find(maven_resolver::api::SNAPSHOT_VERSION_END) == std::string::npos) {
				if (version == NULL) {
					version = new maven_resolver::api::MavenVersionResult();
					version->setValue("0");
				}
//				std::cout << "Setting value between " << version->getValue() << " and " << loopVersion << std::endl;
				version->setValue(versionComparator.max(version->getValue(), loopVersion));
			}
		} else {
			if (version == NULL) {
				version = new maven_resolver::api::MavenVersionResult();
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

maven_resolver::api::MavenVersionResult* maven_resolver::impl::MavenVersionResolver::resolveVersionFromRemote(maven_resolver::api::MavenArtefact artefact,
		std::string metaFileContent, bool release, bool latest) {
	maven_resolver::api::MavenVersionResult* version = NULL;
	if (release) {
		int buildReleaseTagIndex = metaFileContent.find(maven_resolver::api::buildReleaseTag);
		int buildEndreleaseTagIndex = metaFileContent.find(maven_resolver::api::buildEndreleaseTag);
		if (buildReleaseTagIndex != std::string::npos && buildEndreleaseTagIndex != std::string::npos) {
			version = new maven_resolver::api::MavenVersionResult();
			version->setValue(
					metaFileContent.substr(buildReleaseTagIndex + maven_resolver::api::buildReleaseTag.length(),
							buildEndreleaseTagIndex - (buildReleaseTagIndex + maven_resolver::api::buildReleaseTag.length())));
		}
	} else if (latest) {
		int buildLatestTagIndex = metaFileContent.find(maven_resolver::api::buildLatestTag);
		int buildEndLatestTagIndex = metaFileContent.find(maven_resolver::api::buildEndLatestTag);
		if (buildLatestTagIndex != std::string::npos && buildEndLatestTagIndex != std::string::npos) {
			version = new maven_resolver::api::MavenVersionResult();
			version->setValue(
					metaFileContent.substr(buildLatestTagIndex + maven_resolver::api::buildLatestTag.length(),
							buildEndLatestTagIndex - (buildLatestTagIndex + maven_resolver::api::buildLatestTag.length())));
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
			std::string snapshotVersion = maven_resolver::api::trim(whatSnapshotVersions[0].str());

//			std::cout << std::endl << std::endl << snapshotVersion << std::endl << std::endl;

			int snapshotVersionClassifierMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionClassifierMavenTag);
			int snapshotVersionClassifierEndMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionClassifierEndMavenTag);
			int snapshotVersionValueMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionValueMavenTag);
			int snapshotVersionValueEndMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionValueEndMavenTag);
			int snapshotVersionUpdatedMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionUpdatedMavenTag);
			int snapshotVersionUpdatedEndMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionUpdatedEndMavenTag);
			int snapshotVersionExtensionMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionExtensionMavenTag);
			int snapshotVersionExtensionEndMavenTagIndex = snapshotVersion.find(maven_resolver::api::snapshotVersionExtensionEndMavenTag);

			if ((snapshotVersionClassifierMavenTagIndex == std::string::npos
					|| maven_resolver::api::toLower("sources").compare(
							snapshotVersion.substr(snapshotVersionClassifierMavenTagIndex + maven_resolver::api::snapshotVersionClassifierMavenTag.length(),
									snapshotVersion.find(maven_resolver::api::snapshotVersionClassifierEndMavenTag)
											- (snapshotVersionClassifierMavenTagIndex + maven_resolver::api::snapshotVersionClassifierMavenTag.length()))) == 0)
					&& snapshotVersionValueMavenTagIndex != std::string::npos && snapshotVersionUpdatedMavenTagIndex != std::string::npos
					&& (snapshotVersionExtensionMavenTagIndex == std::string::npos
							|| artefact.getExtension().compare(
									snapshotVersion.substr(snapshotVersionExtensionMavenTagIndex + maven_resolver::api::snapshotVersionExtensionMavenTag.length(),
											snapshotVersion.find(maven_resolver::api::snapshotVersionExtensionEndMavenTag)
													- (snapshotVersionExtensionMavenTagIndex + maven_resolver::api::snapshotVersionExtensionMavenTag.length()))) == 0)) {

				version = new maven_resolver::api::MavenVersionResult();
				version->setValue(
						snapshotVersion.substr(snapshotVersionValueMavenTagIndex + maven_resolver::api::snapshotVersionValueMavenTag.length(),
								snapshotVersionValueEndMavenTagIndex - (snapshotVersionValueMavenTagIndex + maven_resolver::api::snapshotVersionValueMavenTag.length())));
				version->setLastUpdate(
						snapshotVersion.substr(snapshotVersionUpdatedMavenTagIndex + maven_resolver::api::snapshotVersionUpdatedMavenTag.length(),
								snapshotVersionUpdatedEndMavenTagIndex - (snapshotVersionUpdatedMavenTagIndex + maven_resolver::api::snapshotVersionUpdatedMavenTag.length())));
			}

			snapshotVersionStart = whatSnapshotVersions[0].second;
		}
		if (version == NULL) {
			int timestampMavenTagIndex = metaFileContent.find(maven_resolver::api::timestampMavenTag);
			int timestampEndMavenTagIndex = metaFileContent.find(maven_resolver::api::timestampEndMavenTag);
			int buildMavenTagIndex = metaFileContent.find(maven_resolver::api::buildMavenTag);
			int buildEndMavenTagIndex = metaFileContent.find(maven_resolver::api::buildEndMavenTag);
			int lastUpdatedMavenTagIndex = metaFileContent.find(maven_resolver::api::lastUpdatedMavenTag);
			int lastUpdatedEndMavenTagIndex = metaFileContent.find(maven_resolver::api::lastUpdatedEndMavenTag);

			if (timestampMavenTagIndex != std::string::npos && timestampEndMavenTagIndex != std::string::npos && buildMavenTagIndex != std::string::npos
					&& buildEndMavenTagIndex != std::string::npos && lastUpdatedMavenTagIndex != std::string::npos && lastUpdatedEndMavenTagIndex != std::string::npos) {

				version = new maven_resolver::api::MavenVersionResult();
				version->setValue(
						metaFileContent.substr(timestampMavenTagIndex + maven_resolver::api::timestampMavenTag.length(),
								timestampEndMavenTagIndex - (timestampMavenTagIndex + maven_resolver::api::timestampMavenTag.length())) + "-"
								+ metaFileContent.substr(buildMavenTagIndex + maven_resolver::api::buildMavenTag.length(),
										buildEndMavenTagIndex - (buildMavenTagIndex + maven_resolver::api::buildMavenTag.length())));
				version->setLastUpdate(
						metaFileContent.substr(lastUpdatedMavenTagIndex + maven_resolver::api::lastUpdatedMavenTag.length(),
								lastUpdatedEndMavenTagIndex - (lastUpdatedMavenTagIndex + maven_resolver::api::lastUpdatedMavenTag.length())));

			}
		}
	}
//	std::cout << "Returning version from Remote: " << version << " " << version->getValue() << std::endl;
	return version;
}
