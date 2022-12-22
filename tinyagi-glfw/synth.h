#pragma once

#include <stdint.h>

typedef struct {
	uint16_t frequency;
	float sample_step;
	float sample_pos;
} synth_channel_t;

void synth_set_sample(uint8_t* sample_data, size_t size);
void synth_set_frequency(uint8_t channel_no, uint16_t frequency);
void synth_get_sample_points(uint8_t* sample_buffer, size_t num_samples);