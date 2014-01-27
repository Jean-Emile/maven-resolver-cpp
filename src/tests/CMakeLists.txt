cmake_minimum_required(VERSION 2.8)
project(tests)

message("Boost: ${BOOST_ROOT}")
include_directories(${BOOST_ROOT}/include)
include_directories(../)

link_directories(${BOOST_ROOT}/lib)


SET(COMMON_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
SET(COMMON_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libs")

file(GLOB_RECURSE files_tests src/tests/*.cpp)
add_executable(
        run_tests_maven_resolver
        src/run_tests.cpp
        ${files_tests}
)
TARGET_LINK_LIBRARIES(run_tests_maven_resolver maven-resolver-cpp )
TARGET_LINK_LIBRARIES(run_tests_maven_resolver pthread )
TARGET_LINK_LIBRARIES(run_tests_maven_resolver cppunit)
