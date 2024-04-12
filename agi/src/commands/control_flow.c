#include "../actions.h"
#include "../state.h"
#include "../heap.h"
#include "../decryption.h"
#include "../vol.h"
#include "../view.h"
#include "../constants.h"
#include "../platform_support.h"

#include <stdlib.h>

void call(uint8_t logicNo)
{
	load_logic_no_script_write(logicNo);

	state.callstack[state.stack_ptr].logic_no = state.current_logic;
	state.callstack[state.stack_ptr].pc = state.pc;
	state.stack_ptr++;

	state.current_logic = logicNo;
	state.pc = state.scan_start[state.current_logic];
}

void call_v(uint8_t var)
{
	uint8_t logicNo = state.variables[var];
	call(logicNo);
}

void load_logic_no_script_write(uint8_t num)
{
	if (heap_data.loaded_logics[num].buffer)
		return;

	heap_data.loaded_logics[num] = load_vol_data("logdir", num, true);
	decrypt_messages(num);
}

void load_logics(uint8_t num)
{
	if (num != 0)
	{
		heap_write_script_entry(SCRIPT_ENTRY_LOAD_LOGIC, num);
	}
	load_logic_no_script_write(num);
}

void load_logics_v(uint8_t var)
{
	load_logics(state.variables[var]);
}

void new_room(uint8_t room_no)
{
	stop_sound();

	stop_update(0);
	for (uint8_t objNo = 0; objNo < MAX_NUM_OBJECTS; objNo++)
	{
		object_t* obj = &state.objects[objNo];

		obj->active = false;
		obj->drawn = false;
		obj->update = true;
		obj->old_view_no = -1;
		obj->step_size = obj->step_time = obj->steps_to_next_update = obj->cycle_time = obj->cycles_to_next_update = 1;
	}

	player_control();

	unblock();

	set_horizon(36);

	state.variables[VAR_1_PREVIOUS_ROOM] = state.variables[VAR_0_CURRENT_ROOM];
	state.variables[VAR_0_CURRENT_ROOM] = room_no;
	state.variables[VAR_4_OBJ_BORDER_OBJNO] = 0;
	state.variables[VAR_5_OBJ_BORDER_CODE] = 0;
	state.variables[VAR_16_EGO_VIEW_NO] = EGO.view_no;

	state.variables[VAR_8_NUM_PAGES_FREE] = 10;

	// if ego touching edge place ego
	switch (state.variables[VAR_2_EGO_BORDER_CODE])
	{
	case BORDER_TOP:
		EGO.y = 165;
		break;
	case BORDER_BOTTOM:
		EGO.y = state.horizon + 1;
		break;
	case BORDER_LEFT:
		EGO.x = 160 - _object_cell(&EGO)->width;
		break;
	case BORDER_RIGHT:
		EGO.x = 0;
		break;
	}

	state.variables[VAR_2_EGO_BORDER_CODE] = BORDER_NOTHING;
	state.flags[FLAG_5_ROOM_EXECUTED_FIRST_TIME] = true;
	state.variables[VAR_9_MISSING_WORD_NO] = 0;
	state.flags[FLAG_2_COMMAND_ENTERED] = false;

	for (size_t i = 1; i < 256; i++)
	{
		state.scan_start[i] = 0;
	}

	state.current_logic = 0;
	state.pc = state.scan_start[0];
	state.stack_ptr = 0;

	if (heap_data.script_size == 0)
	{
		heap_data.script_size = 50;
	}

	heap_reset();
}

void new_room_v(uint8_t var)
{
	new_room(state.variables[var]);
}

void pop_script()
{
	UNIMPLEMENTED
}

void push_script()
{
	UNIMPLEMENTED
}

void reset_scan_start()
{
	state.scan_start[state.current_logic] = 0;
}

void _return()
{
	if (state.current_logic == 0)
	{
		state.cycle_complete = true;
		return;
	}
	if (state.stack_ptr == 0)
	{
		panic("Logic stack underflow!");
		return;
	}

	logicStackEntry_t prevLogic = state.callstack[--state.stack_ptr];
	state.current_logic = prevLogic.logic_no;
	state.pc = prevLogic.pc;
}

void set_scan_start()
{
	state.scan_start[state.current_logic] = state.pc;
}
