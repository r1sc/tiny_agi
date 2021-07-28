#include "text_display.h"
#include "platform_support.h"

void _draw_text(uint8_t row, uint8_t col, const char* text, uint8_t fg, uint8_t bg) {
	unsigned int i = 0;
	while (1) {
		char c = text[i++];
		if (c == '\0')
			break;
		if (c == '\n')
		{
			row++;
			col = 0;
			continue;
		}
		draw_char(col * 8, row * 8, c, fg, bg);
		col++;
	}
}

void _draw_text_len(uint8_t row, uint8_t col, const char* text, uint8_t fg, uint8_t bg, int start, int end) {
	for (int i = start; i <= end; i++)
	{
		char c = text[i];
		if (c == '\0')
			break;
		draw_char(col * 8, row * 8, c, fg, bg);
		col++;
	}
}