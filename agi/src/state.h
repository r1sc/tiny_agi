#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "vol.h"

#define VAR_0_CURRENT_ROOM 0
#define VAR_1_PREVIOUS_ROOM 1
#define VAR_2_EGO_BORDER_CODE 2
#define VAR_3_SCORE 3
#define VAR_4_OBJ_BORDER_OBJNO 4
#define VAR_5_OBJ_BORDER_CODE 5
#define VAR_6_EGO_DIRECTION 6
#define VAR_7_MAX_SCORE 7
#define VAR_8_NUM_PAGES_FREE 8
#define VAR_9_MISSING_WORD_NO 9
#define VAR_10_INTERPRETER_CYCLE_TIME 10
#define VAR_11_CLOCK_SECONDS 11
#define VAR_12_CLOCK_MINUTES 12
#define VAR_13_CLOCK_HOURS 13
#define VAR_14_CLOCK_DAYS 14
#define VAR_15_JOYSTICK_SENS 15
#define VAR_16_EGO_VIEW_NO 16
#define VAR_17_INTERPRETER_ERROR_CODE 17
#define VAR_18_ERROR_CODE_PARAMTER 18
#define VAR_19_KEYBOARD_KEY_PRESSED 19
#define VAR_20_COMPUTER_TYPE 20
#define VAR_21_MESSAGE_WINDOW_TIMER 21
#define VAR_22_SOUND_TYPE 22
#define VAR_23_SOUND_VOLUME 23
#define VAR_24_INPUT_BUFFER_SIZE 24
#define VAR_25_CURRENT_ITEM_SELECTED 25
#define VAR_26_MONITOR_TYPE 26

#define FLAG_0_EGO_ON_WATER 0
#define FLAG_1_EGO_HIDDEN 1
#define FLAG_2_COMMAND_ENTERED 2
#define FLAG_3_EGO_TOUCHED_TRIGGER 3
#define FLAG_4_SAID_ACCEPTED_INPUT 4
#define FLAG_5_ROOM_EXECUTED_FIRST_TIME 5
#define FLAG_6_RESTART_GAME_EXECUTED 6
#define FLAG_7_SCRIPT_BUFFER_WRITE_LOCK 7
#define FLAG_8_JOYSTICK_ENABLED 8
#define FLAG_9_SOUND_ENABLED 9
#define FLAG_10_DEBUGGER_ENABLED 10
#define FLAG_11_LOGIC0_FIRST_TIME 11
#define FLAG_12_GAME_RESTORED 12
#define FLAG_13_SHOW_OBJECT_ENABLED 13
#define FLAG_14_MENU_ENABLED 14
#define FLAG_15_NON_BLOCKING_WINDOWS 15
#define FLAG_16_UNK_RESTART 16

#define DIR_STOPPED 0
#define DIR_UP 1
#define DIR_UP_RIGHT 2
#define DIR_RIGHT 3
#define DIR_DOWN_RIGHT 4
#define DIR_DOWN 5
#define DIR_DOWN_LEFT 6
#define DIR_LEFT 7
#define DIR_UP_LEFT 8

#define BORDER_NOTHING 0
#define BORDER_TOP 1
#define BORDER_RIGHT 2
#define BORDER_BOTTOM 3
#define BORDER_LEFT 4

#define AGI_CALLSTACK_DEPTH 8
#define MAX_NUM_OBJECTS 16


typedef struct {
	unsigned int logic_no;
	unsigned int pc;
} logicStackEntry_t;

typedef struct {
	int x, y;
	int move_mode;

	bool active;
	bool drawn;
	uint8_t view_no;
	uint8_t loop_no;
	uint8_t cel_no;
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

typedef struct {
	int x1, y1;
	int x2, y2;
} rect_t;

typedef struct {
	uint8_t room_no;
} item_t;

typedef struct {
	/* Interpreter state */
	unsigned int pc;
	bool test;
	bool and_result;
	bool or_result;
	bool or;
	bool negate;
	uint8_t current_logic;
	uint16_t scan_start[256];
	logicStackEntry_t callstack[AGI_CALLSTACK_DEPTH];
	unsigned int stack_ptr;
	bool cycle_complete;
	uint8_t* loaded_logics[256];

	/* Game state */
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
	char prompt_buffer[28];
	
	bool block_active;
	rect_t block;

	object_t objects[MAX_NUM_OBJECTS];

	vol_data_t loaded_pics[256];
	vol_data_t loaded_views[256];

} agi_state_t;

extern agi_state_t state;

/* Initializes the agi state */
void agi_reset();

/* Steps the simulation */
void agi_logic_run_cycle();

/* Updates and draws all active objects */
void _draw_all_active();

/* Clears all objects from the screen */
void _undraw_all();