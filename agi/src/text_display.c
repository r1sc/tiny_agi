#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "text_display.h"
#include "platform_support.h"
#include "state.h"
#include "actions.h"
#include "constants.h"
#include "heap.h"

void _draw_char(unsigned int start_x, unsigned int start_y, unsigned char c, uint8_t fg, uint8_t bg) {
	if (c == '\n')
		return;

	if (start_y >= 200)
		return;

	for (unsigned int y = 0; y < 8; y++) {
		int screen_y = y + start_y;
		unsigned char rowData = font_data[c * 8 + y];
		for (unsigned int x = 0; x < 8; x++) {
			int screen_x = x + start_x;
			bool on = (rowData & 0x80) == 0x80;

			screen_set_320(screen_x, screen_y, on ? fg : bg);
			if (screen_y > state.play_top * 8 && screen_y < 168) {
				priority_set(screen_x >> 1, screen_y - (state.play_top * 8), 255);
			}
			rowData = rowData << 1;
		}
	}
}

void _draw_text(uint8_t* row, uint8_t* col, const char* text, uint8_t fg, uint8_t bg) {
	unsigned int i = 0;
	uint8_t num_buffer = 0;
	
	while (1) {
		char c = text[i++];
		if (c == '%') {
			char mode = text[i++];
			num_buffer = 0;
			while (1) {
				c = text[i++];
				if (c < '0' || c > '9') {
					break;
				}
				num_buffer = (num_buffer * 10) + (c - '0');
			}

			switch (mode) {
			case 'v': { // Variable
				int pad = 0;
				if (c == '|') {
					while (1) {
						c = text[i++];
						if (c < '0' || c > '9') {
							break;
						}
						pad = (pad * 10) + (c - '0');
					}
				}
				char buf[8];
				sprintf(buf, "%0*d", pad, state.variables[num_buffer]);
				_draw_text(row, col, buf, fg, bg);
				break;
			}
			case 's': { // String
				char* str = state.strings[num_buffer];
				_draw_text(row, col, str, fg, bg);
				break;
			}
			case 'm': { // Message
				const char* msg = get_message(state.current_logic, num_buffer);
				_draw_text(row, col, msg, fg, bg);
				break;
			}
			}
		}

		if (c == '\0') {
			break;
		}
		else if (c == '\\') {
			// TODO: Parse actual token here
			continue;
		}
		else if (c == '\n') {
			(*row)++;
			(*col) = 0;
		}
		else {
			_draw_char((*col) * 8, (*row) * 8, c, fg, bg);
			(*col)++;
		}
	}
}

void redraw_prompt(const char* prompt_text) {	
	if (!state.input_prompt_active)
		return;

	uint8_t row = state.input_line_row;
	uint8_t col = 0;
	_draw_text(&row, &col, "                                        ", 0, 0);
	
	//_draw_char(0, state.input_line_row * 8, state.strings[0][0], 15, 0);
	row = state.input_line_row;
	col = 0;
	_draw_text(&row, &col, prompt_text, 15, 0);
	size_t len = strlen(prompt_text);
	_draw_char((len + system_state.input_pos) * 8, state.input_line_row * 8, state.cursor_char, 15, 0);

	row = state.input_line_row;
	col = (uint8_t)len;
	_draw_text(&row, &col, system_state.input_buffer, 15, 0);
}

int read_num(const char** source) {
	int num = 0;
	while (**source >= '0' && **source <= '9') {
		int digit = **source - '0';
		num = (num * 10) + digit;
		(*source)++;
	}
	return num;
}

static int line_width;
static int total_line_width;
static int rows;
static int max_width;

