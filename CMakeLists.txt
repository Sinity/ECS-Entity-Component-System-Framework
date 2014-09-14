project(Game)
cmake_minimum_required(VERSION 2.8)

#build type
set(CMAKE_BUILD_TYPE "Debug")     #remember to use appropiate SFML libraries.
#set(CMAKE_BUILD_TYPE "Release")  #for debug with -d suffix, for release without it.

#flags
add_definitions(-std=c++11 -Wall -Wextra)

include_directories(include)
include_directories(src)
file(GLOB_RECURSE SRCS "src/" *.cpp)
file(GLOB_RECURSE HDRS "src/" *.h)

#libraries
include_directories(SYSTEM "D:/libs/SFML/include")
link_directories(SYSTEM "D:/libs/SFML/lib")
include_directories(SYSTEM "D:/libs/boost")
include_directories(SYSYEM "D:/libs/Catch")

#build and link game
add_library(gameLib ${SRCS} ${HDRS})
add_executable(game src/client/main.cpp)
target_link_libraries(game gameLib sfml-system-d sfml-window-d sfml-graphics-d sfml-network-d sfml-audio-d)

#build unit tests
file(GLOB_RECURSE UTEST tests/* *.cpp)
add_executable(tests ${UTEST})
target_link_libraries(tests gameLib sfml-system-d sfml-window-d sfml-graphics-d sfml-network-d sfml-audio-d)
