/*
 * Utilities.h
 *
 *  Created on: 7 janv. 2014
 *      Author: edaubert
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR "/"
#endif

namespace maven_resolver {
namespace api {
const std::string fileSeparator = PATH_SEPARATOR;

std::string replace(std::string strToModify, std::string toReplace, std::string replacement);
std::string toLower(std::string content);
std::string trim(std::string str);
std::string readFile(std::string filePath);
bool mkdirs(std::string filePath);
}
}

#endif
