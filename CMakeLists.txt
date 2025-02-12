cmake_minimum_required(VERSION 2.6)

project(echo CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
 -g
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 -std=c++11
 -rdynamic
 )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/base)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/base BASE)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/efs SERV)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/efc CLI)
link_directories(${PROJECT_SOURCE_DIR}/lib)
add_library(efs_base ${BASE})
add_executable(efs ${SERV})
add_executable(efc ${CLI})
target_link_libraries(efs efs_base)
target_link_libraries(efc efs_base)
target_link_libraries(efs pthread rt)
target_link_libraries(efs jsoncpp)
target_link_libraries(efc pthread rt)
target_link_libraries(efc jsoncpp)

add_subdirectory(test)