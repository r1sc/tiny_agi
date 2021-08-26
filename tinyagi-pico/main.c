#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "pico/platform.h"

#include "vga/vga.h"
#include "audio/pwm_synth.h"
#include "agi_sound_player/agi_sound.h"

#include "archive.h"
#include "agi.h"

uint8_t screen_priority[160 * 168];

inline void __not_in_flash_func(screen_set_320)(int x, int y, int color)
{
    vga_pset(x, y, color);
}
inline void __not_in_flash_func(screen_set_160)(int x, int y, int color)
{
    y += state.play_top * 8;
    vga_pset_wide(x, y, color);
}

inline int __not_in_flash_func(priority_get)(int x, int y)
{
    return screen_priority[y * 160 + x];
}
inline void __not_in_flash_func(priority_set)(int x, int y, int priority)
{
    screen_priority[y * 160 + x] = priority;
}

void update_sound()
{
    if (state.sound_flag > -1 && !agi_sound_tick(1) && state.flags[state.sound_flag] == false)
    {
        state.flags[state.sound_flag] = true;
        state.sound_flag = -1;
        agi_stop_sound();
    }
}

void wait_for_enter()
{
    bool vsync_serviced = false;
    while (1)
    {
        if (!vsync_serviced && vga_current_timing_line > 480)
        {
            update_sound();
            vsync_serviced = true;
        }
        else if (vsync_serviced && vga_current_timing_line < 480)
        {
            vsync_serviced = false;
        }

        char c = getchar_timeout_us(0);
        if (c == 13)
        {
            state.enter_pressed = true;
            return;
        }
    }
}

agi_file_t get_file(const char *filename)
{
    const fileentry_t *file_entry = ar_find_file(filename);

    agi_file_t file;
    file.size = file_entry->length;
    file.data = ar_ptr_to(file_entry);
    return file;
}

void free_file(agi_file_t file)
{
}

void ext_play_sound(uint8_t *sound_data)
{
    agi_sound_start(sound_data);
}

void ext_stop_sound()
{
    agi_sound_stop();
}

// char *font_data;

