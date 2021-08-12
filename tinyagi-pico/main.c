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

void update_sound() {
    if(state.sound_flag > -1 && !agi_sound_tick(1) && state.flags[state.sound_flag] == false){
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

char *font_data;
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

int main()
{
    stdio_init_all();

    pwm_synth_init(18);
    vga_init(2, 6);

    gpio_init(20);
    gpio_set_dir(20, false);
    
    printf("AGI Machine ready! Let's wait two seconds for the monitor to settle...\n");
    busy_wait_ms(2000);

    agi_reset();

    const fileentry_t *fontEntry = ar_find_file("font.bin");
    font_data = (uint8_t *)ar_ptr_to(fontEntry);

    bool vsync_serviced = false;

    uint32_t last = time_us_32();
    uint32_t clock_last = time_us_32();

    while (1)
    {
        uint32_t now = time_us_32();
        uint32_t delta_us = now - last;
        uint32_t delta_ms = delta_us / 1000;

        uint32_t delta_clock_us = now - clock_last;
        if(delta_clock_us >= 1000000) {
            clock_last = now;

            state.variables[VAR_11_CLOCK_SECONDS]++;
            if (state.variables[VAR_11_CLOCK_SECONDS] == 60)
            {
                state.variables[VAR_11_CLOCK_SECONDS] = 0;
                state.variables[VAR_12_CLOCK_MINUTES]++;
                if (state.variables[VAR_12_CLOCK_MINUTES] == 60)
                {
                    state.variables[VAR_12_CLOCK_MINUTES] = 0;
                    state.variables[VAR_13_CLOCK_HOURS]++;
                    if (state.variables[VAR_13_CLOCK_HOURS] == 24)
                    {
                        state.variables[VAR_13_CLOCK_HOURS] = 0;
                        state.variables[VAR_14_CLOCK_DAYS]++;
                    }
                }
            }
        }

        if(delta_ms >= 50 * state.variables[VAR_10_INTERPRETER_CYCLE_TIME]) {
            last = now;
            char c = getchar_timeout_us(0);
            if (c != 0xFF)
            {
                if (!state.program_control)
                {
                    if (c == 65)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP ? DIR_STOPPED : DIR_UP;
                    else if (c == 66)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] ==  DIR_DOWN ? DIR_STOPPED : DIR_DOWN;
                    else if (c == 68)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_LEFT ? DIR_STOPPED : DIR_LEFT;
                    else if (c == 67)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_RIGHT ? DIR_STOPPED : DIR_RIGHT;
                    else if (c == 49)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_LEFT ? DIR_STOPPED : DIR_UP_LEFT;
                    else if (c == 53)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_RIGHT ? DIR_STOPPED : DIR_UP_RIGHT;
                    else if (c == 52)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_LEFT ? DIR_STOPPED : DIR_DOWN_LEFT;
                    else if (c == 54)
                        state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_RIGHT ? DIR_STOPPED : DIR_DOWN_RIGHT;
                    else if (c >= 32 && c <= 122 && c != 91)
                    {
                        agi_push_char(c);
                    }
                    else if (c == '\b')
                    {
                        agi_push_char('\b');
                    }
                }

                if (c == 13)
                {
                    state.enter_pressed = true;
                }
            }

            agi_logic_run_cycle();
            state.enter_pressed = false;
        }

        if (!vsync_serviced && vga_current_timing_line > 480)
        {            
            _draw_all_active();
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
