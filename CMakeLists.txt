cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(audio-streamer)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

find_package(Qt5Multimedia 5.7 REQUIRED)
find_package(Qt5Network 5.7 REQUIRED)
find_package(Qt5Widgets 5.7 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)
