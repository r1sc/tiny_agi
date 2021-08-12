# CMakeList.txt : CMake project for tinyagi, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_library (tinyagi STATIC
	"src/actions.h"
	"src/interpreter.c"
	"src/platform_support.h" 
	"src/state.c" 
	"src/state.h" 
	"src/text_display.c" 
	"src/text_display.h" 
	"src/vol.c"
	"src/vol.h"
	"src/constants.h"
	"src/commands/control_flow.c"
	"src/commands/debugging.c"
	"src/commands/display.c"
	"src/commands/flag.c"
	"src/commands/inventory_item.c"
	"src/commands/math.c"
	"src/commands/menu_io.c"
	"src/commands/object_view.c"
	"src/commands/picture.c"
	"src/commands/sound.c"
	"src/commands/string.c"
	"src/commands/system.c"
	"src/commands/test.c" 
	"src/view.c" 
	"src/view.h" )

target_include_directories(tinyagi PUBLIC include)
