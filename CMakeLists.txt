cmake_minimum_required(VERSION 2.8)
project(maven-resolver-cpp)


file(GLOB_RECURSE maven-resolver-cpp-files src/*)
ADD_LIBRARY(maven-resolver-cpp  STATIC    ${maven-resolver-cpp-files})
