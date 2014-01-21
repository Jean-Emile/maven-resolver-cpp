/*
 * MavenDownloader.cpp
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#include <maven-resolver/impl/MavenDownloader.h>

#include <iostream>
#include <fstream>

#include <maven-resolver/api/Utilities.h>
#include <network/http/api/HTTPRequest.h>

maven_resolver::impl::MavenDownloader::MavenDownloader() {
	client = new network::http::impl::BoostHTTPClient();
}

maven_resolver::impl::MavenDownloader::~MavenDownloader() {
	delete client;
}

bool maven_resolver::impl::MavenDownloader::downloadArtefact(std::string targetFilePath, maven_resolver::api::MavenArtefact artefact, std::string resolvedVersion, std::list<std::string> repoUrls) {

	for (std::list<std::string>::iterator it = repoUrls.begin(); it != repoUrls.end(); it++) {
		try {
			//BUILD HTTP URL
			std::string url = buildUrl(artefact, resolvedVersion, *it, false);
			//DOWNLOAD FILE
			return downloadFile(targetFilePath, url, std::ios::out | std::ios::binary | std::ios::trunc);
		} catch (std::exception & e) {
			//TODO CLEANUP FILE IF CORRUPTED
			return false;
		}

	}
	return false;

}
bool maven_resolver::impl::MavenDownloader::downloadMetadata(std::string targetFilePath, maven_resolver::api::MavenArtefact artefact, std::string resolvedVersion, std::list<std::string> repoUrls) {
	for (std::list<std::string>::iterator it = repoUrls.begin(); it != repoUrls.end(); it++) {
		try {
			//BUILD HTTP URL
			std::string url = buildUrl(artefact, resolvedVersion, *it, true);
			//DOWNLOAD FILE
			return downloadFile(targetFilePath, url, std::ios::out | std::ios::trunc);
		} catch (std::exception & e) {
			//TODO CLEANUP FILE IF CORRUPTED
			return false;
		}

	}
	return false;
}
//bool maven_resolver::impl::MavenDownloader::downloadPom(
//		std::string targetFilePath, MavenArtefact, std::string extension,
//		std::string resolvedVersion, std::list<std::string> repoUrls) {
//	return downloadFile(targetFilePath, , ios::out | ios::trunc);
//}

bool maven_resolver::impl::MavenDownloader::downloadFile(std::string targetFilePath, std::string url, std::ios_base::openmode openMode) {

	network::http::api::HTTPRequest request;
	request.addHeader("User-Agent", "KevoreeMavenResolver-cpp");
	request.setUrl(url);
	network::http::api::HTTPResponse * response = client->doGet(request);

	if (response->getStatus() == network::http::api::ok) {
		std::ofstream file;
		file.open(targetFilePath.c_str(), openMode);
		std::istream * stream = response->getStream();
		std::ostringstream out;
		out << stream->rdbuf();
		file << out.str();
		file.flush();
		file.close();
//		std::cout << "File saved on " << targetFilePath << std::endl;
		delete response;
		return true;
	} else {
//		std::cout << "HTTP Status of the response (" << response->getStatus() << ") is not fine" << std::endl;
		return false;
	}
}

std::string maven_resolver::impl::MavenDownloader::buildUrl(maven_resolver::api::MavenArtefact artefact, std::string resolvedVersion, std::string repoUrl, bool metaFile) {

	std::string url;
	url.append(repoUrl);
	url.append(urlSeparator);
	std::string newString(artefact.getGroup());
	newString = maven_resolver::api::replace(newString, ".", urlSeparator);
	url.append(newString);
	url.append(urlSeparator);
	url.append(artefact.getName());
	url.append(urlSeparator);
	url.append(artefact.getVersion());
	url.append(urlSeparator);
	if (metaFile) {
		url.append(maven_resolver::api::metaFile);
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
	return url;
}

