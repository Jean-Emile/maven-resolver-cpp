cmake_minimum_required(VERSION 2.8)
project(maven-resolver-cpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")
find_package(Boost 1.5.3 COMPONENTS  filesystem system thread regex)
if(Boost_FOUND)
message("maven-resolver-cpp boost  ${Boost_INCLUDE_DIRS}")
ELSE()
include(add_boost) 
message(boost_ ${CMAKE_CURRENT_SOURCE_DIR}/Third_Parties/boost_${BoostVersion})
set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Third_Parties/boost_${BoostVersion})
endif()

#add_subdirectory (libs)
add_subdirectory (src)
#add_subdirectory (src/tests/)

