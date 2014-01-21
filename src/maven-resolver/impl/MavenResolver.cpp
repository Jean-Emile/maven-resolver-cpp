#include "maven-resolver/impl/MavenResolver.h"

#include <stdio.h>
#include <list>
#include <iostream>

#include "maven-resolver/api/Utilities.h"

MavenResolver::MavenResolver() {
	versionComparator = new maven_resolver::api::MavenVersionComparator();
	mavenDownloader = new maven_resolver::impl::MavenDownloader();
	versionResolver = new maven_resolver::impl::MavenVersionResolver();
	basePath = "/home/edaubert/.m2/repository"; // FIXME
}

MavenResolver::~MavenResolver() {
	delete versionResolver;
	delete versionComparator;

}

std::string MavenResolver::resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls) {

	maven_resolver::api::MavenArtefact artefact;
	artefact.setGroup(group);
	artefact.setName(name);
	artefact.setVersion(versionAsked);
	artefact.setExtension(extension);

	return resolve(artefact, urls);
}

std::string MavenResolver::resolve(maven_resolver::api::MavenArtefact artefact, std::list<std::string> urls) {

	std::string versionToLower = maven_resolver::api::toLower(artefact.getVersion()); //transform(artefact.getVersion().begin(), artefact.getVersion().end(), artefact.getVersion().begin(), tolower);
	if (versionToLower.compare("release") == 0 || versionToLower.compare("latest") == 0) {
		maven_resolver::api::MavenVersionResult* vremoteSaved = versionResolver->foundRelevantVersion(artefact, basePath, basePath, false);
		maven_resolver::api::MavenVersionResult* vlocalSaved = versionResolver->foundRelevantVersion(artefact, basePath, basePath, true);
		if (vremoteSaved != NULL) {
			artefact.setVersion(versionComparator->max(artefact.getVersion(), vremoteSaved->getValue()));
		}
		if (vlocalSaved != NULL) {
			artefact.setVersion(versionComparator->max(artefact.getVersion(), vlocalSaved->getValue()));
		}
		delete vremoteSaved;
		delete vlocalSaved;

		//FIXME managing multi threading for resolving version on each repositories
		for (std::list<std::string>::iterator it = urls.begin(); it != urls.end(); it++) {
			maven_resolver::api::MavenVersionResult* tmpVersion = versionResolver->foundRelevantVersion(artefact, basePath, *it, false);
			if (tmpVersion != NULL) {
				artefact.setVersion(versionComparator->max(artefact.getVersion(), tmpVersion->getValue()));
			}
			delete tmpVersion;
		}
	}

	if (artefact.getVersion().find(maven_resolver::api::SNAPSHOT_VERSION_END) == std::string::npos) {
		// try to find release artifact

		//Try from local cache first
		std::string filePath = getArtefactLocalBasePath(artefact);
		filePath.append(artefact.getName());
		filePath.append("-");
		filePath.append(artefact.getVersion());
		filePath.append(".");
		filePath.append(artefact.getExtension());
		std::fstream targetReleaseFile(filePath.c_str());
		if (targetReleaseFile.good()) {
			return filePath;
		} else {
			maven_resolver::api::mkdirs(filePath);
		}
		//download and stop when we find one
		if (mavenDownloader->downloadArtefact(filePath, artefact, "", urls)) {
			return filePath;
		}
		//No url reply, return not found result
		return "";
	} else {
		//try to find snapshot artifact

		std::list<maven_resolver::api::MavenVersionResult*> versions;
		maven_resolver::api::MavenVersionResult* localVersion;
//			localVersion = versionResolver->resolveVersion(artefact, basePath, "", false);
//			if (localVersion != NULL) {
//				versions.push_back(localVersion);
//			}
		localVersion = versionResolver->resolveVersion(artefact, basePath, "", true);
		if (localVersion != NULL) {
			versions.push_back(localVersion);
		}

		// FIXME managing multi threading for resolving version on each repositories
		for (std::list<std::string>::iterator it = urls.begin(); it != urls.end(); it++) {
//			std::cout << "Trying to get metadata from " << *it << std::endl;
			maven_resolver::api::MavenVersionResult* version = versionResolver->resolveVersion(artefact, basePath, *it, false);
			if (version != NULL) {
				versions.push_back(version);
			}
		}

		if (versions.size() == 0) {
			//not version at all , try simply the file with -SNAPSHOT extension
			std::string filePath = getArtefactLocalBasePath(artefact);
			filePath.append(artefact.getName());
			filePath.append("-");
			filePath.append(artefact.getVersion());
			filePath.append(".");
			filePath.append(artefact.getExtension());
			std::fstream snapshotFile(filePath.c_str());
			if (snapshotFile.good()) {
				return filePath;
			} else {
				std::cerr << "No metadata file found for " << artefact.getGroup() << "/" << artefact.getName() << "/" << artefact.getVersion();
				return "";
			}
		} else {
			maven_resolver::api::MavenVersionResult * bestVersion = NULL;
			for (std::list<maven_resolver::api::MavenVersionResult*>::iterator it = versions.begin(); it != versions.end(); it++) {
				if (bestVersion == NULL || bestVersion->isPrior(**it)) {
					bestVersion = *it;
				}
			}

			if (bestVersion != NULL) {
				std::string preresolvedVersion = bestVersion->getValue();
				std::string firstPartVersion = artefact.getVersion();
				firstPartVersion = maven_resolver::api::replace(firstPartVersion, maven_resolver::api::SNAPSHOT_VERSION_END, "");
				if (!preresolvedVersion.find(firstPartVersion) == 0) {
					preresolvedVersion = firstPartVersion + bestVersion->getValue();
				}
				if (bestVersion->getUrl_origin().compare(basePath) == 0) {
					//resolve locally
					std::string filePath = getArtefactLocalBasePath(artefact);
					filePath.append(artefact.getName());
					filePath.append("-");
					if (!bestVersion->isNotDeployed()) {
						filePath.append(preresolvedVersion);
					} else { //TAKE directly -snapshot file
						filePath.append(artefact.getVersion());
					}
					filePath.append(".");
					filePath.append(artefact.getExtension());
					std::fstream snapshotFile(filePath.c_str(), std::ios_base::in | std::ios_base::out);
					if (snapshotFile.good()) {
						return filePath;
					} else {
						//This is really bad... try to get remotely
						//FIXME remove meta local file ?
						maven_resolver::api::MavenVersionResult * bestRemoteVersion = NULL;
						for (std::list<maven_resolver::api::MavenVersionResult*>::iterator it = versions.begin(); it != versions.end(); it++) {
							if ((*it)->getUrl_origin().compare(basePath) != 0 && (bestRemoteVersion == NULL || bestRemoteVersion->isPrior(**it))) {
								bestRemoteVersion = *it;
							}
						}
						std::string preresolvedVersion2 = bestRemoteVersion->getValue();
						std::string firstPartVersion2(artefact.getVersion());
						firstPartVersion2 = maven_resolver::api::replace(firstPartVersion2, maven_resolver::api::SNAPSHOT_VERSION_END, "");
						if (preresolvedVersion2.find(firstPartVersion2) != 0) {
							preresolvedVersion2 = firstPartVersion2 + bestRemoteVersion->getValue();
						}
						//Ok try on all urls, meta file has been download but not the artefact :(
						if (mavenDownloader->downloadArtefact(filePath, artefact, preresolvedVersion2, urls)) {
							//download the metafile
							std::cout << "File resolved remotly, download metafile" << std::endl;
							std::string metaFilePath = filePath.substr(0, filePath.find_last_of(maven_resolver::api::fileSeparator)) + maven_resolver::api::fileSeparator
									+ maven_resolver::api::metaFile;
							maven_resolver::api::mkdirs(metaFilePath);
//							std::fstream newMetaFile(metaFilePath.c_str(), std::ios_base::in | std::ios_base::out);
							mavenDownloader->downloadMetadata(metaFilePath, artefact, preresolvedVersion2, urls);
							return filePath;
						}
						std::cerr << ">" + bestVersion->toString() << std::endl;
						return "";
					}
				} else {
					//try to see if its localy cached
					std::string filePath = getArtefactLocalBasePath(artefact);
					filePath.append(artefact.getName());
					filePath.append("-");
					filePath.append(preresolvedVersion);
					filePath.append(".");
					filePath.append(artefact.getExtension());
					std::fstream targetSnapshotFile(filePath.c_str(), std::ios_base::in | std::ios_base::out);
					if (targetSnapshotFile.good()) {
						return filePath;
					} else {
						std::list<std::string> goodUrl;
						goodUrl.push_back(bestVersion->getUrl_origin());
						std::cout << "Trying to download artefact on " << bestVersion->getUrl_origin() << std::endl;
						if (mavenDownloader->downloadArtefact(filePath, artefact, preresolvedVersion, goodUrl)) {
							std::cout << "File resolved remotly, download metafile" << std::endl;
							//download the metafile
							std::string metaFilePath = filePath.substr(0, filePath.find_last_of(maven_resolver::api::fileSeparator)) + maven_resolver::api::fileSeparator
									+ maven_resolver::api::metaFile;
							maven_resolver::api::mkdirs(metaFilePath);
//							std::fstream newMetaFile(metaFilePath.c_str());
							mavenDownloader->downloadMetadata(metaFilePath, artefact, preresolvedVersion, goodUrl);
							return filePath;
						}
						//not found
						std::cout << "Not resolved " << preresolvedVersion << " from " << bestVersion->getUrl_origin() << "/" << artefact.getGroup() << "/" << artefact.getName()
								<< "/" << artefact.getVersion() << std::endl;
						return "";
					}
				}
			} else {
				std::cerr << "Not best version are found for " << artefact.getGroup() << "/" << artefact.getName() << "/" << artefact.getVersion() << std::endl;
				return "";
			}
		}
	}
}

std::string MavenResolver::getArtefactLocalBasePath(maven_resolver::api::MavenArtefact artefact) {
	std::string localBasePath;
	localBasePath.append(basePath);
	localBasePath.append(maven_resolver::api::fileSeparator);
	std::string newString(artefact.getGroup());
	newString = maven_resolver::api::replace(newString, ".", maven_resolver::api::fileSeparator);
	localBasePath.append(newString);
	localBasePath.append(maven_resolver::api::fileSeparator);
	localBasePath.append(artefact.getName());
	localBasePath.append(maven_resolver::api::fileSeparator);
	localBasePath.append(artefact.getVersion());
	localBasePath.append(maven_resolver::api::fileSeparator);
	return localBasePath;
}
