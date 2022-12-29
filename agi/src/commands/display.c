#include "../state.h"
#include "../actions.h"
#include "../platform_support.h"
#include "../text_display.h"

void clear_lines(uint8_t top, uint8_t bottom, uint8_t color)
{
	for (uint8_t row = top; row <= bottom; row++)
	{
		uint8_t col = 0;
		_draw_text(&row, &col, "                                        ", 0, color);
	}
}

void clear_text_rect(uint8_t row1, uint8_t col1, uint8_t row2, uint8_t col2, uint8_t color)
{
	color = color > 0 ? 15 : 0;
	for (uint8_t row = row1; row <= row2; row++)
	{
		for (uint8_t col = col1; col <= col2; col++)
		{
			_draw_char(col * 8, row * 8, ' ', 0, color);
		}
	}
}

void close_dialogue()
{
	// DO NOTHING - only needed on Hercules graphics cards
}

void close_window()
{
	show_pic();
}

void configure_screen(uint8_t pPlaytop, uint8_t pInputLine, uint8_t pStatusLine)
{
	state.play_top = pPlaytop;
	state.input_line_row = pInputLine;
	state.status_line = pStatusLine;
}

void display(uint8_t row, uint8_t col, uint8_t msg)
{
	const char *message = get_message(state.current_logic, msg);
	_draw_text(&row, &col, message, state.display_fg, state.display_bg);
}

void display_v(uint8_t vRow, uint8_t vCol, uint8_t vMsg)
{
	display(state.variables[vRow], state.variables[vCol], state.variables[vMsg]);
}

void graphics()
{
	// TODO: Maybe OK, figure out if this is enough
	show_pic();
}

void open_dialogue()
{
	// DO NOTHING - only needed on Hercules graphics cards
}

void _print(const char *message, int row, int col, uint8_t max_width, bool pause)
{
	if (pause) {
		agi_draw_all_active();
	}
	
	print_message_box(message, max_width, row, col);

	if (pause) {
		wait_for_enter();
		show_pic();
	}
}

void print(uint8_t msg)
{
	const char *message = get_message(state.current_logic, msg);
	bool pause = !state.flags[FLAG_15_NON_BLOCKING_WINDOWS]; // TODO: Broken //!state.flags[FLAG_15_NON_BLOCKING_WINDOWS];
	_print(message, 0, 0, 30, pause);
}

void print_at(uint8_t msg, uint8_t row, uint8_t col, uint8_t maxWidth)
{
	const char *message = get_message(state.current_logic, msg);
	bool pause = !state.flags[FLAG_15_NON_BLOCKING_WINDOWS]; // TODO: Broken //!state.flags[FLAG_15_NON_BLOCKING_WINDOWS];
	_print(message, row, col, maxWidth, pause);
}

void print_at_v(uint8_t var, uint8_t row, uint8_t col, uint8_t maxWidth)
{
	print_at(state.variables[var], row, col, maxWidth);
}

void print_v(uint8_t var)
{
	print(state.variables[var]);
}

void set_cursor_char(uint8_t msg)
{
	const char *message = get_message(state.current_logic, msg);
	state.cursor_char = message[0];
}

void set_text_attribute(uint8_t fg, uint8_t bg)
{
	state.display_bg = bg > 0 ? 15 : 0;
	state.display_fg = state.display_bg == 15 ? 0 : fg;
}

void shake_screen(uint8_t num)
{
	agi_shake_screen(num);
}

void status_line_off()
{
	state.status_line_on = false;
	redraw_status_line();
}

void status_line_on()
{
	state.status_line_on = true;
	redraw_status_line();
}

void redraw_status_line() {
	if (!state.status_line_on){
		clear_lines(state.status_line, state.status_line, 0);
	}
	else {
		clear_lines(state.status_line, state.status_line, 15);

		char line[41];

		uint8_t col = 1;
		sprintf(line, "Score:%d of %d\0", state.variables[VAR_3_SCORE], state.variables[VAR_7_MAX_SCORE]);
		_draw_text(&state.status_line, &col, line, 0, 15);
		
		col = 29;
		sprintf(line, "Sound:%s\0", state.flags[FLAG_9_SOUND_ENABLED] ? "on" : "off");
		_draw_text(&state.status_line, &col, line, 0, 15);
	}
}

void text_screen()
{
	clear_lines(0, 25, 0);
}