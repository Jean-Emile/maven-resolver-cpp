cmake_minimum_required(VERSION 2.8)
project(maven-resolver-cpp)

if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(arch_hint "x64")
elseif("${CMAKE_GENERATOR}" MATCHES "ARM")
    set(arch_hint "ARM")
elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(arch_hint "x64")
elseif("$ENV{LIB}" MATCHES "(amd64|ia64)")
    set(arch_hint "x64")
endif()

if(NOT arch_hint)
   set(arch_hint "x86")
   set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/libs/boost/boost_elf32-i386/)
endif()
 
if(arch_hint MATCHES "ARM")
     set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/libs/boost/boost_elf32-arm/)
endif() 
if(arch_hint MATCHES "x64")
     set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/libs/boost/boost_efl64-x64/)
endif()  
 
if(ANDROID)
 set(arch_hint "ARM")
#jni is available by default on android
#find_package(JNI REQUIRED)
#include_directories(${JNI_INCLUDE_DIRS})
set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/libs/boost/boost_android_r8e/)
set(BOOST_INCLUDEDIR ${CMAKE_SOURCE_DIR}/libs/boost/boost_android_r8e/)
endif()

#add_subdirectory (libs)
add_subdirectory (src)
add_subdirectory (src/tests/)

