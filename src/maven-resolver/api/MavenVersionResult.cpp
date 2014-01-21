#include <stdlib.h>     /* atol */
#include <iostream>

#include "maven-resolver/api/MavenVersionResult.h"

maven_resolver::api::MavenVersionResult::MavenVersionResult() {
	notDeployed = false;
}

maven_resolver::api::MavenVersionResult::~MavenVersionResult() {
}

std::string maven_resolver::api::MavenVersionResult::getLastUpdate() {
	return lastUpdate;
}

void maven_resolver::api::MavenVersionResult::setLastUpdate(std::string _lastUpdate) {
	lastUpdate = _lastUpdate;
}

std::string maven_resolver::api::MavenVersionResult::getValue() {
	return value;
}

void maven_resolver::api::MavenVersionResult::setValue(std::string _value) {
	value = _value;
}

std::string maven_resolver::api::MavenVersionResult::getUrl_origin() {
	return url_origin;
}

void maven_resolver::api::MavenVersionResult::setUrl_origin(std::string _url_origin) {
	url_origin = _url_origin;
}

bool maven_resolver::api::MavenVersionResult::isPrior(MavenVersionResult remote) {
	try {
		return atol(lastUpdate.c_str()) < atol(remote.lastUpdate.c_str());
	} catch (std::exception & e) {
		std::cerr << "Bad timestamp for the artifact" << std::endl;
		return false;
	}
}

bool maven_resolver::api::MavenVersionResult::isNotDeployed() {
	return notDeployed;
}

void maven_resolver::api::MavenVersionResult::setNotDeployed(bool _notDeployed) {
	notDeployed = _notDeployed;
}

std::string maven_resolver::api::MavenVersionResult::toString() {
	return url_origin + " -> " + value + "/" + lastUpdate;
}
