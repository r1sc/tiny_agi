#pragma once

#include <stdint.h>

extern void panic(const char* fmt, ...);

typedef struct {
	uint8_t* data;
	size_t size;
} agi_file_t;


extern agi_file_t get_file(const char* filename);
extern void free_file(agi_file_t file);

extern void screen_set_160(int x, int y, int color);
extern void screen_set_320(int x, int y, int color);
extern int priority_get(int x, int y);
extern void priority_set(int x, int y, int priority);


extern void wait_for_enter();

extern void agi_ext_sound_update(uint16_t channel_hz[4]);