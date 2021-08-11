# CMakeList.txt : CMake project for tinyagi, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(tinyagi_global)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "/ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "/SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO")
endif()

add_definitions(/ZI)

add_subdirectory(agi)
add_subdirectory(tinyagi-glfw)