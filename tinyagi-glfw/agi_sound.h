#pragma once

#include <stdint.h>
#include <stdbool.h>

void agi_sound_init();
void agi_sound_start(uint8_t* new_sound_data);
void agi_sound_stop();
bool agi_sound_fill();