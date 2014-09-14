project(Game)
cmake_minimum_required(VERSION 2.8)

#build type
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

#flags
add_definitions(-std=c++11 -Wall -Wextra)

#project files and include pathes
include_directories(.)
include_directories(include)
file(GLOB_RECURSE SRCS "src/" *.cpp)

#libraries include pathes
include_directories(SYSTEM "D:/libs/boost")
include_directories(SYSYEM "D:/libs/Catch")

#build library
add_library(ecsLib ${SRCS})

#build library unit tests
file(GLOB_RECURSE UTEST tests/* *.cpp)
add_executable(tests ${UTEST})
target_link_libraries(tests ecsLib)
