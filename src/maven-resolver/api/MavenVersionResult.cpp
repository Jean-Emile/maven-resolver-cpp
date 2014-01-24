#include <stdlib.h>     /* atol */
#include <iostream>

#include "maven-resolver/api/MavenVersionResult.h"
namespace maven {
namespace resolver {

MavenVersionResult::MavenVersionResult() {
	notDeployed = false;
}

MavenVersionResult::~MavenVersionResult() {
}

std::string MavenVersionResult::getLastUpdate() {
	return lastUpdate;
}

void MavenVersionResult::setLastUpdate(std::string _lastUpdate) {
	lastUpdate = _lastUpdate;
}

std::string MavenVersionResult::getValue() {
	return value;
}

void MavenVersionResult::setValue(std::string _value) {
	value = _value;
}

std::string MavenVersionResult::getUrl_origin() {
	return url_origin;
}

void MavenVersionResult::setUrl_origin(std::string _url_origin) {
	url_origin = _url_origin;
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

void MavenVersionResult::setNotDeployed(bool _notDeployed) {
	notDeployed = _notDeployed;
}

std::string MavenVersionResult::toString() {
	return url_origin + " -> " + value + "/" + lastUpdate;
}

}
}
