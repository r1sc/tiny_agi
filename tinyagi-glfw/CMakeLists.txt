cmake_minimum_required(VERSION 3.13)

project(tinyagi_glfw)

add_subdirectory(deps/glfw-3.3.4)
add_subdirectory(../agi ${CMAKE_CURRENT_BINARY_DIR}/tinyagi)

add_executable (tinyagi_glfw
	"tinyagi.c" 
	"winmm_audio.h" 
	"winmm_audio.c" 
	"panic.h" 
	"synth.h" 
	"synth.c"
)

	
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)

target_link_libraries(tinyagi_glfw glfw OpenGL::GL tinyagi winmm.lib)