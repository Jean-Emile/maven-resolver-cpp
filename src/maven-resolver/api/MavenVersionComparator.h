/*
 * MavenVersionComparator.h
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#ifndef MAVENVERSIONCOMPARATOR_H_
#define MAVENVERSIONCOMPARATOR_H_

#include <string>

namespace maven_resolver {
namespace api {

class MavenVersionComparator {
public:
	std::string max(std::string version, std::string otherVersion);
};
}
}

#endif /* MAVENVERSIONCOMPARATOR_H_ */
