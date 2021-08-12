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
{"logdir", 2048, 390},
{"object", 2438, 778},
{"picdir", 3216, 531},
{"snddir", 3747, 120},
{"viewdir", 3867, 756},
{"vol.0", 4623, 78782},
{"vol.1", 83405, 365256},
{"vol.2", 448661, 206032},
{"vol.3", 654693, 13},
{"words.tok", 654706, 5670},
};

uint8_t* ARCHIVE_START = (uint8_t*)0x10100000;

const fileentry_t* ar_find_file(const char* filename) {
    for (size_t i = 0; i < 11; i++)
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
