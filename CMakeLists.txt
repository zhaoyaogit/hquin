cmake_minimum_required(VERSION 2.9)
project(hquin C CXX)

set(CMAKE_CXX_COMPILER "clang++")
set(CXX_FLAGS
	-std=c++17
	-O3
	-Wall
	-Werror
	-Wno-unused-parameter
	-march=native
	-fno-inline
	# -MMD
)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

# set(CMAKE_VERBOSE_MAKEFILE ON)
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