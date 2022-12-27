#pragma once

#include <stdint.h>

// Returns NULL if all buffers are full
int16_t* waveout_get_current_buffer();
void waveout_queue_buffer();

void waveout_initialize(unsigned int sample_rate, unsigned buffer_size);
void waveout_free();