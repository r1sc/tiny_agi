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
	UNIMPLEMENTED
}

void close_window()
{
	UNIMPLEMENTED
}

void configure_screen(uint8_t pPlaytop, uint8_t pInputLine, uint8_t pStatusLine)
{
	state.play_top = pPlaytop;
	state.input_line_row = pInputLine;
	state.status_line = pStatusLine;
}

void display(uint8_t row, uint8_t col, uint8_t msg)
{
	char *message = _get_message(msg);
	_draw_text(&row, &col, message, state.display_fg, state.display_bg);
}

void display_v(uint8_t vRow, uint8_t vCol, uint8_t vMsg)
{
	display(state.variables[vRow], state.variables[vCol], state.variables[vMsg]);
}

void graphics()
{
	UNIMPLEMENTED
}

void open_dialogue()
{
	UNIMPLEMENTED
}

void _next_word(const char *str, char **end_i)
{
	for (char *c = (char *)str;; c++)
	{
		if (*c == '\0' || *c == ' ' || *c == '\n')
		{
			*end_i = c;
			return;
		}
	}
}

void _find_longest_line(const char *message, uint8_t max_cols, uint8_t *num_cols, uint8_t *num_lines)
{
	int col = 0;
	int row = 0;
	char *start = (char *)message;
	char *end = start;
	*num_cols = 0;

	while (*end != '\0')
	{
		_next_word(start, &end);
		int word_len = end - start + 1;

		if (col + word_len >= max_cols)
		{
			if (col > *num_cols)
			{
				*num_cols = col;
			}
			col = 0;
			row++;
		}

		col += word_len;

		if (*end == '\n')
		{
			col = 0;
			row++;
		}

		start = end + 1;
	}

	*num_lines = row;
}

void _print(const char *message, int col, int row, uint8_t max_width)
{
	_draw_all_active();

	uint8_t width, height;
	_find_longest_line(message, max_width, &width, &height);

	if (col == -1)
		col = 20 - (max_width >> 1);
	if (row == -1)
		row = 10 - (height >> 1);

	int start_col = col;
	int start_row = row;
	int written_line_chars = 0;
	char *start = (char *)message;
	char *end = start;

	while (*end != '\0')
	{
		_next_word(start, &end);
		int word_len = end - start + 1;

		if (written_line_chars + word_len >= max_width)
		{
			for (size_t i = written_line_chars; i < width; i++)
			{
				_draw_char((start_col + i) * 8, row * 8, ' ', 0, 15);
			}
			col = start_col;
			row++;
			written_line_chars = 0;
		}

		for (; start <= end; start++)
		{
			if (*start == '\0')
				break;
			_draw_char(col * 8, row * 8, *start, 0, 15);
			col++;
			written_line_chars++;
		}

		if (*end == '\n')
		{
			col = start_col;
			row++;
			written_line_chars = 0;
		}
	}

	for (size_t i = written_line_chars; i < width; i++)
	{
		_draw_char((start_col + i) * 8, row * 8, ' ', 4, 15);
	}

	row++;
	for (size_t x = start_col; x < start_col + width - 1; x++)
	{
		_draw_char(x * 8, (start_row - 1) * 8, 0xC4, 4, 15);
		_draw_char(x * 8, row * 8, 0xC4, 4, 15);
	}
	for (size_t y = start_row; y < row; y++)
	{
		_draw_char((start_col - 1) * 8, y * 8, 0xB3, 4, 15);
		_draw_char((start_col + width - 1) * 8, y * 8, 0xB3, 4, 15);
	}
	_draw_char((start_col - 1) * 8, (start_row - 1) * 8, 0xDA, 4, 15);
	_draw_char((start_col + width - 1) * 8, (start_row - 1) * 8, 0xBF, 4, 15);
	_draw_char((start_col - 1) * 8, row * 8, 0xC0, 4, 15);
	_draw_char((start_col + width - 1) * 8, row * 8, 0xD9, 4, 15);

	wait_for_enter();
	// _undraw_all();
	show_pic();
}

void print(uint8_t msg)
{
	char *message = _get_message(msg);
	_print(message, -1, -1, 32);
}

void print_at(uint8_t msg, uint8_t row, uint8_t col, uint8_t maxWidth)
{
	char *message = _get_message(msg);
	_print(message, col, row, maxWidth);
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
	char *message = _get_message(msg);
	state.cursor_char = message[0];
}

void set_text_attribute(uint8_t fg, uint8_t bg)
{
	state.display_bg = bg > 0 ? 15 : 0;
	state.display_fg = state.display_bg == 15 ? 0 : fg;
}

void shake_screen(uint8_t num)
{
	UNIMPLEMENTED
}

void status_line_off()
{
	state.status_line_on = false;
	_redraw_status_line();
}

void status_line_on()
{
	state.status_line_on = true;
	_redraw_status_line();
}

void _redraw_status_line() {
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