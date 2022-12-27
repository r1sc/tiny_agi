#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	uint32_t hz;
	uint32_t duration;
	uint8_t attenuation;
} chan_note_t;

#define AGI_NUM_CHANNELS 3
extern chan_note_t channel_notes[AGI_NUM_CHANNELS];

void agi_sound_start(uint8_t* new_sound_data);
void agi_sound_stop();
void agi_sound_tick(int delta_ms);