#pragma once

#include <stdint.h>

char* _get_message(uint8_t message_no);
void _draw_text(uint8_t row, uint8_t col, const char* text, uint8_t fg, uint8_t bg);
void _draw_text_len(uint8_t row, uint8_t col, const char* text, uint8_t fg, uint8_t bg, int start, int end);