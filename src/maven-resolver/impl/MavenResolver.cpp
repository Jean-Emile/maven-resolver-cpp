#include "maven-resolver/api/MavenResolver.h"

#include <stdio.h>
#include <list>
#include <iostream>
#include <stdlib.h>

#include "maven-resolver/api/Utilities.h"
namespace maven {
namespace resolver {

MavenResolver::MavenResolver() {
	versionComparator = new MavenVersionComparator();
	mavenDownloader = new MavenDownloader();
	versionResolver = new MavenVersionResolver();
	this->basePath = "";
}

MavenResolver::~MavenResolver() {
	delete versionResolver;
	delete versionComparator;
	delete mavenDownloader;

}
void MavenResolver::setBasePath(std::string path){
	this->basePath = path;
}

std::string MavenResolver::resolve(std::string group, std::string name, std::string versionAsked, std::string extension, std::list<std::string> urls) {

	MavenArtefact artefact;
	artefact.setGroup(group);
	artefact.setName(name);
	artefact.setVersion(versionAsked);
	artefact.setExtension(extension);

	return resolve(artefact, urls);
}

std::string MavenResolver::resolve(MavenArtefact artefact, std::list<std::string> urls) {

	std::string versionToLower = toLower(artefact.getVersion()); //transform(artefact.getVersion().begin(), artefact.getVersion().end(), artefact.getVersion().begin(), tolower);
	if (versionToLower.compare("release") == 0 || versionToLower.compare("latest") == 0) {
		//		MavenVersionResult* vremoteSaved = versionResolver->foundRelevantVersion(artefact, basePath, basePath, false);
		MavenVersionResult* vlocalSaved = versionResolver->foundRelevantVersion(artefact, basePath, "", true);
		//		if (vremoteSaved != NULL) {
		//			artefact.setVersion(versionComparator->max(artefact.getVersion(), vremoteSaved->getValue()));
		//		}
		if (vlocalSaved != NULL) {
			//			std::cout << "Compare current version to the one get from the local repository" << std::endl;
			artefact.setVersion(versionComparator->max(artefact.getVersion(), vlocalSaved->getValue()));
		}
		//		delete vremoteSaved;
		delete vlocalSaved;

		//FIXME managing multi threading for resolving version on each repositories
		for (std::list<std::string>::iterator it = urls.begin(); it != urls.end(); it++) {
			MavenVersionResult* tmpVersion = versionResolver->foundRelevantVersion(artefact, basePath, *it, false);
			if (tmpVersion != NULL) {
				//				std::cout << "Compare current version to the one get from the repo: " << *it << std::endl;
				artefact.setVersion(versionComparator->max(artefact.getVersion(), tmpVersion->getValue()));
			}
			delete tmpVersion;
		}
	}

	//	std::cout << artefact.getVersion() << std::endl;

	if (artefact.getVersion().find(SNAPSHOT_VERSION_END) == std::string::npos) {
		// try to find release artifact

		//Try from local cache first
		std::string filePath = getArtefactLocalBasePath(artefact);
		filePath.append(artefact.getName());
		filePath.append("-");
		filePath.append(artefact.getVersion());
		filePath.append(".");
		filePath.append(artefact.getExtension());
		std::fstream targetReleaseFile(filePath.c_str());
		bool isGood = targetReleaseFile.good();
		targetReleaseFile.close();
		if (isGood) {
			return filePath;
		} else {
			mkdirs(filePath);
		}
		//download and stop when we find one
		if (mavenDownloader->downloadArtefact(filePath, artefact, "", urls)) {
			return filePath;
		}
		//No url reply, return not found result
		return "";
	} else {
		//try to find snapshot artifact

		std::list<MavenVersionResult*> versions;
		MavenVersionResult* localVersion;
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
					//	std::cout << "Trying to get metadata from " << *it << std::endl;
			MavenVersionResult* version = versionResolver->resolveVersion(artefact, basePath, *it, false);
			if (version != NULL) {
				versions.push_back(version);
			}
		}

		//		std::cout << "Potential versions: ";
		//		for (std::list<MavenVersionResult*>::iterator it = versions.begin(); it != versions.end(); it++) {
		//			std::cout << (*it)->getValue() << ", ";
		//		}
		//		std::cout << std::endl;

		if (versions.size() == 0) {
			//not version at all , try simply the file with -SNAPSHOT extension
			std::string filePath = getArtefactLocalBasePath(artefact);
			filePath.append(artefact.getName());
			filePath.append("-");
			filePath.append(artefact.getVersion());
			filePath.append(".");
			filePath.append(artefact.getExtension());
			std::fstream snapshotFile(filePath.c_str());
			bool isGood = snapshotFile.good();
			snapshotFile.close();
			if (isGood) {
				return filePath;
			} else {
				std::cerr << "No metadata file found for " << artefact.getGroup() << "/" << artefact.getName() << "/" << artefact.getVersion() << std::endl;
				return "";
			}
		} else {
			MavenVersionResult * bestVersion = NULL;
			for (std::list<MavenVersionResult*>::iterator it = versions.begin(); it != versions.end(); it++) {
				if (bestVersion == NULL || bestVersion->isPrior(**it)) {
					delete bestVersion;
					bestVersion = *it;
				}
			}

			if (bestVersion != NULL) {
				std::string preresolvedVersion = bestVersion->getValue();
				std::string firstPartVersion = artefact.getVersion();
				firstPartVersion = replace(firstPartVersion, SNAPSHOT_VERSION_END, "");
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
					delete bestVersion;
					filePath.append(".");
					filePath.append(artefact.getExtension());
					std::fstream snapshotFile(filePath.c_str(), std::ios_base::in | std::ios_base::out);
					bool isGood = snapshotFile.good();
					snapshotFile.close();
					if (isGood) {
						return filePath;
					} else {
						//This is really bad... try to get remotely
						//FIXME remove meta local file ?
						MavenVersionResult * bestRemoteVersion = NULL;
						for (std::list<MavenVersionResult*>::iterator it = versions.begin(); it != versions.end(); it++) {
							if ((*it)->getUrl_origin().compare(basePath) != 0 && (bestRemoteVersion == NULL || bestRemoteVersion->isPrior(**it))) {
								delete bestRemoteVersion;
								bestRemoteVersion = *it;
							}
						}
						std::string preresolvedVersion2 = bestRemoteVersion->getValue();
						std::string firstPartVersion2(artefact.getVersion());
						firstPartVersion2 = replace(firstPartVersion2, SNAPSHOT_VERSION_END, "");
						if (preresolvedVersion2.find(firstPartVersion2) != 0) {
							preresolvedVersion2 = firstPartVersion2 + bestRemoteVersion->getValue();
						}
						delete bestRemoteVersion;
						//Ok try on all urls, meta file has been download but not the artefact :(
						if (mavenDownloader->downloadArtefact(filePath, artefact, preresolvedVersion2, urls)) {
							//download the metafile
							//							std::cout << "File resolved remotly, download metafile" << std::endl;
							std::string metaFilePath = filePath.substr(0, filePath.find_last_of(fileSeparator)) + fileSeparator
									+ metaFile;
							mkdirs(metaFilePath);
							//							std::fstream newMetaFile(metaFilePath.c_str(), std::ios_base::in | std::ios_base::out);
							mavenDownloader->downloadMetadata(metaFilePath, artefact, preresolvedVersion2, urls);
							return filePath;
						}
						//						std::cerr << ">" + bestVersion->toString() << std::endl;
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
					bool isGood = targetSnapshotFile.good();
					targetSnapshotFile.close();
					if (isGood) {
						delete bestVersion;
						return filePath;
					} else {
						std::list<std::string> goodUrl;
						goodUrl.push_back(bestVersion->getUrl_origin());
						delete bestVersion;
						//						std::cout << "Trying to download artefact on " << bestVersion->getUrl_origin() << std::endl;
						if (mavenDownloader->downloadArtefact(filePath, artefact, preresolvedVersion, goodUrl)) {
							//							std::cout << "File resolved remotly, download metafile" << std::endl;
							//download the metafile
							std::string metaFilePath = filePath.substr(0, filePath.find_last_of(fileSeparator)) + fileSeparator
									+ metaFile;
							mkdirs(metaFilePath);
							mavenDownloader->downloadMetadata(metaFilePath, artefact, preresolvedVersion, goodUrl);
							return filePath;
						}
						//not found
						//						std::cout << "Not resolved " << preresolvedVersion << " from " << bestVersion->getUrl_origin() << "/" << artefact.getGroup() << "/" << artefact.getName()
						//								<< "/" << artefact.getVersion() << std::endl;
						return "";
					}
				}
			} else {
				//				std::cerr << "Not best version are found for " << artefact.getGroup() << "/" << artefact.getName() << "/" << artefact.getVersion() << std::endl;
				delete bestVersion;
				return "";
			}
		}
	}
}

std::string MavenResolver::getArtefactLocalBasePath(MavenArtefact artefact) {
	std::string localBasePath;
	localBasePath.append(basePath);
	localBasePath.append(fileSeparator);
	std::string newString(artefact.getGroup());
	newString = replace(newString, ".", fileSeparator);
	localBasePath.append(newString);
	localBasePath.append(fileSeparator);
	localBasePath.append(artefact.getName());
	localBasePath.append(fileSeparator);
	localBasePath.append(artefact.getVersion());
	localBasePath.append(fileSeparator);
	return localBasePath;
}

}
}
