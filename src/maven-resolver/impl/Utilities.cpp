/*
 * Utilities.cpp
 *
 *  Created on: 21 janv. 2014
 *      Author: edaubert
 */
#include <maven-resolver/api/Utilities.h>

#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

namespace maven {
namespace resolver {

std::string replace(std::string strToModify, std::string toReplace, std::string replacement) {
	int index = 0;
	index = strToModify.find(toReplace, index);
	while (index != std::string::npos) {
		strToModify.replace(index, toReplace.length(), replacement);
		index = strToModify.find(toReplace, index);
	}
	return strToModify;
}

std::string toLower(std::string content) {
	for (int q = 0; q < content.length(); q++) {
		content[q] = std::tolower(content[q]);
	}
	return content;
}

std::string trim(std::string str) {
    std::string::size_type begin=0;
    std::string::size_type end=str.size()-1;
    while(begin<=end && (str[begin]<=0x20 || str[begin]==0x7f))
        ++begin;
    while(end>begin && (str[end]<=0x20 || str[end]==0x7f))
      --end;
    str = str.substr(begin, end - begin + 1);
    return str;
}


std::string readFile(std::string filePath) {
	std::fstream file(filePath.c_str(), std::fstream::in);
	if (file.good()) {
		std::string result;
		while (!file.eof()) {
			std::string str;
			file >> str;
			result.append(str);
		}
		return result;
	} else {
//		std::cerr << "File \"" << filePath <<"\" doesn't exist." << std::endl;
		return "";
	}
}

bool mkdirs(std::string filePath) {
	return boost::filesystem::create_directories(boost::filesystem::path(filePath).parent_path());
}

}
}
