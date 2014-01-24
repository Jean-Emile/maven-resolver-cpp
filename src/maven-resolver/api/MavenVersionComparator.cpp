/*
 * MavenVersionComparator.cpp
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */
#include "maven-resolver/api/MavenVersionComparator.h"

#include <cstdlib>
#include <iostream>

namespace maven {
namespace resolver {

void VersionItem::setMajor(int _major) {
	major = _major;
}
void VersionItem::setMinor(int _minor) {
	minor = _minor;
}
void VersionItem::setIncremental(int _incremental) {
	incremental = _incremental;
}
void VersionItem::setQualifier(std::string _qualifier) {
	qualifier = _qualifier;
}
int VersionItem::getMajor() {
	return major;
}
int VersionItem::getMinor() {
	return minor;
}
int VersionItem::getIncremental() {
	return incremental;
}
std::string VersionItem::getQualifier() {
	return qualifier;
}

int VersionItem::compare(VersionItem anotherVersion) {
	if (major == anotherVersion.getMajor()) {
		if (minor == anotherVersion.getMinor()) {
			if (incremental == anotherVersion.getIncremental()) {
				return qualifier.compare(anotherVersion.getQualifier());
			} else {
				return anotherVersion.getIncremental() - incremental;
			}
		} else {
			return anotherVersion.getMinor() - minor;
		}
	} else {
		return anotherVersion.getMajor() - major;
	}
}

VersionItem * MavenVersionComparator::parseVersion(std::string version) {
	VersionItem * versionItem = new VersionItem();
	// find the number of element in the version <major>.<minor>.<incremental>-<qualifier>
	int nbElement = 0;
	int index = 0;
	int previousIndex = 0;
	index = version.find(".", index);
	std::string listItems[3];
	while (index != std::string::npos) {
		listItems[nbElement] = version.substr(previousIndex, index);
		previousIndex = index;
		nbElement++;
		index = version.find(".", index+1);
	}
	listItems[nbElement] = version.substr(previousIndex, index);

//	std::cout <<listItems[0] << " " << listItems[1] << " " << listItems[2] << std::endl;

	if (nbElement == 0) {
		index = version.find_first_of("-");
		if (index != std::string::npos) {
			versionItem->setMajor(atoi(version.substr(index).c_str()));
			versionItem->setQualifier(version.substr(index, version.length()));
		} else {
			versionItem->setMajor(atoi(version.c_str()));
		}
	} else if (nbElement == 1) {
		versionItem->setMajor(atoi(listItems[0].c_str()));
		index = listItems[1].find_first_of("-");
		if (index != std::string::npos) {
			versionItem->setMinor(atoi(listItems[1].substr(index).c_str()));
			versionItem->setQualifier(listItems[1].substr(index, version.length()));
		} else {
			versionItem->setMinor(atoi(listItems[1].c_str()));
		}
	} else if (nbElement == 2) {
		versionItem->setMajor(atoi(listItems[0].c_str()));
		versionItem->setMinor(atoi(listItems[1].c_str()));
		index = listItems[2].find_first_of("-");
		if (index != std::string::npos) {
			versionItem->setIncremental(atoi(listItems[2].substr(index).c_str()));
			versionItem->setQualifier(listItems[2].substr(index, version.length()));
		} else {
			versionItem->setIncremental(atoi(listItems[2].c_str()));
		}
	}

	return versionItem;
}

std::string MavenVersionComparator::max(std::string version, std::string anotherVersion) {
	if (version.compare("") != 0 && anotherVersion.compare("") == 0) {
		return version;
	}
	if (anotherVersion.compare("") != 0 && version.compare("") == 0) {
		return anotherVersion;
	}
	VersionItem* v1c = parseVersion(version);
	VersionItem* v2c = parseVersion(anotherVersion);

//	std::cout << version << " compare to " << anotherVersion << " == " << v1c->compare(*v2c) << std::endl;
	bool compare = v1c->compare(*v2c) < 0;
	delete v1c;
	delete v2c;

	if (compare) {
//		std::cout << "Return " << version << std::endl;
		return version;
	} else {
//		std::cout << "Return " << anotherVersion << std::endl;
		return anotherVersion;
	}
	return version;
}


}
}
