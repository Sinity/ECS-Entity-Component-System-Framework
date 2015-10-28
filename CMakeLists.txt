project(Game)
cmake_minimum_required(VERSION 2.8)

#build type
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

#flags
add_definitions(-std=c++14 -Wall -Wextra -m64)

#libs
include_directories(SYSTEM D:/libs/boost)

#project files and include pathes
include_directories(.)
include_directories(include)
include_directories(src)
include_directories(src/3party)
file(GLOB_RECURSE SRCS "src/*.cpp")

#build library
add_library(ecsLib ${SRCS})

#build library unit tests
file(GLOB_RECURSE UTEST "tests/*.cpp")
add_executable(tests ${UTEST})
target_link_libraries(tests ecsLib)