char* format_and_print(char* dest, const char* source) {
	char* last_space = dest;
	while (*source != '\0') {
		if (line_width == max_width) {
			if (last_space != NULL) {
				*last_space = '\n';
				int offset = dest - last_space;
				if (line_width - offset > total_line_width) {
					total_line_width = line_width - offset;
				}
				dest = last_space + 1;
				source -= offset;
				last_space = NULL;
				while (*source == ' ')
					source++;
			}
			else {
				*dest = '\n';
				dest++;
			}

			line_width = 0;
			rows++;
		}
		else {
			if (*source == '\\') {
				source++;
				*dest = *source;
				dest++;
				line_width++;
			}
			else if (*source == '\n') {
				*dest = *source;
				source++;
				dest++;

				if (line_width > total_line_width) {
					total_line_width = line_width;
				}
				line_width = 0;
				rows++;
			}
			else {
				switch (*source) {
				case '%':
					source++;
					switch (*source) {
					case 'g':
						source++;
						int global_message_no = read_num(&source);
						const char* global_message = get_message(0, global_message_no);
						dest = format_and_print(dest, global_message);
						break;
					case 'm':
						source++;
						int message_no = read_num(&source);
						const char* message = get_message(state.current_logic, message_no);
						dest = format_and_print(dest, message);
						break;
					case 's':
						source++;
						int str_no = read_num(&source);
						char* str = state.strings[str_no];
						dest = format_and_print(dest, (const char*)str);
						break;
					case 'v':
						source++;
						int var_no = read_num(&source);
						int num_width = 0;
						if (*source == '|') {
							num_width = read_num(&source);
						}
						char num_str[4];
						sprintf(num_str, "%0*d", num_width, state.variables[var_no]);
						dest = format_and_print(dest, (const char*)num_str);
						break;
					case 'w':
						source++;
						int word_no = read_num(&source);
						dest = format_and_print(dest, "<some word>");
						break;
					case 'o':
						source++;
						int obj_no = read_num(&source);
						const char* description = (const char*)(((uint8_t*)heap_data.item_file->items) + heap_data.item_file->items[obj_no].name_offset);
						dest = format_and_print(dest, description);
						break;
					}
					break;
				case ' ':
					last_space = dest;
				default:
					*dest = *source;
					dest++;
					line_width++;
					source++;
				}
			}
		}
	}
	if (line_width > total_line_width) {
		total_line_width = line_width;
	}
	*dest = '\0';
	return dest;
}

void print_message_box(const char* text, int width, int desired_row, int desired_col) {
	static char dest[600];
	
	line_width = 0;
	rows = 0;
	total_line_width = 0;
	max_width = width;

	format_and_print(dest, text);	

	uint8_t start_row = 11 - (rows >> 1);
	if (desired_row > 0) {
		start_row = desired_row;
	}
	uint8_t row = start_row;

	uint8_t start_col = 20 - (total_line_width >> 1);
	if (desired_col > 0) {
		start_col = desired_col;
	}
	uint8_t end_col = start_col + total_line_width - 1;
	uint8_t col = start_col;

	char* s = dest;

	while (*s != '\0') {
		if (*s == '\n') {
			while (col <= end_col) {
				_draw_char(col * 8, row * 8, ' ', 0, 15);
				col++;
			}
			row++;
			col = start_col;
		}
		else {
			_draw_char(col * 8, row * 8, *s, 0, 15);
			col++;
		}
		s++;
	}
	while (col <= end_col) {
		_draw_char(col * 8, row * 8, ' ', 0, 15);
		col++;
	}

	start_row--;
	row++;
	start_col--;
	end_col++;
	
	_draw_char(start_col * 8, start_row * 8, CORNER_NW_BAR, 4, 15);
	_draw_char(start_col * 8, row * 8, CORNER_SW_BAR, 4, 15);	
	_draw_char(end_col * 8, start_row * 8, CORNER_NE_BAR, 4, 15);
	_draw_char(end_col * 8, row * 8, CORNER_SE_BAR, 4, 15);
	
	for (size_t c = start_col + 1; c < end_col; c++) {
		_draw_char(c * 8, start_row * 8, HORIZONTAL_BAR, 4, 15);
		_draw_char(c * 8, row * 8, HORIZONTAL_BAR, 4, 15);
	}
	for (size_t r = start_row + 1; r < row; r++) {
		_draw_char(start_col * 8, r * 8, VERTICAL_BAR, 4, 15);
		_draw_char(end_col * 8, r * 8, VERTICAL_BAR, 4, 15);
	}

	state.variables[VAR_21_MESSAGE_WINDOW_TIMER] = 0;
	state.flags[FLAG_15_NON_BLOCKING_WINDOWS] = false;
}