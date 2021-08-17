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
	bool active;
	bool drawn;
	bool fix_loop;
	bool update;
	bool observe_horizon;
	bool is_cycling;
	bool collide_with_objects;
	bool collide_with_blocks;

	int x, y;
	uint8_t view_no;
	uint8_t loop_no;
	uint8_t cel_no;

	int old_x, old_y;
	int old_view_no;
	uint8_t old_loop_no;
	uint8_t old_cel_no;
	
	int8_t fixed_priority;
	uint8_t allowed_on;
	
	uint8_t cycling_mode;
	uint8_t cycle_time;
	uint8_t cycles_to_next_update;
	
	int move_mode;	
	uint8_t step_time;
	uint8_t steps_to_next_update;

	uint8_t step_size;

	int move_distance_x, move_distance_y;
	uint8_t move_step_size;
	int move_done_flag;
	
	uint8_t end_of_loop_flag;

	uint8_t direction;

	uint8_t wander_distance;
} object_t;

#pragma pack(push,1)
typedef struct {
	int x1, y1;
	int x2, y2;
} rect_t;
#pragma pack(pop)

typedef struct
{
	uint8_t scancode;
	char ascii;
	uint8_t controller_no;
} controller_assignment_t;

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

} agi_state_t;

typedef struct {
	uint8_t room_no;
} item_save_data_t;

extern agi_state_t state;
#define EGO state.objects[0]

/* Initializes the agi state */
void state_reset();

/* Updates and draws all active objects */
void agi_draw_all_active();

/* Steps the simulation */
void agi_logic_run_cycle();
