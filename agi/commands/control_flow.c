#include "../actions.h"
#include "../state.h"
#include "../vol.h"

void call(uint8_t logicNo) {
	load_logics(logicNo);

	state.callstack[state.stack_ptr].logic_no = state.current_logic;
	state.callstack[state.stack_ptr].pc = state.pc;
	state.stack_ptr++;

	state.current_logic = logicNo;
	state.pc = state.scan_start[state.current_logic];
}

void call_v(uint8_t var) {
	uint8_t logicNo = state.variables[var];
	call(logicNo);
}

void load_logics(uint8_t num) {
	if (state.loaded_logics[num])
		return;

	state.loaded_logics[num] = load_vol_data("logdir", num).buffer;
}

void load_logics_v(uint8_t var) {
	load_logics(state.variables[var]);
}

void new_room(uint8_t room_no) {
	unanimate_all();
	player_control();
	unblock();
	set_horizon(36);

	state.variables[VAR_1_PREVIOUS_ROOM] = state.variables[VAR_0_CURRENT_ROOM];
	state.variables[VAR_0_CURRENT_ROOM] = room_no;
	state.variables[VAR_16_EGO_VIEW_NO] = state.objects[0].view_no;
	// if ego touching edge place ego

	state.flags[FLAG_5_ROOM_EXECUTED_FIRST_TIME] = true;

	state.current_logic = 0;
	state.pc = state.scan_start[0];
	state.stack_ptr = 0;
}

void new_room_v(uint8_t var) {
	new_room(state.variables[var]);
}

void pop_script() {
	UNIMPLEMENTED
}

void push_script() {
	UNIMPLEMENTED
}

void reset_scan_start() {
	state.scan_start[state.current_logic] = 0;
}

void _return() {
	if (state.current_logic == 0)
	{
		state.cycle_complete = true;
		return;
	}
	if (state.stack_ptr == 0) {
		panic("Logic stack underflow!");
		return;
	}

	logicStackEntry_t prevLogic = state.callstack[--state.stack_ptr];
	state.current_logic = prevLogic.logic_no;
	state.pc = prevLogic.pc;
}

void set_scan_start() {
	state.scan_start[state.current_logic] = state.pc;
}
