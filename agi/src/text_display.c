#include <stdio.h>

#include "text_display.h"
#include "platform_support.h"
#include "state.h"
#include "actions.h"


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
				char* msg = get_message(num_buffer);
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

void _redraw_prompt() {	
	uint8_t row = state.input_line_row;
	uint8_t col = 2;
	_draw_text(&row, &col, "                                        ", 0, 0);
	
	_draw_char(0, state.input_line_row * 8, state.strings[0][0], 15, 0);
	_draw_char((1 + state.input_pos) * 8, state.input_line_row * 8, state.cursor_char, 15, 0);

	row = state.input_line_row;
	col = 1;
	_draw_text(&row, &col, state.input_buffer, 15, 0);
}