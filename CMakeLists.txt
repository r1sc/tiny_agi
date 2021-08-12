# CMakeList.txt : CMake project for tinyagi, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.13)

set(BUILD_FOR_PICO OFF CACHE BOOL "Build for Pico")

if(BUILD_FOR_PICO)
	message("Building for pico 1")
	include(tinyagi-pico/pico_sdk_import.cmake)
endif()

project(tinyagi_global)

add_subdirectory(agi)

if(BUILD_FOR_PICO)
	message("Building for Pico 2")
	add_subdirectory(tinyagi-pico)
else()
	message("Building for Win32")
	add_subdirectory(tinyagi-glfw)
endif()