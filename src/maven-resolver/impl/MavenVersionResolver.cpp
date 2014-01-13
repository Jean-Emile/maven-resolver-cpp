#include "maven-resolver/impl/MavenVersionResolver.h"

#include <boost/filesystem.hpp>
#include <fstream>

#include "maven-resolver/api/FileSystemManagement.h"
#include "network/http/api/HTTP.h"
#include "network/http/api/Status.h"

MavenVersionResolver::MavenVersionResolver() {
	mavenDownloader = new maven_resolver::impl::MavenDownloader();
}

MavenVersionResolver::~MavenVersionResolver() {
}

MavenVersionResult* MavenVersionResolver::resolveVersion(MavenArtefact artefact,
		std::string basePath, bool localDeploy) {
	MavenVersionResult *result = new MavenVersionResult();
	std::string builder;
	std::string sep = PATH_SEPARATOR;
	builder.append(basePath);

	if (basePath.find(network::http::api::http) == 0) {
		sep = "/";
	}
	if (basePath.find(sep) != 0) {
		builder.append(sep);
	}
	if (basePath.find(network::http::api::http) == 0) {
		builder.append(
				artefact.getGroup().replace(builder.begin(), builder.end(), ".",
						"/"));
	} else {
		builder.append(
				artefact.getGroup().replace(builder.begin(), builder.end(), ".",
				PATH_SEPARATOR));
	}
	builder.append(sep);
	builder.append(artefact.getName());
	builder.append(sep);
	builder.append(artefact.getVersion());
	builder.append(sep);
	if (localDeploy) {
		builder.append(localmetaFile);
	} else {
		builder.append(metaFile);
	}

	// TODO 

	return result;
}

std::string MavenVersionResolver::foundRelevantVersion(MavenArtefact artefact,
		std::string basePath, std::string remoteURL, bool localDeploy) {
	std::string askedVersion = artefact.getVersion(); // todo tolower ?
	bool release = false;
	bool lastest = false;
	std::string sep = PATH_SEPARATOR;
	if (askedVersion.compare("release") == 0) {
		release = true;
	}
	if (askedVersion.compare("latest") == 0) {
		lastest = true;
	}
	if (!release && !lastest) {
		return NULL;
	}
	std::string builder;
	if (basePath.find(network::http::api::http) == 0) {
		sep = "/";
	}
	if (basePath.find(sep) != 0) {
		builder.append(sep);
	}
	if (basePath.find(network::http::api::http) == 0) {
		builder.append(
				artefact.getGroup().replace(builder.begin(), builder.end(), ".",
						"/"));
	} else {
		builder.append(
				artefact.getGroup().replace(builder.begin(), builder.end(), ".",
				PATH_SEPARATOR));
	}
	builder.append(sep);
	builder.append(artefact.getName());
	builder.append(sep);
	builder.append(artefact.getVersion());
	builder.append(sep);
	if (localDeploy) {
		builder.append(localmetaFile);
	} else {
		builder.append(metaFile);
	}

	std::string cacheFilePath;
	std::fstream cacheFile;
	if (remoteURL.find(network::http::api::http + "://") == 0
			|| remoteURL.find(network::http::api::https + "://") == 0) {
		cacheFilePath = buildCacheFilePath(artefact, cachePath, remoteURL);
		boost::filesystem::create_directories(
				boost::filesystem::path(
						buildCacheFilePath(artefact, cachePath, remoteURL)).parent_path());
	}
	std::string buffer;
	std::string metadataURL = builder;
	if (remoteURL.find(network::http::api::http + "://") == std::string::npos
			&& remoteURL.find(network::http::api::https + "://") == std::string::npos) {
		metadataURL = "file:///" + builder;
		try {
			cacheFile.open(metadataURL.c_str(), std::ofstream::in);
			if (cacheFile.good()) {
				// TODO read the file
			}
		} catch (std::exception& e) {
			std::cerr << "Can't create cache file: " << cacheFilePath
					<< std::endl;
		}
	} else if (remoteURL.find(network::http::api::http + "://") == 0
			|| remoteURL.find(network::http::api::https + "://") == 0) {


		// create request and then call doGet
		network::http::api::HTTPRequest request;
		request.addHeader("User-Agent", "Kevoree");
		request.setUrl(metadataURL);
		network::http::api::HTTPResponse * response = client->doGet(request);

		//FIXME remove boost::filesystem !!!
		// use result
		if (response->getStatus() == network::http::api::ok) {
			buffer = response->getContentAsString();
			// store the content on local filesystem

			try {
				cacheFile.open(cacheFilePath.c_str(),
						std::ofstream::out | std::ofstream::app);
				if (cacheFile.good()) {
					cacheFile << buffer;
				}
				cacheFile.flush();
				cacheFile.close();
				std::cout << cacheFilePath << " is created" << std::endl;
			} catch (std::exception& e) {
				std::cerr << "Can't create cache file: " << cacheFilePath
						<< std::endl;
			}
		}
	} else {
		std::cerr
				<< "Unable to retrieve the needed information, so the version cannot be resolved."
				<< std::endl;
		return NULL;
	}

	if (builder.length() != 0) {
		try {
			if (release) {
				if (builder.find(buildReleaseTag) != std::string::npos
						&& builder.find(buildEndreleaseTag)
								!= std::string::npos) {
					return builder.substr(
							builder.find(buildReleaseTag)
									+ buildReleaseTag.length(),
							builder.find(buildEndreleaseTag));
				}
			}
			if (lastest) {
				if (builder.find(buildLatestTag) != std::string::npos
						&& builder.find(buildEndLatestTag)
								!= std::string::npos) {
					return builder.substr(
							builder.find(buildLatestTag)
									+ buildLatestTag.length(),
							builder.find(buildEndLatestTag));
				}
			}
			// FIXME SNAPSHOT version is not yet managed
			/*if (localDeploy) {
				Pattern pattern = Pattern.compile(
						"<versions>(\\s|.)*</versions>");
				Matcher matcher = pattern.matcher(flatFile);
				String bestVersion = "-1";
				while (matcher.find()) {
					Pattern patternVersion = Pattern.compile(
							"(<version>)((\\d|\\w|[-]|\\S)*)</version>");
					Matcher matcher2 = patternVersion.matcher(matcher.group());
					while (matcher2.find()) {
						for (int i = 2; i < matcher2.groupCount(); i++) {
							String loopVersion = matcher2.group(i).trim();
							if (release) {
								if (!loopVersion.toLowerCase().contains(
										"snapshot")) {
									bestVersion = MavenVersionComparator.max(
											bestVersion, loopVersion);
								}
							} else {
								bestVersion = MavenVersionComparator.max(
										bestVersion, loopVersion);
							}
						}
					}
				}
			}*/
		} catch (std::exception& e) {
		}
	} else {
		std::cerr << "No result retrieve... The version cannot be resolved."
				<< std::endl;
		return NULL;
	}
	/*
	 try {
	 //still not resolve try the local resolution
	 if(localDeploy) {
	 Pattern pattern = Pattern.compile("<versions>(\\s|.)*</versions>");
	 Matcher matcher = pattern.matcher(flatFile);
	 String bestVersion = "-1";
	 while (matcher.find()) {
	 Pattern patternVersion = Pattern.compile("(<version>)((\\d|\\w|[-]|\\S)*)</version>");
	 Matcher matcher2 = patternVersion.matcher(matcher.group());
	 while (matcher2.find()) {
	 for(int i=2;i<matcher2.groupCount();i++) {
	 String loopVersion = matcher2.group(i).trim();
	 if(release) {
	 if(!loopVersion.toLowerCase().contains("snapshot")) {
	 bestVersion = MavenVersionComparator.max(bestVersion,loopVersion);
	 }
	 } else {
	 bestVersion = MavenVersionComparator.max(bestVersion,loopVersion);
	 }
	 }
	 }
	 }
	 }
	 } catch (Exception e) {
	 Log.error("Maven Resolver internal error !", e);
	 }
	 }*/
	return NULL;
}

