cmake_minimum_required(VERSION 2.9)
project(hquin C CXX)

set(CMAKE_CXX_COMPILER "g++")
set(CXX_FLAGS
	-std=c++14
	-g
	-Wall
	-Werror
	-march=native
)

option(HQUIN_BUILD_TESTS "Build tests" OFF)
option(HQUIN_BUILD_CLANG "clang++" OFF)
option(HQUIN_BUILD_RELEASE "Release version" OFF)

include_directories($(PROJECT_SOURCE_DIR)/include)

add_subdirectory(src)
add_subdirectory(tests)

if (HQUIN_BUILD_TESTS)
endif()

if (HQUIN_BUILD_CLANG)
	set(CMAKE_CXX_COMPILER "clang++")
endif()

if (HQUIN_BUILD_RELEASE)
	set(CMAKE_BUILD_TYPE "Release")
else()
	set(CMAKE_BUILD_TYPE "Debug")
endif()