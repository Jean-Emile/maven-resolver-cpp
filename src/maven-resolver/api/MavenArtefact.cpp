#include "maven-resolver/api/MavenArtefact.h"

maven_resolver::api::MavenArtefact::MavenArtefact() {
}

maven_resolver::api::MavenArtefact::~MavenArtefact() {
}

std::string maven_resolver::api::MavenArtefact::getExtension() {
	return extension;
}

void maven_resolver::api::MavenArtefact::setExtension(std::string _extension) {
	extension = _extension;
}

std::string maven_resolver::api::MavenArtefact::getVersion() {
	return version;
}

void maven_resolver::api::MavenArtefact::setVersion(std::string _version) {
	version = _version;
}

std::string maven_resolver::api::MavenArtefact::getGroup() {
	return group;
}

void maven_resolver::api::MavenArtefact::setGroup(std::string _group) {
	group = _group;
}

std::string maven_resolver::api::MavenArtefact::getName() {
	return name;
}

void maven_resolver::api::MavenArtefact::setName(std::string _name) {
	name = _name;
}
