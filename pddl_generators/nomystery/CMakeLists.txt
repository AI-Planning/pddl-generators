cmake_minimum_required(VERSION 3.4)
project(nomysterygenerator)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pg")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    src/state.cpp
    src/state.h
    src/globals.cpp
    src/globals.h
    src/search.cpp
    src/search.h
    src/utils.cpp
    src/utils.h
    src/state_registry.cpp
    src/state_registry.h
    src/segmented_vector.h
    src/per_state_information.h
    src/open_list.h
    src/open_list.cpp
    src/heuristic.h
    src/heuristic.cpp)

add_executable(nomystery src/main.cpp ${SOURCE_FILES})
