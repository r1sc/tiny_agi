#pragma once

#include "../src/state.h"
#include "../src/platform_support.h"
#include "../src/sound.h"

void agi_initialize();
void print_message_box(const char* text, int max_width, int desired_row, int desired_col);