std::string MavenVersionResolver::buildCacheFilePath(MavenArtefact artefact,
		std::string basePath, std::string remoteURL) {
	std::string filePath;
	filePath.append(basePath);
	if (!basePath.find(PATH_SEPARATOR, basePath.length() - 2)
			== basePath.length() - 1) {
		filePath.append(PATH_SEPARATOR);
	}

	std::string newString = artefact.getGroup();
//	int index = 0;
//	index = newString.find(".", index);
//	while (index != std::string::npos) {
//		newString.replace(index, 1, PATH_SEPARATOR);
//		index = newString.find(".", index);
//	}
	maven_resolver::api::replace(newString, ".", maven_resolver::api::fileSeparator);

	filePath.append(newString);
	filePath.append(PATH_SEPARATOR);
	filePath.append(artefact.getName());
	filePath.append(PATH_SEPARATOR);
	filePath.append(metaFile);
	filePath.append("-");

	newString = remoteURL;
//	index = 0;
//	index = newString.find("/", index);
//	while (index != std::string::npos) {
//		newString.replace(index, 1, "_");
//		index = newString.find("/", index);
//	}
	maven_resolver::api::replace(newString, "/", "_");

//	index = 0;
//	index = newString.find(":", index);
//	while (index != std::string::npos) {
//		newString.replace(index, 1, "_");
//		index = newString.find(":", index);
//	}
	maven_resolver::api::replace(newString, ":", "_");

//	index = 0;
//	index = newString.find(".", index);
//	while (index != std::string::npos) {
//		newString.replace(index, 1, "_");
//		index = newString.find(".", index);
//	}
	maven_resolver::api::replace(newString, ".", "_");
	filePath.append(newString);

	return filePath;
}
