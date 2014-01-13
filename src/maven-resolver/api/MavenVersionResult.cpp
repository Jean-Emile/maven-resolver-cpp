#include <stdlib.h>     /* atol */
#include <iostream>

#include "maven-resolver/api/MavenVersionResult.h"

MavenVersionResult::MavenVersionResult() {
	notDeployed = false;
}

MavenVersionResult::~MavenVersionResult() {
}

std::string MavenVersionResult::getLastUpdate() {
	return lastUpdate;
}

void MavenVersionResult::setLastUpdate(std::string lastUpdate) {
	this->lastUpdate = lastUpdate;
}

std::string MavenVersionResult::getValue() {
	return value;
}

void MavenVersionResult::setValue(std::string value) {
	this->value = value;
}

std::string MavenVersionResult::getUrl_origin() {
	return url_origin;
}

void MavenVersionResult::setUrl_origin(std::string url_origin) {
	this->url_origin = url_origin;
}

bool MavenVersionResult::isPrior(MavenVersionResult remote) {
	try {
		return atol(lastUpdate.c_str()) < atol(remote.lastUpdate.c_str());
	} catch (std::exception & e) {
		std::cerr << "Bad timestamp for the artifact" << std::endl;
		return false;
	}
}

bool MavenVersionResult::isNotDeployed() {
	return notDeployed;
}

void MavenVersionResult::setNotDeployed(bool notDeployed) {
	this->notDeployed = notDeployed;
}
