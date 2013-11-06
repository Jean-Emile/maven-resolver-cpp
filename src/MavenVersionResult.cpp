#include "MavenVersionResult.h"
#include <stdlib.h>     /* atol */

MavenVersionResult::MavenVersionResult()
{
	notDeployed = false;
}

MavenVersionResult::~MavenVersionResult()
{
}

std::string MavenVersionResult::getLastUpdate() {
	return lastUpdate;
}

void MavenVersionResult::setLastUpdate(std::string lastUpdate) {
	lastUpdate = lastUpdate;
}

std::string MavenVersionResult::getValue() {
	return value;
}

void MavenVersionResult::setValue(std::string value) {
	value = value;
}

std::string MavenVersionResult::getUrl_origin() {
	return url_origin;
}

void MavenVersionResult::setUrl_origin(std::string url_origin) {
	url_origin = url_origin;
}


bool MavenVersionResult::isPrior(MavenVersionResult remote) {
	return atol(lastUpdate.c_str()) < atol(remote.lastUpdate.c_str());
}

bool MavenVersionResult::isNotDeployed() {
	return notDeployed;
}

void MavenVersionResult::setNotDeployed(bool notDeployed) {
	notDeployed = notDeployed;
}