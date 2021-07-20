#include "../actions.h"
#include "../state.h"
#include "../platform_support.h"
#include "../text_display.h"

#define PROMPT_CHAR state.strings[0][0]

void _redraw_prompt() {
	draw_char(0, state.input_line_row * 8, PROMPT_CHAR, 15, 0);
	draw_char(8, state.input_line_row * 8, state.cursor_char, 15, 0);
	_draw_text(state.input_line_row, 2, state.prompt_buffer, 15, 0);
}

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
