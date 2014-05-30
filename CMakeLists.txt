project(Game)
cmake_minimum_required(VERSION 2.8)

#debug
set(CMAKE_BUILD_TYPE "Debug")
add_definitions(-O0)

#release
#set(CMAKE_BUILD_TYPE "Release")
#add_definitions(-O3)

include_directories(src)
include_directories(src/client)

aux_source_directory(src/core SRC_CORE)
aux_source_directory(src/tool SRC_TOOLS)
aux_source_directory(src/common SRC_COMMON)
aux_source_directory(src/client/components SRC_GAMEPLAY_COMPONENTS)
aux_source_directory(src/client/tasks SRC_GAMEPLAY_SYSTEMS)
aux_source_directory(src/client/graphics SRC_GAMEPLAY_GRAPHICS)

#flags
add_definitions(-std=c++11 -Wall -Wextra -DBOOST_TEST_DYN_LINK )

#build and link game
add_executable(game src/client/main.cpp ${SRC_CORE} ${SRC_GAMEPLAY_COMPONENTS} ${SRC_GAMEPLAY_SYSTEMS} ${SRC_GAMEPLAY_GRAPHICS} ${SRC_TOOLS} ${SRC_COMMON})
target_link_libraries(game sfml-system sfml-window sfml-graphics sfml-network sfml-audio)

#build and link configuration system unit tests
add_executable(configTest src/test/core/configTest.cpp src/tool/config.cpp src/tool/logger.cpp)
target_link_libraries(configTest boost_unit_test_framework)

#build and link component manager unit tests
add_executable(componentsTest src/test/core/componentsTest.cpp src/tool/logger.cpp src/tool/profiler.cpp src/tool/config.cpp)
target_link_libraries(componentsTest sfml-system boost_unit_test_framework)

#build and link task manager unit tests
add_executable(taskTest src/test/core/tasksTest.cpp src/core/taskManager.cpp src/tool/logger.cpp src/tool/profiler.cpp src/tool/config.cpp src/core/engine.cpp)
target_link_libraries(taskTest sfml-system boost_unit_test_framework)

#buind and link event system unit tests
add_executable(eventsTest src/test/core/eventsTest.cpp)
target_link_libraries(eventsTest boost_unit_test_framework)
