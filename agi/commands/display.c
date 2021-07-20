#include "../state.h"
#include "../actions.h"
#include "../platform_support.h"
#include "../text_display.h"

void clear_lines(uint8_t top, uint8_t bottom, uint8_t color) {
	for (int row = top; row <= bottom; row++)
	{
		_draw_text(row, 0, "                                        ", 0, 0);
	}
}

void clear_text_rect(uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4, uint8_t num5) {
	UNIMPLEMENTED
}

void close_dialogue() {
	UNIMPLEMENTED
}

void close_window() {
	UNIMPLEMENTED
}

void configure_screen(uint8_t pPlaytop, uint8_t pInputLine, uint8_t pStatusLine) {
	state.play_top = pPlaytop;
	state.input_line_row = pInputLine;
	state.status_line = pStatusLine;
}

void display(uint8_t row, uint8_t col, uint8_t msg) {
	_read_message(msg);
	//if (messageBuffer[0] == '%')
	//{
	//	// TODO: Replace with strings
	//}
	//draw_text(row, 0, "                                        ", 0, 0);

	_draw_text(row, col, message_buffer, 15, 0);
}

void display_v(uint8_t vRow, uint8_t vCol, uint8_t vMsg) {
	display(state.variables[vRow], state.variables[vCol], state.variables[vMsg]);
}

void graphics() {
	UNIMPLEMENTED
}

void open_dialogue() {
	UNIMPLEMENTED
}

void print(uint8_t msg) {
	//_print(msg, -1, -1, 40);
	UNIMPLEMENTED
}

void print_at(uint8_t msg, uint8_t row, uint8_t col, uint8_t maxWidth) {
	//_print(msg, col, row, maxWidth);
	UNIMPLEMENTED
}

void print_at_v(uint8_t var, uint8_t row, uint8_t col, uint8_t maxWidth) {
	print_at(state.variables[var], row, col, maxWidth);
}

void print_v(uint8_t var) {
	print(state.variables[var]);
}

void set_cursor_char(uint8_t msg) {
	_read_message(msg);
	state.cursor_char = message_buffer[0];
}

void set_text_attribute(uint8_t num, uint8_t num2) {
	UNIMPLEMENTED
}

void shake_screen(uint8_t num) {
	UNIMPLEMENTED
}

void status_line_off() {
	UNIMPLEMENTED
}

void status_line_on() {
	UNIMPLEMENTED
}

void text_screen() {
	UNIMPLEMENTED
}