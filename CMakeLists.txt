# CMakeList.txt : CMake project for tinyagi, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("tinyagi")

# Add source to this project's executable.
add_executable (tinyagi 
	"tinyagi.c" 
	"agi/actions.h"
	"agi/state.h" 
	"agi/state.c" 
	"agi/interpreter.c" 
	"agi/platform_support.h" 
	"agi/commands/control_flow.c"
	"agi/commands/debugging.c"
	"agi/commands/display.c"
	"agi/commands/flag.c"
	"agi/commands/inventory_item.c"
	"agi/commands/math.c"
	"agi/commands/menu_io.c"
	"agi/commands/object_view.c"
	"agi/commands/picture.c"
	"agi/commands/sound.c"
	"agi/commands/string.c"
	"agi/commands/system.c"
	"agi/commands/test.c"
	"agi/vol.h"
	"agi/vol.c"
	"agi/text_display.c" 
	"agi/text_display.h")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(./deps/glfw-3.3.4)

find_package(OpenGL REQUIRED)

target_link_libraries(tinyagi glfw OpenGL::GL)

# TODO: Add tests and install targets if needed.
