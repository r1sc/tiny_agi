#pragma once

#include <stdint.h>

void _draw_char(unsigned int x, unsigned int y, unsigned char c, uint8_t fg, uint8_t bg);
void _draw_text(uint8_t* row, uint8_t* col, const char* text, uint8_t fg, uint8_t bg);
void redraw_prompt(const char* prompt_text);
void print_message_box(const char* text, int max_width, int desired_row, int desired_col);