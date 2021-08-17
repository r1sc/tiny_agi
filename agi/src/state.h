#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "vol.h"
#include "constants.h"

typedef struct {
	unsigned int logic_no;
	unsigned int pc;
} logicStackEntry_t;

typedef struct {
	int x, y;
	uint8_t view_no;
	uint8_t loop_no;
	uint8_t cel_no;

	int old_x, old_y;
	int old_view_no;
	uint8_t old_loop_no;
	uint8_t old_cel_no;
	
	int move_mode;

	bool active;
	bool drawn;
	
	bool fix_loop;
	int8_t fixed_priority;
	bool update;
	bool observe_horizon;
	uint8_t allowed_on;
	uint8_t cycle_time;
	uint8_t cycles_to_next_update;
	uint8_t step_time;
	uint8_t steps_to_next_update;

	bool is_cycling;
	uint8_t cycling_mode;

	bool collide_with_objects;
	uint8_t step_size;
	bool collide_with_blocks;
	int move_distance_x, move_distance_y;
	uint8_t move_step_size;
	int move_done_flag;
	uint8_t end_of_loop_flag;
	uint8_t direction;
} object_t;

#pragma pack(push,1)
typedef struct {
	int x1, y1;
	int x2, y2;
} rect_t;

typedef struct {
	uint16_t name_offset;
	uint8_t room_no;
} item_t;

typedef struct {
	uint16_t item_names_offset;
	uint8_t num_objects;
	item_t items[];
} item_file_t;

typedef struct {
	uint8_t hi_byte;
	uint8_t lo_byte;
} uint16_be_t;

typedef struct {
	uint8_t prefix;
	uint8_t data[];
} word_entry_t;

typedef struct {
	uint16_be_t word_indices[25];
	uint8_t data[];
} words_file_t;
#pragma pack(pop)

typedef struct
{
	uint8_t scancode;
	char ascii;
	uint8_t controller_no;
} controller_assignment_t;

typedef struct {
	uint8_t script_type;
	uint8_t resource_number;
} script_entry_t;

typedef struct {
	vol_data_t loaded_logics[256];	
	vol_data_t loaded_pics[256];
	vol_data_t loaded_views[256];
	vol_data_t loaded_sounds[256];
	script_entry_t *script_entries;
	
	item_file_t* item_file;
	words_file_t* words_file;	
} agi_heap_t;

typedef struct {
	/* Interpreter state */
	unsigned int pc;
	bool test;
	bool and_result;
	bool or_result;
	bool or;
	bool negate;
	uint8_t current_logic;

	logicStackEntry_t callstack[AGI_CALLSTACK_DEPTH];
	unsigned int stack_ptr;
	
	bool cycle_complete;

	uint16_t scan_start[256];	
	uint8_t variables[256];
	bool flags[256];
	char strings[12][40];

	uint8_t play_top;
	uint8_t input_line_row;
	uint8_t status_line;
	bool program_control;
	bool enter_pressed;
	uint8_t horizon;
	bool input_prompt_active;
	char cursor_char;
	char input_buffer[40];
	uint8_t input_pos;
	
	bool block_active;
	rect_t block;

	object_t objects[MAX_NUM_OBJECTS];

	uint8_t display_fg;
	uint8_t display_bg;

	int sound_flag;

	bool status_line_on;

	bool controllers[MAX_NUM_CONTROLLERS];	
	controller_assignment_t controller_assignments[MAX_NUM_CONTROLLERS];

	bool escape_pressed;

	uint16_t parsed_word_groups[20];
	uint16_t num_parsed_word_groups;

	int script_size;
	int script_entry_pos;
} agi_state_t;

typedef struct {
	uint8_t room_no;
} item_save_data_t;

extern agi_heap_t heap_data;
extern agi_state_t state;
#define EGO state.objects[0]

void agi_free_heap();
bool agi_discard(vol_data_t* vol_data);

void write_script_entry(uint8_t script_type, uint8_t resource_no);
void write_add_to_pic_script_entry(uint8_t view_no, uint8_t loop_no, uint8_t cel_no, uint8_t x, uint8_t y, uint8_t pri);

void clear_controller_assignments();

/* Initializes the agi state */
void agi_reset();

/* Steps the simulation */
void agi_logic_run_cycle();

/* Updates and draws all active objects */
void _draw_all_active();