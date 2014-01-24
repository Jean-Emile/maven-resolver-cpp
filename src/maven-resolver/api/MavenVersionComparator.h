/*
 * MavenVersionComparator.h
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#ifndef MAVENVERSIONCOMPARATOR_H_
#define MAVENVERSIONCOMPARATOR_H_

#include <string>

namespace maven {
namespace resolver {

class VersionItem {
private:
	int major;
	int minor;
	int incremental;
	std::string qualifier;

public:
	void setMajor(int major);
	void setMinor(int minor);
	void setIncremental(int incremental);
	void setQualifier(std::string qualifier);
	int getMajor();
	int getMinor();
	int getIncremental();
	std::string getQualifier();
	int compare(VersionItem anotherVersion);
};

class MavenVersionComparator {
public:
	std::string max(std::string version, std::string otherVersion);
private:
	VersionItem * parseVersion(std::string version);
};

}
}

#endif /* MAVENVERSIONCOMPARATOR_H_ */
