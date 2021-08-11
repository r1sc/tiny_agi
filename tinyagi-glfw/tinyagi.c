﻿#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include <math.h>

#include <GLFW/glfw3.h>
#include <gl/GL.h>

#include "agi.h"
#include "winmm_audio.h"
#include "synth.h"

uint8_t screen_priority[160 * 168];
uint32_t framebuffer[320 * 200];

void panic(const char* fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	char buffer[256];
	vsprintf(buffer, fmt, vl);
	printf("Panic! %s\n", buffer);
	exit(1);
}

const char* game_path = "C:\\classic\\sierra\\pq";
agi_file_t get_file(const char* filename) {
	char path[256];
	sprintf(path, "%s\\%s\0", game_path, filename);


	agi_file_t result;
	FILE* f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	result.size = ftell(f);
	result.data = (uint8_t*)malloc(result.size);
	if (result.data) {
		fseek(f, 0, SEEK_SET);
		fread(result.data, 1, result.size, f);
	}
	else {
		panic("Failed to allocate memory for file %s, of size %d", path, result.size);
	}

	fclose(f);
	return result;
}

void free_file(agi_file_t file) {
	free(file.data);
}

const unsigned int palette[16] = {
	0xFF000000,
	0xFF0000AA,
	0xFF00AA00,
	0xFF00AAAA,
	0xFFAA0000,
	0xFFAA00AA,
	0xFFAA5500,
	0xFFAAAAAA,
	0xFF555555,
	0xFF5555FF,
	0xFF55FF55,
	0xFF55FFFF,
	0xFFFF5555,
	0xFFFF55FF,
	0xFFFFFF55,
	0xFFFFFFFF
};

inline void screen_set_160(int x, int y, int color) {
	int x2 = x << 1;
	y += state.play_top * 8;
	framebuffer[y * 320 + x2 + 0] = palette[color];
	framebuffer[y * 320 + x2 + 1] = palette[color];
}

inline void screen_set_320(int x, int y, int color) {
	framebuffer[y * 320 + x] = palette[color];
}

inline void priority_set(int x, int y, int priority) {
	screen_priority[y * 160 + x] = priority;
}

inline int priority_get(int x, int y) {
	return screen_priority[y * 160 + x];
}

