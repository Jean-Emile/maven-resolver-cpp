#include "MavenArtefact.h"

MavenArtefact::MavenArtefact()
{
}

MavenArtefact::~MavenArtefact()
{
}



std::string  MavenArtefact::getExtension() {
	return extension;
}

void MavenArtefact::setExtension(std::string  _extension) {
	extension = _extension;
}

std::string  MavenArtefact::getVersion() {
	return version;
}

void MavenArtefact::setVersion(std::string  _version) {
	version = _version;
}

std::string  MavenArtefact::getGroup() {
	return group;
}

void MavenArtefact::setGroup(std::string  _group) {
	group = _group;
}

std::string  MavenArtefact::getName() {
	return name;
}

void MavenArtefact::setName(std::string  _name) {
	name = _name;
}
