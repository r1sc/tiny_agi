#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    char* name;
    uint32_t data_offset;
    size_t length;
} fileentry_t;

const fileentry_t files[] = {
{"font.bin", 0, 2048},
{"logdir", 2048, 426},
{"object", 2474, 331},
{"picdir", 2805, 444},
{"snddir", 3249, 210},
{"viewdir", 3459, 720},
{"vol.0", 4179, 70654},
{"vol.1", 74833, 161225},
{"vol.2", 236058, 411824},
{"words.tok", 647882, 6828},
};

uint8_t* ARCHIVE_START = (uint8_t*)0x10100000;

const fileentry_t* ar_find_file(const char* filename) {
    for (size_t i = 0; i < 10; i++)
    {
        if(strcmp(filename, files[i].name) == 0){
            return &files[i];
        }
    }
    return NULL;
}

uint8_t* ar_ptr_to(const fileentry_t* entry) {
    return (uint8_t*)(ARCHIVE_START + entry->data_offset);
}
