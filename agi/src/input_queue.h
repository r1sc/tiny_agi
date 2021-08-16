#pragma once

#include <stdint.h>

typedef enum {
    AGI_KEY_MODIFIER_NONE = 0,
    AGI_KEY_MODIFIER_SHIFT,
    AGI_KEY_MODIFIER_CTRL,
    AGI_KEY_MODIFIER_ALT
} agi_key_modifier_e;

typedef struct {
    uint8_t scancode;
    char ascii;
} input_queue_entry_t;

#define QUEUE_LEN 10
extern input_queue_entry_t queue[QUEUE_LEN];
extern int queue_pos;