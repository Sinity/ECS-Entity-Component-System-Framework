project(Game)
cmake_minimum_required(VERSION 2.8)

#build type
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

#flags
add_definitions(-std=c++11 -m64 -Wall -Wextra)

include_directories(src)
include_directories(src/client)
file(GLOB_RECURSE SRCS src *.cpp)
file(GLOB_RECURSE HDRS src *.h)

#libraries
include_directories(SYSTEM "D:/dev/tools/libs/SFML/include")
link_directories(SYSTEM "D:/dev/tools/libs/SFML/lib")
include_directories(SYSTEM "D:/dev/tools/libs/boost")

#build and link game
add_library(GameLib ${SRCS} ${HDRS})
add_executable(game src/client/main.cpp)
target_link_libraries(game GameLib sfml-system-d sfml-window-d sfml-graphics-d sfml-network-d sfml-audio-d)
