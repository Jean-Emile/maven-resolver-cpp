cmake_minimum_required(VERSION 2.8)
project(maven-resolver-cpp)


message("Boost: ${BOOST_ROOT}")
include_directories(${BOOST_ROOT}/include)
include_directories(.)

link_directories(${BOOST_ROOT}/lib)

set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE maven_resolver_src_files maven-resolver/*)
file(GLOB_RECURSE network_src_files network/*)

#message("maven-resolver sources: ${maven_resolver_src_files}\n")
#message("network sources: ${network_src_files}\n")



ADD_LIBRARY(http-client  STATIC  ${network_src_files})

ADD_LIBRARY(maven-resolver-cpp   STATIC  ${maven_resolver_src_files})
TARGET_LINK_LIBRARIES(maven-resolver-cpp 	boost_system)
TARGET_LINK_LIBRARIES(maven-resolver-cpp 	boost_regex)
TARGET_LINK_LIBRARIES(maven-resolver-cpp 	boost_filesystem)
TARGET_LINK_LIBRARIES(maven-resolver-cpp 	http-client)

