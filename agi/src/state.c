#include "actions.h"
#include "state.h"
#include "platform_support.h"
#include "text_display.h"

#include <stdlib.h>
#include <string.h>

agi_state_t state;

void free_menu_item(menu_item_t* node) {
	if (node->next) {
		free_menu_item(node->next);
	}
	free(node);
}

void free_menu(menu_header_t *node) {
	if (node->next) {
		free_menu(node->next);
	}
	free(node);
}

void free_controller(controller_assignment_t* node) {
	if (node->next) {
		free_controller(node->next);
	}
	free(node);
}

void state_reset() {
	state.pc = 0;
	state.test = false;
	state.and_result = true;
	state.or_result = true;
	state.or = false;
	state.negate = false;
	state.current_logic = 0;

	state.stack_ptr = 0;

	state.cycle_complete = false;

	for (size_t i = 0; i < 256; i++)
	{
		state.scan_start[i] = 0;
		state.variables[i] = 0;
		state.flags[i] = false;
	}

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
	state.input_prompt_active = false;
	state.cursor_char = '_';
	state.input_buffer[0] = '\0';
	state.input_pos = 0;

	state.block_active = false;

	for (size_t i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		state.objects[i].active = false;
		state.objects[i].drawn = false;
		state.objects[i].update = true;
		state.objects[i].old_view_no = -1;		
	}

	state.display_fg = 15;
	state.display_bg = 0;

	state.sound_flag = -1;

	for (size_t i = 0; i < MAX_NUM_CONTROLLERS; i++)
	{
		state.controllers[i] = false;
	}
	
	state.num_parsed_word_groups = 0;


	state.variables[VAR_20_COMPUTER_TYPE] = 0; // IBM-PC
	state.variables[VAR_22_SOUND_TYPE] = 3; // Tandy
	state.variables[VAR_26_MONITOR_TYPE] = 3; // EGA
	state.flags[FLAG_9_SOUND_ENABLED] = true;

	if (state.first_menu) {
		free_menu(state.first_menu);
	}
	state.first_menu = NULL;
	state.current_menu = &state.first_menu;
	state.prev_menu = NULL;
	state.prev_menu_item = NULL;

	if (state.first_controller_assignment) {
		free_controller(state.first_controller_assignment);
	}
	state.first_controller_assignment = NULL;
	state.current_controller_assignment = &state.first_controller_assignment;

	state.game_state = STATE_PLAYING;
}