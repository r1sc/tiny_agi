#include "actions.h"
#include "state.h"
#include "platform_support.h"
#include "text_display.h"

#include <stdlib.h>
#include <string.h>

agi_state_t state;
agi_heap_t heap_data;

void clear_controller_assignments() {
	for (size_t i = 0; i < MAX_NUM_CONTROLLERS; i++)
	{
		state.controllers[i] = false;
	}
}

void agi_reset() {
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
	}

	state.display_fg = 15;
	state.display_bg = 0;

	state.sound_flag = -1;
	
	clear_controller_assignments();
	
	state.escape_pressed = false;

	state.num_parsed_word_groups = 0;

	state.script_entry_pos = 0;	
	
	state.variables[VAR_20_COMPUTER_TYPE] = 0; // IBM-PC
	state.variables[VAR_22_SOUND_TYPE] = 3; // Tandy
	state.variables[VAR_26_MONITOR_TYPE] = 3; // EGA
	state.flags[FLAG_9_SOUND_ENABLED] = true;
	
	agi_free_heap();

	agi_file_t item_file = get_file("object");
	heap_data.item_file = (item_file_t*)malloc(item_file.size);
	memcpy(heap_data.item_file, item_file.data, item_file.size);
	_decrypt_item_file((uint8_t*)heap_data.item_file, item_file.size);

	agi_file_t words_file = get_file("words.tok");
	heap_data.words_file = (words_file_t*)words_file.data;

	load_logic_no_script_write(0);
}

bool agi_discard(vol_data_t* vol_data) {
	if (!vol_data->buffer) {
		return false;
	}
	free(vol_data->buffer);
	vol_data->buffer = NULL;
	vol_data->size = 0;
	return true;
}

void agi_free_heap() {
	for (size_t i = 0; i < 256; i++)
	{	
		agi_discard(&heap_data.loaded_logics[i]);
		agi_discard(&heap_data.loaded_pics[i]);
		agi_discard(&heap_data.loaded_views[i]);
		agi_discard(&heap_data.loaded_sounds[i]);
	}
	
	if(heap_data.script_entries){
		free(heap_data.script_entries);
		heap_data.script_entries = NULL;
	}
}

void write_script_entry(uint8_t script_type, uint8_t resource_no) {
	if(state.flags[FLAG_7_SCRIPT_BUFFER_WRITE_LOCK]) {
		return;
	}
	heap_data.script_entries[state.script_entry_pos].script_type = script_type;
	heap_data.script_entries[state.script_entry_pos].resource_number = resource_no;
	state.script_entry_pos++;
}

void write_add_to_pic_script_entry(uint8_t view_no, uint8_t loop_no, uint8_t cel_no, uint8_t x, uint8_t y, uint8_t pri) {
	write_script_entry(SCRIPT_ENTRY_ADD_TO_PIC, 0);
	write_script_entry(view_no, loop_no);
	write_script_entry(cel_no, x);
	write_script_entry(y, pri);
}
