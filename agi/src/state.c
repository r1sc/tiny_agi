#include "actions.h"
#include "state.h"
#include "platform_support.h"
#include "text_display.h"

#include <stdlib.h>
#include <string.h>

agi_state_t state;


void agi_reset() {
	state.pc = 0;
	state.test = false;
	state.and_result = true;
	state.or_result = true;
	state.or = false;
	state.negate = false;
	state.current_logic = 0;

	for (size_t i = 0; i < 256; i++)
	{
		state.variables[i] = 0;
		state.flags[i] = false;
		state.scan_start[i] = 0;
		if (state.loaded_logics[i]) {
			free(state.loaded_logics[i]);
		}
		if (state.loaded_pics[i].buffer) {
			free(state.loaded_pics[i].buffer);
		}
		if (state.loaded_views[i].buffer) {
			free(state.loaded_views[i].buffer);
		}
	}
	state.stack_ptr = 0;
	state.cycle_complete = false;

	for (size_t i = 0; i < 12; i++)
	{
		state.strings[i][0] = '\0';
	}
	state.strings[0][0] = '>';
	state.strings[0][1] = '\0';

	state.play_top = 0;
	state.input_line_row = 0;
	state.status_line = 0;
	state.program_control = false;
	state.enter_pressed = false;
	state.horizon = 36;
	state.input_prompt_active = true;
	state.cursor_char = '_';
	state.input_buffer[0] = '\0';
	state.input_pos = 0;

	for (size_t i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		state.objects[i].active = false;
	}

	state.program_control = false;
	state.display_fg = 15;
	state.display_bg = 0;
	state.sound_flag = -1;
	
	state.variables[VAR_20_COMPUTER_TYPE] = 0; // IBM-PC
	state.variables[VAR_22_SOUND_TYPE] = 3; // Tandy
	state.variables[VAR_26_MONITOR_TYPE] = 3; // EGA
	state.flags[FLAG_9_SOUND_ENABLED] = true;

	agi_file_t item_file = get_file("object");
	state.item_file = (item_file_t*)malloc(item_file.size);
	memcpy(state.item_file, item_file.data, item_file.size);
	
	_decrypt_item_file((uint8_t*)state.item_file, item_file.size);

	agi_file_t words_file = get_file("words.tok");
	state.words_file = (words_file_t*)words_file.data;

	new_room(0);
}