#pragma once

#include <stdint.h>
#include <stddef.h>

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

extern void check_key();
extern void wait_for_enter();

extern void agi_shake_screen(uint8_t times);

extern void agi_play_sound(uint8_t* sound_data);
extern void agi_stop_sound();

extern uint8_t font_data[];

typedef void* agi_save_data_file_ptr;
agi_save_data_file_ptr agi_save_data_open(const char* mode);
void agi_save_data_write(agi_save_data_file_ptr file_ptr, void* data, size_t size);
void agi_save_data_read(agi_save_data_file_ptr file_ptr, void* destination, size_t size);
void agi_save_data_close(agi_save_data_file_ptr file_ptr);