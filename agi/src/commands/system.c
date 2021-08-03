#include "../actions.h"
#include "../state.h"
#include "../platform_support.h"
#include "../text_display.h"

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
	UNIMPLEMENTED
}

void save_game() {
	UNIMPLEMENTED
}

void script_size(uint8_t num) {
	WARNING("Not sure what this does, but find out")
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
