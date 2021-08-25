#pragma once

#define VERTICAL_BAR (unsigned char)0xB3
#define HORIZONTAL_BAR (unsigned char)0xC4
#define CORNER_NE_BAR (unsigned char)0xBF
#define CORNER_SW_BAR (unsigned char)0xC0
#define CORNER_SE_BAR (unsigned char)0xD9
#define CORNER_NW_BAR (unsigned char)0xDA

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
#define MAX_NUM_CONTROLLERS 50

#define OBJ_ON_ANYTHING 0
#define OBJ_ON_WATER 1
#define OBJ_ON_LAND 2

#define CYCLE_MODE_NORMAL 1
#define CYCLE_MODE_REVERSE_CYCLE 2
#define CYCLE_MODE_REVERSE_LOOP 3
#define CYCLE_MODE_END_OF_LOOP 4

#define OBJ_MOVEMODE_NORMAL 0
#define OBJ_MOVEMODE_MOVE_TO 1
#define OBJ_MOVEMODE_WANDER 2



#define AGI_KEY_INS 82
#define AGI_KEY_DEL 83
#define AGI_KEY_F1 59
#define AGI_KEY_F2 60
#define AGI_KEY_F3 61
#define AGI_KEY_F4 62
#define AGI_KEY_F5 63
#define AGI_KEY_F6 64
#define AGI_KEY_F7 65
#define AGI_KEY_F8 66
#define AGI_KEY_F9 67
#define AGI_KEY_F10 6

#define AGI_KEY_PGUP 0x49
#define AGI_KEY_PGDN 0x51
#define AGI_KEY_HOME 0x47
#define AGI_KEY_END 0x4f
#define AGI_KEY_UP 0x48
#define AGI_KEY_DOWN 0x50
#define AGI_KEY_LEFT 0x4b
#define AGI_KEY_RIGHT 0x4d


#define SCRIPT_ENTRY_LOAD_LOGIC 0
#define SCRIPT_ENTRY_LOAD_VIEW 1
#define SCRIPT_ENTRY_LOAD_PIC 2
#define SCRIPT_ENTRY_LOAD_SOUND 3
#define SCRIPT_ENTRY_DRAW_PIC 4
#define SCRIPT_ENTRY_ADD_TO_PIC 5
#define SCRIPT_ENTRY_DISCARD_PIC 6
#define SCRIPT_ENTRY_DISCARD_VIEW 7
#define SCRIPT_ENTRY_OVERLAY_PIC 8