unsigned char font_data[2048] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x42, 0x3C, 0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0x7E, 0x3C, 0x36, 0x7F, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00,
        0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x1C, 0x3E, 0x1C, 0x7F, 0x7F, 0x6B, 0x08, 0x1C, 0x08, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x08, 0x1C, 0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,
        0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, 0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00, 0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF, 0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,
        0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18, 0x08, 0x0C, 0x0E, 0x0A, 0x0A, 0x38, 0x78, 0x30, 0x00, 0x3E, 0x22, 0x22, 0x26, 0x6E, 0xE4, 0x40, 0x18, 0xDB, 0x3C, 0xE7, 0xE7, 0x3C, 0xDB, 0x18,
        0x20, 0x30, 0x38, 0x3C, 0x38, 0x30, 0x20, 0x00, 0x04, 0x0C, 0x1C, 0x3C, 0x1C, 0x0C, 0x04, 0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,
        0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00, 0x3C, 0x60, 0x3C, 0x66, 0x3C, 0x06, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x00, 0x00, 0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,
        0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00, 0x00, 0x0C, 0x06, 0x7F, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x18, 0x30, 0x7F, 0x30, 0x18, 0x00, 0x00,
        0x00, 0x00, 0x60, 0x60, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00,
        0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00, 0x60, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x06, 0x00, 0x38, 0x66, 0x66, 0x3F, 0x66, 0x66, 0x3B, 0x00, 0x1C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00,
        0x3C, 0x66, 0x6E, 0x7E, 0x76, 0x66, 0x3C, 0x00, 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x3C, 0x66, 0x06, 0x1C, 0x30, 0x66, 0x7E, 0x00, 0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00,
        0x0E, 0x1E, 0x36, 0x66, 0x7F, 0x06, 0x0F, 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x06, 0x66, 0x3C, 0x00, 0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00, 0x7E, 0x66, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00,
        0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30,
        0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00, 0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00,
        0x3C, 0x66, 0x6E, 0x6A, 0x6E, 0x60, 0x3C, 0x00, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00, 0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00,
        0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00, 0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00,
        0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x0E, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C, 0x00, 0x66, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x66, 0x00,
        0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00, 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00, 0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00,
        0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x0E, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00, 0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00,
        0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00,
        0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00, 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00, 0x3E, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x00,
        0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x3E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3E, 0x00, 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
        0x38, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x00,
        0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, 0x1E, 0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C,
        0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x30, 0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x00,
        0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x76, 0x7F, 0x6B, 0x6B, 0x63, 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00,
        0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x07, 0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00,
        0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x1C, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x63, 0x6B, 0x6B, 0x7F, 0x36, 0x00,
        0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C, 0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00, 0x1C, 0x30, 0x30, 0x60, 0x30, 0x30, 0x1C, 0x00,
        0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, 0x38, 0x0C, 0x0C, 0x06, 0x0C, 0x0C, 0x38, 0x00, 0x00, 0x00, 0x3B, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x36, 0x63, 0x63, 0x7F, 0x00,
        0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x60, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x0C, 0x18, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, 0x7E, 0xC3, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00,
        0x66, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, 0x30, 0x18, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, 0x18, 0x24, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x60,
        0x7E, 0xC3, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, 0x66, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, 0x30, 0x18, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, 0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00,
        0x3C, 0x66, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00, 0x30, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00, 0x66, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00, 0x18, 0x24, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00,
        0x0C, 0x18, 0x7E, 0x60, 0x7C, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x7E, 0x1B, 0x7F, 0xD8, 0x7E, 0x00, 0x1F, 0x3C, 0x6C, 0x7F, 0x6C, 0x6C, 0x6F, 0x00, 0x3C, 0x66, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00,
        0x00, 0x66, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x30, 0x18, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x3C, 0x66, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x30, 0x18, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x00,
        0x66, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x0C, 0x1E, 0x30, 0x30, 0x1E, 0x0C, 0x00,
        0x1C, 0x36, 0x30, 0x7C, 0x30, 0x60, 0x7E, 0x00, 0x66, 0x66, 0x3C, 0x7E, 0x18, 0x7E, 0x18, 0x00, 0x7C, 0x66, 0x6E, 0x7C, 0x6E, 0x6C, 0x66, 0x00, 0x0E, 0x18, 0x18, 0x3C, 0x18, 0x18, 0x70, 0x00,
        0x0C, 0x18, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, 0x0C, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00, 0x0C, 0x18, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x0C, 0x18, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x00,
        0x36, 0x6C, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x00, 0x36, 0x6C, 0x00, 0x66, 0x76, 0x6E, 0x66, 0x00, 0x38, 0x0C, 0x3C, 0x6C, 0x3C, 0x00, 0x00, 0x00, 0x38, 0x6C, 0x6C, 0x6C, 0x38, 0x00, 0x00, 0x00,
        0x18, 0x00, 0x18, 0x30, 0x60, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x06, 0x06, 0x00, 0x00, 0x40, 0x42, 0x44, 0x48, 0x16, 0x23, 0x46, 0x07,
        0x40, 0x42, 0x44, 0x48, 0x15, 0x25, 0x47, 0x01, 0x18, 0x00, 0x18, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00, 0x36, 0x6C, 0xD8, 0x6C, 0x36, 0x00, 0x00, 0x00, 0x6C, 0x36, 0x1B, 0x36, 0x6C, 0x00, 0x00,
        0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0xBB, 0xEE, 0xBB, 0xEE, 0xBB, 0xEE, 0xBB, 0xEE, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
        0x18, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x66, 0x66, 0x66, 0xE6, 0xE6, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x66, 0x66, 0x66,
        0x00, 0xF8, 0xF8, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x66, 0xE6, 0xE6, 0x06, 0x06, 0xE6, 0xE6, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0xFE, 0xFE, 0x06, 0x06, 0xE6, 0xE6, 0x66,
        0x66, 0xE6, 0xE6, 0x06, 0x06, 0xFE, 0xFE, 0x00, 0x66, 0x66, 0x66, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x10, 0xF8, 0xF8, 0x18, 0x18, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10,
        0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x18,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x66, 0x66, 0x66, 0x67, 0x67, 0x66, 0x66, 0x66,
        0x66, 0x67, 0x67, 0x60, 0x60, 0x7F, 0x7F, 0x00, 0x00, 0x7F, 0x7F, 0x60, 0x60, 0x67, 0x67, 0x66, 0x66, 0xE7, 0xE7, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xE7, 0xE7, 0x66,
        0x66, 0x67, 0x67, 0x60, 0x60, 0x67, 0x67, 0x66, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x66, 0xE7, 0xE7, 0x00, 0x00, 0xE7, 0xE7, 0x66, 0x18, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00,
        0x66, 0x66, 0x66, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x18, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7F, 0x7F, 0x00, 0x00, 0x00,
        0x18, 0x1F, 0x1F, 0x18, 0x18, 0x1F, 0x1F, 0x00, 0x00, 0x1F, 0x1F, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xE7, 0xE7, 0x66, 0x66, 0x66,
        0x18, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x18, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x3B, 0x6E, 0x64, 0x6E, 0x3B, 0x00, 0x3C, 0x66, 0x66, 0x6C, 0x66, 0x63, 0x6E, 0x60, 0x7E, 0x66, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x7F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00,
        0x7E, 0x60, 0x30, 0x18, 0x30, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x3E, 0x78, 0x6C, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x66, 0x66, 0x6E, 0x7B, 0x60, 0x60, 0x00, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x0C, 0x00,
        0x3C, 0x18, 0x3C, 0x66, 0x66, 0x3C, 0x18, 0x3C, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x3C, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x36, 0x77, 0x00, 0x3C, 0x30, 0x18, 0x3C, 0x66, 0x66, 0x3C, 0x00,
        0x00, 0x00, 0x36, 0x6F, 0x6B, 0x7B, 0x36, 0x00, 0x00, 0x03, 0x3E, 0x66, 0x7E, 0x66, 0x7C, 0xC0, 0x00, 0x00, 0x3E, 0x60, 0x3C, 0x60, 0x3E, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00,
        0x00, 0x7E, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x7E, 0x00, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x00, 0x7E, 0x00, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x00, 0x7E, 0x00,
        0x0E, 0x1B, 0x1B, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xD8, 0xD8, 0x70, 0x00, 0x18, 0x00, 0x7E, 0x00, 0x18, 0x00, 0x00, 0x00, 0x3B, 0x6E, 0x00, 0x3B, 0x6E, 0x00, 0x00,
        0x1C, 0x36, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x0F, 0x0C, 0x0C, 0xCC, 0x6C, 0x3C, 0x1C, 0x0C,
        0x3C, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x38, 0x0C, 0x18, 0x30, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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

