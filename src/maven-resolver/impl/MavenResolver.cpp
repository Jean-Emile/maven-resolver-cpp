#include <stdio.h>
#include <list>
#include <iostream>

#include "maven-resolver/api/FileSystemManagement.h"
#include "maven-resolver/impl/MavenResolver.h"
#include "maven-resolver/api/MavenArtefact.h"

MavenResolver::MavenResolver() {
	versionComparator = new maven_resolver::api::MavenVersionComparator();
	mavenDownloader = new maven_resolver::impl::MavenDownloader();
	versionResolver = new MavenVersionResolver();
	basePath = "/home/edaubert/.m2/repository"; // TO FIX
}

MavenResolver::~MavenResolver() {
	delete versionResolver;
	delete versionComparator;

}

std::string MavenResolver::resolve(std::string group, std::string name,
		std::string versionAsked, std::string extension,
		std::list<std::string> urls) {

	MavenArtefact artefact;
	artefact.setGroup(group);
	artefact.setName(name);
	artefact.setVersion(versionAsked);
	artefact.setExtension(extension);

	std::string versionToLower = transform(artefact.getVersion().begin(),
			artefact.getVersion().end(), artefact.getVersion().begin(),
			tolower);
	if (versionToLower.compare("release") == 0
			|| versionToLower.compare("latest") == 0) {
		std::string vremoteSaved = versionResolver->foundRelevantVersion(
				artefact, basePath, basePath, false);
		std::string vlocalSaved = versionResolver->foundRelevantVersion(
				artefact, basePath, basePath, true);
		artefact.setVersion(
				versionComparator->max(artefact.getVersion(), vremoteSaved));
		artefact.setVersion(
				versionComparator->max(artefact.getVersion(), vlocalSaved));

		//TODO managing multi threading for resolving version on each repositories
		for (std::list<std::string>::iterator it = urls.begin();
				it != urls.end(); it++) {
			std::string tmpVersion = versionResolver->foundRelevantVersion(
					artefact, basePath, *it, false);
			artefact.setVersion(
					versionComparator->max(artefact.getVersion(), tmpVersion));
		}
	}

	if (artefact.getVersion().find(SNAPSHOT_VERSION_END) == std::string::npos) {
		// try to find release artifact

		//Try from local cache first
		std::string filePath = getArtefactLocalBasePath(artefact);
		filePath.append(name);
		filePath.append("-");
		filePath.append(artefact.getVersion());
		filePath.append(".");
		filePath.append(extension);
		std::fstream targetReleaseFile(filePath);
		if (targetReleaseFile.good()) {
			return filePath;
		}
		//download and stop when we find one
		if (mavenDownloader->downloadArtefact(filePath, artefact, extension,
		NULL, false, urls)) {
			return filePath;
		}
		//No url reply, return not found result
		return NULL;
	} else {
		//try to find snapshot artifact

		std::list<MavenVersionResult> versions;
		MavenVersionResult localVersion;
		try {
			localVersion = versionResolver->resolveVersion(artefact, basePath,
					false);
			if (localVersion != NULL) {
				versions.push_back(localVersion);
			}
		} catch (std::exception & e) {
			//not found remotely, ignore it
		}
		try {
			localVersion = versionResolver->resolveVersion(artefact, basePath,
					true);
			if (localVersion != NULL) {
				versions.push_back(localVersion);
			}
		} catch (std::exception & e) {
			//not found locally, ignore it
		}

		ExecutorService pool = Executors.newCachedThreadPool();
		ArrayList < AsyncVersionResolver > resolvers = new ArrayList<
				AsyncVersionResolver>();
		for (String url : urls) {
			resolvers.add(
					new AsyncVersionResolver(artefact, url, versionResolver));
		}
		try {
			List < Future < MavenVersionResult >> results = pool.invokeAll(
					resolvers);
			for (Future<MavenVersionResult> r : results) {
				if (r != null) {
					try {
						MavenVersionResult interRes = r.get();
						if (interRes != null) {
							versions.add(interRes);
						}
					} catch (ExecutionException e) {
						e.printStackTrace();
					}
				}
			}
		} catch (std::exception & e) {
			std::cout << e.what();
		}
//		pool.shutdown();

		if (versions.size() == 0) {
			//not version at all , try simply the file with -SNAPSHOT extension
			std::string filePath = getArtefactLocalBasePath(artefact);
			filePath.append(name);
			filePath.append("-");
			filePath.append(artefact.getVersion());
			filePath.append(".");
			filePath.append(extension);
			std::fstream snapshotFile(filePath);
			if (snapshotFile.good()) {
				return snapshotFile;
			} else {
				std::cerr << "No metadata file found for " << group << "/"
						<< name << "/" << artefact.getVersion();
				return NULL;
			}
		} else {
			MavenVersionResult bestVersion = NULL;
			for (MavenVersionResult loopVersion : versions) {
				if (bestVersion == NULL || bestVersion.isPrior(loopVersion)) {
					bestVersion = loopVersion;
				}
			}
			if (bestVersion != NULL) {
				std::string preresolvedVersion = bestVersion.getValue();
				std::string firstPartVersion = new std::string(
						artefact.getVersion());
				maven_resolver::api::replace(firstPartVersion, "SNAPSHOT", "");
//				std::string firstPartVersion = artefact.getVersion().replace(
//						"SNAPSHOT", "");
				if (!preresolvedVersion.find(firstPartVersion) == 0) {
					preresolvedVersion = firstPartVersion
							+ bestVersion.getValue();
				}
				if (bestVersion.getUrl_origin().equals(basePath)) {
					//resolve locally
					std::string filePath = getArtefactLocalBasePath(artefact);
					filePath.append(name);
					filePath.append("-");
					if (!bestVersion.isNotDeployed()) {
						filePath.append(preresolvedVersion);
					} else { //TAKE directly -snapshot file
						filePath.append(artefact.getVersion());
					}
					filePath.append(".");
					filePath.append(extension);
					std::fstream snapshotFile(filePath);
					if (snapshotFile.good()) {
						return snapshotFile;
					} else {
						//This is really bad... try to get remotely
						//remove meta local file ?
						//TODO
						MavenVersionResult bestRemoteVersion = NULL;
						for (MavenVersionResult loopVersion : versions) {
							if ((!loopVersion.getUrl_origin().equals(basePath))
									&& (bestRemoteVersion == NULL
											|| bestRemoteVersion.isPrior(
													loopVersion))) {
								bestRemoteVersion = loopVersion;
							}
						}
						std::string preresolvedVersion2 =
								bestRemoteVersion.getValue();
						std::string firstPartVersion2 = new std::string(
								artefact.getVersion());
						maven_resolver::api::replace(firstPartVersion2,
								"SNAPSHOT", "");
//						std::string firstPartVersion2 =
//								artefact.getVersion().replace("SNAPSHOT", "");
						if (preresolvedVersion2.find(firstPartVersion2) != 0) {
							preresolvedVersion2 = firstPartVersion2
									+ bestRemoteVersion.getValue();
						}
						/*artefact.getVersion().replace("SNAPSHOT", "");
						 preresolvedVersion2 = preresolvedVersion2 + bestRemoteVersion.getTimestamp();
						 if (bestRemoteVersion.getBuildNumber() != null) {
						 preresolvedVersion2 = preresolvedVersion2 + "-";
						 preresolvedVersion2 = preresolvedVersion2 + bestRemoteVersion.getBuildNumber();
						 }*/
						//Ok try on all urls, meta file has been download but not the artefact :(
//						for (String url : urls) {
							if (mavenDownloader->downloadArtefact(snapshotFile, artefact,
									extension, preresolvedVersion2, urls)) {
								//download the metafile
								std::cout << "File resolved remotly, download metafile" << std::endl;
								std::fstream newMetaFile (snapshotFile.getAbsolutePath().substring(
														0,
														snapshotFile.getAbsolutePath().lastIndexOf(
																"/")) + "/"
														+ MavenVersionResolver.metaFile);
								mavenDownloader->downloadMetadata(newMetaFile, artefact,
										extension, preresolvedVersion2, urls);
								return snapshotFile;
							}
//						}
						std::cerr << ">" + bestVersion;
						return NULL;
					}
				} else {
					//try to see if its localy cached
					std::string filePath = getArtefactLocalBasePath(artefact);
					filePath.append(name);
					filePath.append("-");
					filePath.append(preresolvedVersion);
					filePath.append(".");
					filePath.append(extension);
					std::fstream targetSnapshotFile(filePath);
					if (targetSnapshotFile.good()) {
						return targetSnapshotFile;
					} else {

						std::list<std::string> goodUrl;
						goodUrl.push_back(bestVersion.getUrl_origin());
						if (mavenDownloader->downloadArtefact(targetSnapshotFile, artefact, extension, preresolvedVersion, goodUrl)) {
//						if (downloader.download(targetSnapshotFile,
//								bestVersion.getUrl_origin(), artefact,
//								extension, preresolvedVersion, false)) {
							std::cout << "File resolved remotly, download metafile" << std::endl;
							//download the metafile
							std::fstream newMetaFile =
									new File(
											targetSnapshotFile.getAbsolutePath().substring(
													0,
													targetSnapshotFile.getAbsolutePath().lastIndexOf(
															"/")) + "/"
													+ MavenVersionResolver.metaFile);
//							downloader.download(newMetaFile,
//									bestVersion.getUrl_origin(), artefact,
//									extension, preresolvedVersion, true);
							mavenDownloader->downloadMetadata(newMetaFile, artefact,
																	extension, preresolvedVersion, goodUrl);
							return targetSnapshotFile;
						}
						//not found
						std::cout << "Not resolved " << preresolvedVersion << " from " << bestVersion.getUrl_origin() << "/" << group << "/" <<  name << "/" <<  artefact.getVersion();
						return NULL;
					}
				}

			} else {
				std::cerr << "Not best version are found for ", group << "/"
						<< name << "/" << artefact.getVersion();
				return NULL;
			}
		}
	}
}

std::string MavenResolver::getArtefactLocalBasePath(MavenArtefact artefact) {
	std::string builder = new std::string();
	builder.append(basePath);
	builder.append(maven_resolver::api::fileSeparator);
	std::string newString (artefact.getGroup());
	maven_resolver::api::replace(newString, ".",
			maven_resolver::api::fileSeparator);
	builder.append(newString);
	builder.append(maven_resolver::api::fileSeparator);
	builder.append(artefact.getName());
	builder.append(maven_resolver::api::fileSeparator);
	builder.append(artefact.getVersion());
	builder.append(maven_resolver::api::fileSeparator);
	return builder;
}
