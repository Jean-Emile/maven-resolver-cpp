/*
 * MavenDownloader.cpp
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#include <maven-resolver/api/MavenDownloader.h>

#include <iostream>
#include <fstream>

#include <maven-resolver/api/Utilities.h>
#include <network/http/api/HTTPRequest.h>
#include <network/http/impl/BoostHTTPClient.h>

namespace maven {
namespace resolver {

MavenDownloader::MavenDownloader() {
	client = new network::http::BoostHTTPClient();
}

MavenDownloader::~MavenDownloader() {
	delete client;
}

bool MavenDownloader::downloadArtefact(std::string targetFilePath, MavenArtefact artefact, std::string resolvedVersion,
		std::list<std::string> repoUrls) {

	for (std::list<std::string>::iterator it = repoUrls.begin(); it != repoUrls.end(); it++) {
		try {
			//BUILD HTTP URL
			std::string url = buildUrl(artefact, resolvedVersion, *it, false);
			std::cout << "Downloading File  => " << url << std::endl;
			//DOWNLOAD FILE
			return downloadFile(targetFilePath, url, std::ios::out | std::ios::binary | std::ios::trunc);
		} catch (std::exception & e) {
			//TODO CLEANUP FILE IF CORRUPTED
			return false;
		}

	}
	return false;

}
bool MavenDownloader::downloadMetadata(std::string targetFilePath, MavenArtefact artefact, std::string resolvedVersion,std::list<std::string> repoUrls) {
	for (std::list<std::string>::iterator it = repoUrls.begin(); it != repoUrls.end(); it++) {
		//BUILD HTTP URL
		std::string url = buildUrl(artefact, resolvedVersion, *it, true);
		//std::cout << "downloadMetadata" <<  " " << url << std::endl;
		try
		{
			//DOWNLOAD FILE
			return downloadFile(targetFilePath, url, std::ios::out | std::ios::trunc);
		} catch (std::exception & e) {
			//TODO CLEANUP FILE IF CORRUPTED
			std::cerr << "ERROR : downloadMetadata" <<  " " << url <<  e.what() << std::endl;
			return false;
		}

	}
	return false;
}
//bool MavenDownloader::downloadPom(
//		std::string targetFilePath, MavenArtefact, std::string extension,
//		std::string resolvedVersion, std::list<std::string> repoUrls) {
//	return downloadFile(targetFilePath, , ios::out | ios::trunc);
//}

bool MavenDownloader::downloadFile(std::string targetFilePath, std::string url, std::ios_base::openmode openMode) {

	network::http::HTTPRequest request;
	request.addHeader("User-Agent", "KevoreeMavenResolver-cpp");
	request.setUrl(url);
	network::http::HTTPResponse * response = client->doGet(request);

	bool result;
	if (response->getStatus() == network::http::ok) {
		std::ofstream file;
		file.open(targetFilePath.c_str(), openMode);
		std::istream * stream = response->getStream();
		std::ostringstream out;
		out << stream->rdbuf();
		file << out.str();
		file.flush();
		file.close();
		//std::cout << "File saved on " << targetFilePath << std::endl;
		result = true;
	} else {
		//std::cerr << "HTTP Status of the response (" << response->getStatus() << ") for url: " << url << std::endl;
		result = false;
	}
	delete response;
	return result;
}

std::string MavenDownloader::buildUrl(MavenArtefact artefact, std::string resolvedVersion, std::string repoUrl, bool metaFile) {

	std::string url;
	url.append(repoUrl);
	url.append(urlSeparator);
	std::string newString(artefact.getGroup());
	newString = replace(newString, ".", urlSeparator);
	url.append(newString);
	url.append(urlSeparator);
	url.append(artefact.getName());
	url.append(urlSeparator);

	std::string askedVersion = toLower(artefact.getVersion());
	if (askedVersion.compare("release") != 0 && askedVersion.compare("latest") != 0) {
		url.append(artefact.getVersion());
		url.append(urlSeparator);
	}

	if (metaFile) {
		url.append(maven::resolver::metaFile);
	} else {
		url.append(artefact.getName());
		url.append("-");
		if (resolvedVersion.compare("") != 0) {
			url.append(resolvedVersion);
		} else {
			url.append(artefact.getVersion());
		}
		url.append(".");
		url.append(artefact.getExtension());
	}
//	std::cout << "URL to use: " << url << std::endl;
	return url;
}


}
}
