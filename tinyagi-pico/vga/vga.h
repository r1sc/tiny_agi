#pragma once

#include <stdint.h>

#define VGA_FB_SIZE 160*240

extern uint8_t vga_framebuffer[VGA_FB_SIZE];
extern int vga_current_timing_line;

void vga_init(int sync_pins_base, int rgb_pins_base);
void vga_pset(uint16_t x, uint16_t y, uint8_t c);
void vga_pset_wide(uint8_t x, uint8_t y, uint8_t c);