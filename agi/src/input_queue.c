#include "input_queue.h"

input_queue_entry_t queue[QUEUE_LEN];
int queue_pos = 0;

void agi_input_queue_push_keypress(char ascii, uint8_t scancode) {
    queue[queue_pos].ascii = ascii;
    queue[queue_pos].scancode = scancode;
    queue_pos++;
}