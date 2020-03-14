cmake_minimum_required(VERSION 2.9)
project(hquin C CXX)

set(CXX_FLAGS
	-std=c++14
	-Og
	-g
	-Wall
	-Werror
	-Wno-unused-parameter
	-march=native
	-fno-inline
	# -MMD
)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__REL_FILE__='\"$(subst ${PROJECT_SOURCE_DIR}/,,$(abspath $<))\"'")

# set(CMAKE_VERBOSE_MAKEFILE ON)
option(HQUIN_BUILD_TESTS "Build tests" OFF)
option(HQUIN_BUILD_CLANG "clang++" OFF)
option(HQUIN_BUILD_RELEASE "Release version" OFF)


include_directories($(PROJECT_SOURCE_DIR)/hquin)
include_directories($(PROJECT_SOURCE_DIR)/limlog)

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

if (LIMLOG_NO_FILE_FUNC_LINE)
	add_definitions(-DNO_FILE_FUNC_LINE)
endif()

add_subdirectory(limlog)
add_subdirectory(hquin)
add_subdirectory(tests)