const char font_data[] =
{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E, 0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E, 0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x0, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x0, 0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C, 0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C, 0x0, 0x0, 0x18, 0x3C, 0x3C, 0x18, 0x0, 0x0, 0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, 0x0, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x0, 0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF, 0xF, 0x7, 0xF, 0x7D, 0xCC, 0xCC, 0xCC, 0x78, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18, 0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0, 0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0, 0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99, 0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x0, 0x2, 0xE, 0x3E, 0xFE, 0x3E, 0xE, 0x2, 0x0, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x66, 0x66, 0x66, 0x66, 0x66, 0x0, 0x66, 0x0, 0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x0, 0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78, 0x0, 0x0, 0x0, 0x0, 0x7E, 0x7E, 0x7E, 0x0, 0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x0, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x0, 0x0, 0x18, 0xC, 0xFE, 0xC, 0x18, 0x0, 0x0, 0x0, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0xC0, 0xFE, 0x0, 0x0, 0x0, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x0, 0x0, 0x0, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x78, 0x78, 0x30, 0x30, 0x0, 0x30, 0x0, 0x6C, 0x6C, 0x6C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x0, 0x30, 0x7C, 0xC0, 0x78, 0xC, 0xF8, 0x30, 0x0, 0x0, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x0, 0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x0, 0x60, 0x60, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x0, 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x0, 0x0, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x0, 0x0, 0x0, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x30, 0x60, 0x0, 0x0, 0x0, 0xFC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x30, 0x0, 0x6, 0xC, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x0, 0x7C, 0xC6, 0xCE, 0xDE, 0xF6, 0xE6, 0x7C, 0x0, 0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x0, 0x78, 0xCC, 0xC, 0x38, 0x60, 0xCC, 0xFC, 0x0, 0x78, 0xCC, 0xC, 0x38, 0xC, 0xCC, 0x78, 0x0, 0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0xC, 0x1E, 0x0, 0xFC, 0xC0, 0xF8, 0xC, 0xC, 0xCC, 0x78, 0x0, 0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x0, 0xFC, 0xCC, 0xC, 0x18, 0x30, 0x30, 0x30, 0x0, 0x78, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0x78, 0x0, 0x78, 0xCC, 0xCC, 0x7C, 0xC, 0x18, 0x70, 0x0, 0x0, 0x30, 0x30, 0x0, 0x0, 0x30, 0x30, 0x0, 0x0, 0x30, 0x30, 0x0, 0x0, 0x30, 0x30, 0x60, 0x18, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x18, 0x0, 0x0, 0x0, 0xFC, 0x0, 0x0, 0xFC, 0x0, 0x0, 0x60, 0x30, 0x18, 0xC, 0x18, 0x30, 0x60, 0x0, 0x78, 0xCC, 0xC, 0x18, 0x30, 0x0, 0x30, 0x0, 0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x78, 0x0, 0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x0, 0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x0, 0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x0, 0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x0, 0xFE, 0x62, 0x68, 0x78, 0x68, 0x62, 0xFE, 0x0, 0xFE, 0x62, 0x68, 0x78, 0x68, 0x60, 0xF0, 0x0, 0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3E, 0x0, 0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x0, 0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x0, 0x1E, 0xC, 0xC, 0xC, 0xCC, 0xCC, 0x78, 0x0, 0xE6, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0xE6, 0x0, 0xF0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xFE, 0x0, 0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x0, 0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x0, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x0, 0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x0, 0x78, 0xCC, 0xCC, 0xCC, 0xDC, 0x78, 0x1C, 0x0, 0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x0, 0x78, 0xCC, 0xE0, 0x70, 0x1C, 0xCC, 0x78, 0x0, 0xFC, 0xB4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x0, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0x0, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x0, 0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x0, 0xC6, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x0, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x78, 0x0, 0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xFE, 0x0, 0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x0, 0xC0, 0x60, 0x30, 0x18, 0xC, 0x6, 0x2, 0x0, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x0, 0x10, 0x38, 0x6C, 0xC6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x30, 0x30, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x78, 0xC, 0x7C, 0xCC, 0x76, 0x0, 0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0xDC, 0x0, 0x0, 0x0, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x0, 0x1C, 0xC, 0xC, 0x7C, 0xCC, 0xCC, 0x76, 0x0, 0x0, 0x0, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x0, 0x38, 0x6C, 0x60, 0xF0, 0x60, 0x60, 0xF0, 0x0, 0x0, 0x0, 0x76, 0xCC, 0xCC, 0x7C, 0xC, 0xF8, 0xE0, 0x60, 0x6C, 0x76, 0x66, 0x66, 0xE6, 0x0, 0x30, 0x0, 0x70, 0x30, 0x30, 0x30, 0x78, 0x0, 0xC, 0x0, 0xC, 0xC, 0xC, 0xCC, 0xCC, 0x78, 0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x0, 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x0, 0x0, 0x0, 0xCC, 0xFE, 0xFE, 0xD6, 0xC6, 0x0, 0x0, 0x0, 0xF8, 0xCC, 0xCC, 0xCC, 0xCC, 0x0, 0x0, 0x0, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x0, 0x0, 0x0, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0, 0x0, 0x0, 0x76, 0xCC, 0xCC, 0x7C, 0xC, 0x1E, 0x0, 0x0, 0xDC, 0x76, 0x66, 0x60, 0xF0, 0x0, 0x0, 0x0, 0x7C, 0xC0, 0x78, 0xC, 0xF8, 0x0, 0x10, 0x30, 0x7C, 0x30, 0x30, 0x34, 0x18, 0x0, 0x0, 0x0, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x0, 0x0, 0x0, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x0, 0x0, 0x0, 0xC6, 0xD6, 0xFE, 0xFE, 0x6C, 0x0, 0x0, 0x0, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x0, 0x0, 0x0, 0xCC, 0xCC, 0xCC, 0x7C, 0xC, 0xF8, 0x0, 0x0, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x0, 0x1C, 0x30, 0x30, 0xE0, 0x30, 0x30, 0x1C, 0x0, 0x18, 0x18, 0x18, 0x0, 0x18, 0x18, 0x18, 0x0, 0xE0, 0x30, 0x30, 0x1C, 0x30, 0x30, 0xE0, 0x0, 0x76, 0xDC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x0, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x18, 0xC, 0x78, 0x0, 0xCC, 0x0, 0xCC, 0xCC, 0xCC, 0x7E, 0x0, 0x1C, 0x0, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x0, 0x7E, 0xC3, 0x3C, 0x6, 0x3E, 0x66, 0x3F, 0x0, 0xCC, 0x0, 0x78, 0xC, 0x7C, 0xCC, 0x7E, 0x0, 0xE0, 0x0, 0x78, 0xC, 0x7C, 0xCC, 0x7E, 0x0, 0x30, 0x30, 0x78, 0xC, 0x7C, 0xCC, 0x7E, 0x0, 0x0, 0x0, 0x78, 0xC0, 0xC0, 0x78, 0xC, 0x38, 0x7E, 0xC3, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x0, 0xCC, 0x0, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x0, 0xE0, 0x0, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x0, 0xCC, 0x0, 0x70, 0x30, 0x30, 0x30, 0x78, 0x0, 0x7C, 0xC6, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x0, 0xE0, 0x0, 0x70, 0x30, 0x30, 0x30, 0x78, 0x0, 0xC6, 0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0xC6, 0x0, 0x30, 0x30, 0x0, 0x78, 0xCC, 0xFC, 0xCC, 0x0, 0x1C, 0x0, 0xFC, 0x60, 0x78, 0x60, 0xFC, 0x0, 0x0, 0x0, 0x7F, 0xC, 0x7F, 0xCC, 0x7F, 0x0, 0x3E, 0x6C, 0xCC, 0xFE, 0xCC, 0xCC, 0xCE, 0x0, 0x78, 0xCC, 0x0, 0x78, 0xCC, 0xCC, 0x78, 0x0, 0x0, 0xCC, 0x0, 0x78, 0xCC, 0xCC, 0x78, 0x0, 0x0, 0xE0, 0x0, 0x78, 0xCC, 0xCC, 0x78, 0x0, 0x78, 0xCC, 0x0, 0xCC, 0xCC, 0xCC, 0x7E, 0x0, 0x0, 0xE0, 0x0, 0xCC, 0xCC, 0xCC, 0x7E, 0x0, 0x0, 0xCC, 0x0, 0xCC, 0xCC, 0x7C, 0xC, 0xF8, 0xC3, 0x18, 0x3C, 0x66, 0x66, 0x3C, 0x18, 0x0, 0xCC, 0x0, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x0, 0x18, 0x18, 0x7E, 0xC0, 0xC0, 0x7E, 0x18, 0x18, 0x38, 0x6C, 0x64, 0xF0, 0x60, 0xE6, 0xFC, 0x0, 0xCC, 0xCC, 0x78, 0xFC, 0x30, 0xFC, 0x30, 0x30, 0xF8, 0xCC, 0xCC, 0xFA, 0xC6, 0xCF, 0xC6, 0xC7, 0xE, 0x1B, 0x18, 0x3C, 0x18, 0x18, 0xD8, 0x70, 0x1C, 0x0, 0x78, 0xC, 0x7C, 0xCC, 0x7E, 0x0, 0x38, 0x0, 0x70, 0x30, 0x30, 0x30, 0x78, 0x0, 0x0, 0x1C, 0x0, 0x78, 0xCC, 0xCC, 0x78, 0x0, 0x0, 0x1C, 0x0, 0xCC, 0xCC, 0xCC, 0x7E, 0x0, 0x0, 0xF8, 0x0, 0xF8, 0xCC, 0xCC, 0xCC, 0x0, 0xFC, 0x0, 0xCC, 0xEC, 0xFC, 0xDC, 0xCC, 0x0, 0x3C, 0x6C, 0x6C, 0x3E, 0x0, 0x7E, 0x0, 0x0, 0x38, 0x6C, 0x6C, 0x38, 0x0, 0x7C, 0x0, 0x0, 0x30, 0x0, 0x30, 0x60, 0xC0, 0xCC, 0x78, 0x0, 0x0, 0x0, 0x0, 0xFC, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFC, 0xC, 0xC, 0x0, 0x0, 0xC3, 0xC6, 0xCC, 0xDE, 0x33, 0x66, 0xCC, 0xF, 0xC3, 0xC6, 0xCC, 0xDB, 0x37, 0x6F, 0xCF, 0x3, 0x18, 0x18, 0x0, 0x18, 0x18, 0x18, 0x18, 0x0, 0x0, 0x33, 0x66, 0xCC, 0x66, 0x33, 0x0, 0x0, 0x0, 0xCC, 0x66, 0x33, 0x66, 0xCC, 0x0, 0x0, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0xDB, 0x77, 0xDB, 0xEE, 0xDB, 0x77, 0xDB, 0xEE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x36, 0x36, 0x36, 0x0, 0x0, 0x0, 0x0, 0xFE, 0x36, 0x36, 0x36, 0x0, 0x0, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x36, 0x36, 0xF6, 0x6, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x0, 0x0, 0xFE, 0x6, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x6, 0xFE, 0x0, 0x0, 0x0, 0x36, 0x36, 0x36, 0x36, 0xFE, 0x0, 0x0, 0x0, 0x18, 0x18, 0xF8, 0x18, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x0, 0x0, 0x0, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x3F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3F, 0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0xF7, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xF7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x36, 0x36, 0xF7, 0x0, 0xF7, 0x36, 0x36, 0x36, 0x18, 0x18, 0xFF, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x36, 0x36, 0x36, 0x36, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0xFF, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x3F, 0x0, 0x0, 0x0, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x3F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xFF, 0x36, 0x36, 0x36, 0x18, 0x18, 0xFF, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1F, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x76, 0xDC, 0xC8, 0xDC, 0x76, 0x0, 0x0, 0x78, 0xCC, 0xF8, 0xCC, 0xF8, 0xC0, 0xC0, 0x0, 0xFC, 0xCC, 0xC0, 0xC0, 0xC0, 0xC0, 0x0, 0x0, 0xFE, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x0, 0xFC, 0xCC, 0x60, 0x30, 0x60, 0xCC, 0xFC, 0x0, 0x0, 0x0, 0x7E, 0xD8, 0xD8, 0xD8, 0x70, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0xC0, 0x0, 0x76, 0xDC, 0x18, 0x18, 0x18, 0x18, 0x0, 0xFC, 0x30, 0x78, 0xCC, 0xCC, 0x78, 0x30, 0xFC, 0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0x6C, 0x38, 0x0, 0x38, 0x6C, 0xC6, 0xC6, 0x6C, 0x6C, 0xEE, 0x0, 0x1C, 0x30, 0x18, 0x7C, 0xCC, 0xCC, 0x78, 0x0, 0x0, 0x0, 0x7E, 0xDB, 0xDB, 0x7E, 0x0, 0x0, 0x6, 0xC, 0x7E, 0xDB, 0xDB, 0x7E, 0x60, 0xC0, 0x38, 0x60, 0xC0, 0xF8, 0xC0, 0x60, 0x38, 0x0, 0x78, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x0, 0x0, 0xFC, 0x0, 0xFC, 0x0, 0xFC, 0x0, 0x0, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x0, 0xFC, 0x0, 0x60, 0x30, 0x18, 0x30, 0x60, 0x0, 0xFC, 0x0, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0, 0xFC, 0x0, 0xE, 0x1B, 0x1B, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xD8, 0xD8, 0x70, 0x30, 0x30, 0x0, 0xFC, 0x0, 0x30, 0x30, 0x0, 0x0, 0x76, 0xDC, 0x0, 0x76, 0xDC, 0x0, 0x0, 0x38, 0x6C, 0x6C, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x0, 0x0, 0x0, 0xF, 0xC, 0xC, 0xC, 0xEC, 0x6C, 0x3C, 0x1C, 0x78, 0x6C, 0x6C, 0x6C, 0x6C, 0x0, 0x0, 0x0, 0x70, 0x18, 0x30, 0x60, 0x78, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3C, 0x3C, 0x3C, 0x3C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

void _draw_char(unsigned int start_x, unsigned int start_y, unsigned char c, uint8_t fg, uint8_t bg)
{
	if (c == '\n')
		return;

	for (unsigned int y = 0; y < 8; y++)
	{
		unsigned char rowData = font_data[c * 8 + y];
		for (unsigned int x = 0; x < 8; x++)
		{
			bool on = (rowData & 0x80) == 0x80;
			screen_set_320(x + start_x, y + start_y, on ? fg : bg);
			rowData = rowData << 1;
		}
	}
}

void window_resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

bool show_priority = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) && action == GLFW_PRESS)
		state.enter_pressed = true;

	if (!state.program_control) {
		if ((key == GLFW_KEY_UP || key == GLFW_KEY_KP_8) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP ? DIR_STOPPED : DIR_UP;
		else if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_KP_2) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN ? DIR_STOPPED : DIR_DOWN;
		else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_KP_4) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_LEFT ? DIR_STOPPED : DIR_LEFT;
		else if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_KP_6) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_RIGHT ? DIR_STOPPED : DIR_RIGHT;
		else if (key == GLFW_KEY_KP_7 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_LEFT ? DIR_STOPPED : DIR_UP_LEFT;
		else if (key == GLFW_KEY_KP_9 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_RIGHT ? DIR_STOPPED : DIR_UP_RIGHT;
		else if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_LEFT ? DIR_STOPPED : DIR_DOWN_LEFT;
		else if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_RIGHT ? DIR_STOPPED : DIR_DOWN_RIGHT;
		else if (key == GLFW_KEY_KP_5 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = DIR_STOPPED;
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		show_priority = !show_priority;
	}

	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		agi_push_char('\b');
	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	char c = (char)codepoint;
	agi_push_char(c);
}

void agi_stop_sound(){}
void agi_play_sound(uint8_t* sound_data) {}

GLFWwindow* window;

void render() {
	if (show_priority) {
		for (size_t y = 0; y < 168; y++)
		{
			for (size_t x = 0; x < 160; x++)
			{	
				screen_set_160(x, y, priority_get(x, y));
				/*unsigned int c = palette[priority_get(x, y)];

				framebuffer[(y + state.play_top * 8) * 320 + (x << 1) + 0] = c;
				framebuffer[(y + state.play_top * 8) * 320 + (x << 1) + 1] = c;*/
			}
		}
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_BGRA_EXT, GL_UNSIGNED_BYTE, framebuffer);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0);	glVertex2f(320, 0);
	glTexCoord2f(1, 1);	glVertex2f(320, 200);
	glTexCoord2f(0, 1);	glVertex2f(0, 200);
	glEnd();

	/* Swap front and back buffers */
	glfwSwapBuffers(window);
}

