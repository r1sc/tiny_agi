#include "../actions.h"
#include "../state.h"
#include "../platform_support.h"
#include "../text_display.h"

#include <stdlib.h>

void accept_input() {
	state.input_prompt_active = true;
	_redraw_prompt();
}

void cancel_line() {
	UNIMPLEMENTED
}

void echo_line() {
	UNIMPLEMENTED
}

void init_disk() {
	UNIMPLEMENTED
}

void init_joy() {
	UNIMPLEMENTED
}

void pause() {
	UNIMPLEMENTED
}

void player_control() {
	state.program_control = false;
}

void prevent_input() {
	state.input_prompt_active = false;
	clear_lines(state.input_line_row, state.input_line_row, 0);
}

void program_control() {
	state.program_control = true;
}

void quit(uint8_t noConfirm) {
	panic("Logic called quit");
}

void restart_game() {
	UNIMPLEMENTED
}

void restore_game() {
	agi_reset();

	agi_save_data_file_ptr file = agi_save_data_open("rb");
	agi_save_data_read(file, &state, sizeof(agi_state_t));
	
	uint8_t num_items;
	agi_save_data_read(file, &num_items, sizeof(uint8_t));	
	for (size_t i = 0; i < num_items; i++)
	{
		agi_save_data_read(file, (void*)&(heap_data.item_file->items[i].room_no), sizeof(uint8_t));
	}

	heap_data.script_entries = (script_entry_t*)malloc(state.script_size * sizeof(script_entry_t));
	// replay scripts
	for (int i = 0; i < state.script_entry_pos; i++)
	{		
		agi_save_data_read(file, (void*)&(heap_data.script_entries[i]), sizeof(script_entry_t));
	}
	
	bool write_lock = state.flags[FLAG_7_SCRIPT_BUFFER_WRITE_LOCK];
	state.flags[FLAG_7_SCRIPT_BUFFER_WRITE_LOCK] = true;

	for (int i = 0; i < state.script_entry_pos; i++)
	{
		script_entry_t entry = heap_data.script_entries[i];
		switch(entry.script_type) {
			case SCRIPT_ENTRY_LOAD_LOGIC:
				load_logics(entry.resource_number);
				break;
			case SCRIPT_ENTRY_LOAD_VIEW:
				load_view(entry.resource_number);
				break;
			case SCRIPT_ENTRY_LOAD_PIC:
				load_pic_no(entry.resource_number);
				break;
			case SCRIPT_ENTRY_LOAD_SOUND:
				load_sound(entry.resource_number);
				break;
			case SCRIPT_ENTRY_DRAW_PIC:
				draw_pic_no(entry.resource_number);
				break;
			case SCRIPT_ENTRY_ADD_TO_PIC:
				entry = heap_data.script_entries[++i];
				uint8_t view_no = entry.script_type;
				uint8_t loop_no = entry.resource_number;
				entry = heap_data.script_entries[++i];
				uint8_t cel_no = entry.script_type;
				uint8_t x = entry.resource_number;
				entry = heap_data.script_entries[++i];
				uint8_t y = entry.script_type;
				uint8_t pri = entry.resource_number;
				add_to_pic(view_no, loop_no, cel_no, x, y, pri, 0);
				break;
			case SCRIPT_ENTRY_DISCARD_PIC:
				discard_pic_no(entry.resource_number);
				break;
			case SCRIPT_ENTRY_OVERLAY_PIC:
				overlay_pic_no(entry.resource_number);
				break;
		}
	}
	agi_save_data_close(file);
	
	state.flags[FLAG_7_SCRIPT_BUFFER_WRITE_LOCK] = write_lock;
	// state.flags[FLAG_12_GAME_RESTORED] = true;

	show_pic();
}

void save_game() {
	agi_save_data_file_ptr file = agi_save_data_open("wb");
	agi_save_data_write(file, &state, sizeof(agi_state_t));

	agi_save_data_write(file, &(heap_data.item_file->num_objects), sizeof(uint8_t));
	for (size_t i = 0; i < heap_data.item_file->num_objects; i++)
	{
		agi_save_data_write(file, &(heap_data.item_file->items[i].room_no), sizeof(uint8_t));
	}

	for (int i = 0; i < state.script_entry_pos; i++)
	{
		agi_save_data_write(file, &(heap_data.script_entries[i]), sizeof(script_entry_t));
	}
	
	agi_save_data_close(file);
}

void script_size(uint8_t num) {
	state.script_size = num;
	heap_data.script_entries = malloc(num * sizeof(script_entry_t));
}

void set_game_id(uint8_t msg) {
	//_read_message(msg);
}

void set_simple(uint8_t a) {
	UNIMPLEMENTED
}

void toggle_monitor() {
	UNIMPLEMENTED
}
