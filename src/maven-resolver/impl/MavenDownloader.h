/*
 * MavenDownloader.h
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#ifndef MAVENDOWNLOADER_H_
#define MAVENDOWNLOADER_H_

#include <string>
#include <list>

#include "maven-resolver/api/MavenConstants.h"
#include "maven-resolver/api/MavenArtefact.h"
#include <network/http/impl/BoostHTTPClient.h>

namespace maven_resolver{
namespace impl {

const std::string urlSeparator = "/";

class MavenDownloader{
public:
	MavenDownloader();
	~MavenDownloader();
	bool downloadArtefact(std::string targetFile, maven_resolver::api::MavenArtefact artefact, std::string resolvedVersion, std::list<std::string> repoUrls);
	bool downloadMetadata(std::string targetFilePath, maven_resolver::api::MavenArtefact artefact, std::string resolvedVersion, std::list<std::string> repoUrls);
//	bool downloadPom(std::string targetFilePath, MavenArtefact artefact, std::string extension, std::string resolvedVersion, std::list<std::string> repoUrls);
private:
	std::string buildUrl(maven_resolver::api::MavenArtefact artefact, std::string resolvedVersion, std::string repoUrl, bool metaFile);
	bool downloadFile(std::string targetFilePath, std::string url, std::ios_base::openmode openMode);

	network::http::impl::BoostHTTPClient * client;
};

}
}


#endif /* MAVENDOWNLOADER_H_ */