void wait_for_enter() {
	render();

	state.enter_pressed = false;
	while (!state.enter_pressed) {
		Sleep(10);
		glfwPollEvents();
	}
}

void agi_ext_sound_update(uint16_t channel_hz[4]) {
	/*for (size_t i = 0; i < 4; i++)
	{
		synth_set_frequency(i, channel_hz[i]);
	}
	synth_get_sample_points()*/
}

#define PI 3.14159265358979323846
int main() {

	// Init audio
	/*winmm_audio_init();
	
	uint8_t sample[64];
	for (size_t i = 0; i < 64; i++)
	{
		sample[i] = (uint8_t)(sin((i / 64.0f) * PI * 2) * 255);
	}
	synth_set_sample(sample, 64);*/


	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 600, "TinyAGI", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, window_resize);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);

	glEnable(GL_TEXTURE_2D);

	agi_reset();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 320, 200, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 200, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, framebuffer);

	int clock_counter = 0;
	double last_s = 0;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double delta = now - last_s;

		if (delta >= (((double)state.variables[10]) * 0.05)) {
			last_s = now;

			clock_counter++;
			if (clock_counter > 20) {
				state.variables[VAR_11_CLOCK_SECONDS]++;
				if (state.variables[VAR_11_CLOCK_SECONDS] == 60) {
					state.variables[VAR_11_CLOCK_SECONDS] = 0;
					state.variables[VAR_12_CLOCK_MINUTES]++;
					if (state.variables[VAR_12_CLOCK_MINUTES] == 60) {
						state.variables[VAR_12_CLOCK_MINUTES] = 0;
						state.variables[VAR_13_CLOCK_HOURS]++;
						if (state.variables[VAR_13_CLOCK_HOURS] == 24) {
							state.variables[VAR_13_CLOCK_HOURS] = 0;
							state.variables[VAR_14_CLOCK_DAYS]++;
						}
					}
				}
				clock_counter = 0;
			}

			//_undraw_all();
			agi_logic_run_cycle();
			_draw_all_active();
			render();
			state.enter_pressed = false;
		}

		/* Poll for and process events */
		glfwPollEvents();

		Sleep(1);
	}

	glfwTerminate();
	return 0;
}
