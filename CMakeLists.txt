project(Game)
cmake_minimum_required(VERSION 2.8)

#build type
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

#flags
if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++11 -Wall -Wextra)
endif()
if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    add_definitions(/W3)
endif()

include_directories(src)
include_directories(src/client)
file(GLOB_RECURSE SRCS src *.cpp)
file(GLOB_RECURSE HDRS src *.h)

#libraries
include_directories("D:/dev/tools/libs/SFML/include")
link_directories("D:/dev/tools/libs/SFML/lib")
include_directories("D:/dev/tools/libs/boost")
link_directories("D:/dev/tools/libs/boost/stage/lib")
add_definitions(-DBOOST_TEST_DYN_LINK)

#build and link game
add_library(GameLib ${SRCS} ${HDRS})
add_executable(game src/client/main.cpp)
target_link_libraries(game GameLib sfml-system-d sfml-window-d sfml-graphics-d sfml-network-d sfml-audio-d)

#build and link unit tests

add_executable(componentsTest test/core/componentsTest.cpp)
target_link_libraries(componentsTest GameLib)
add_test(componentsTest componentsTest)

add_executable(tasksTest test/core/tasksTest.cpp)
target_link_libraries(tasksTest GameLib sfml-system-d)

add_executable(configTest test/core/configTest.cpp)
target_link_libraries(configTest GameLib)

add_executable(eventsTest test/core/eventsTest.cpp)
target_link_libraries(eventsTest GameLib)

add_executable(formatStringTest test/common/formatString.cpp)
target_link_libraries(formatStringTest GameLib)
