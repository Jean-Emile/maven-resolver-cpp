cmake_minimum_required(VERSION 2.8)
project(maven-resolver-cpp)


find_package(Boost 1.5.3 COMPONENTS  filesystem system thread regex)
if(Boost_FOUND)
#message("maven-resolver-cpp boost  ${Boost_INCLUDE_DIRS}")
endif()

#add_subdirectory (libs)
add_subdirectory (src)
#add_subdirectory (src/tests/)