void agi_play_sound(uint8_t *sound_data)
{
    agi_sound_start(sound_data);
}

void agi_stop_sound()
{
    agi_sound_stop();
}

#define FLAG_VALUE 123

void get_char(uint8_t *ascii, uint8_t *scancode)
{
    char buf[7];
    buf[6] = '/0';
    int len = 0;
    char c;
    while ((c = getchar_timeout_us(0)) != 0xFF)
    {
        buf[len++] = c;
    }
    
    *ascii = 0;
    *scancode = 0;
    if (len > 0)
    {
        if (buf[0] == 27)
        {
            printf("Escape seq (%d): ", len);
            for (size_t i = 0; i < len; i++)
            {
                printf("%c (%d) ", buf[i] == 27 ? 'E' : buf[i], buf[i]);
            }
            printf("\n");
            
            if(len == 1) {
                *ascii = 27;
                return;
            } 
            else if(len == 3 && buf[1] == '[') {
                switch(buf[2]){
                    case 'A': *scancode = AGI_KEY_UP; break;
                    case 'B': *scancode = AGI_KEY_DOWN; break;
                    case 'C': *scancode = AGI_KEY_RIGHT; break;
                    case 'D': *scancode = AGI_KEY_LEFT; break;
                }
            } else if(len == 3 && buf[1] == 'O') {
                switch(buf[2]){
                    case 'P': *scancode = AGI_KEY_F1; break;
                    case 'Q': *scancode = AGI_KEY_F2; break;
                    case 'R': *scancode = AGI_KEY_F3; break;
                    case 'S': *scancode = AGI_KEY_F4; break;
                    case 'T': *scancode = AGI_KEY_F5; break;
                    case 'U': *scancode = AGI_KEY_F6; break;
                    case 'V': *scancode = AGI_KEY_F7; break;
                    case 'W': *scancode = AGI_KEY_F8; break;
                    case 'X': *scancode = AGI_KEY_F9; break;
                    case 'Y': *scancode = AGI_KEY_F10; break;
                }
            } else if(len == 4 && buf[1] == '[') {
                switch(buf[2]){
                    case '1': *scancode = AGI_KEY_HOME; break;
                    case '5': *scancode = AGI_KEY_PGUP; break;
                    case '6': *scancode = AGI_KEY_PGDN; break;
                    case '4': case '8': *scancode = AGI_KEY_END; break;
                }
            }
        } 
        else if(len == 1) {
            *ascii = buf[0];
        }
    }
}

int main()
{
    stdio_init_all();

    pwm_synth_init(18);
    vga_init(2, 6);

    gpio_init(20);
    gpio_set_dir(20, false);

    agi_initialize();

    bool vsync_serviced = false;

    uint32_t last = time_us_32();

    while (1)
    {
        uint32_t now = time_us_32();
        uint32_t delta_us = now - last;
        uint32_t delta_ms = delta_us / 1000;

        if (delta_ms >= 50)
        {
            last = now;
            uint8_t ascii, scancode;
            get_char(&ascii, &scancode);
            if(ascii != 0 || scancode != 0) {
                if(ascii == 13){
                    state.enter_pressed = true;
                } else {
                    agi_input_queue_push_keypress(ascii, scancode);
                }
            }
        }

        uint32_t now_ms = now / 1000;
        agi_logic_run_cycle(now_ms);

        if (!vsync_serviced && vga_current_timing_line > 480)
        {
            agi_draw_all_active();
            update_sound();
            vsync_serviced = true;
        }
        else if (vsync_serviced && vga_current_timing_line < 480)
        {
            vsync_serviced = false;
        }
    }

    return 0;
}
