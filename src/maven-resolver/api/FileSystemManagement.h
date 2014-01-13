/*
 * FileSystemManagement.h
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#ifndef FILESYSTEMMANAGEMENT_H_
#define FILESYSTEMMANAGEMENT_H_

#define PATH_SEPARATOR "/"

namespace maven_resolver {
namespace api {
const std::string fileSeparator = PATH_SEPARATOR;

void replace(std::string& strToModify, std::string toReplace,
		std::string replacement) {
	int index = 0;
	index = strToModify.find(".", index);
	while (index != std::string::npos) {
		strToModify.replace(index, 1, PATH_SEPARATOR);
		index = strToModify.find(".", index);
	}
}
}
}

#endif /* FILESYSTEMMANAGEMENT_H_ */
