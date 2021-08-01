include_directories(${tinyagi_glfw_SOURCE_DIR}/agi/include)
link_directories(${tinyagi_glfw_SOURCE_DIR}/agi/src)
add_subdirectory(deps/glfw-3.3.4)

# Add source to this project's executable.
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

# TODO: Add tests and install targets if needed.
