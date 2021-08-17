#pragma once

#include <stdint.h>
#include "vol.h"

#pragma pack(push,1)
typedef struct {
	uint16_t name_offset;
	uint8_t room_no;
} item_t;

typedef struct {
	uint16_t item_names_offset;
	uint8_t num_objects;
	item_t items[];
} item_file_t;

typedef struct {
	uint8_t hi_byte;
	uint8_t lo_byte;
} uint16_be_t;

typedef struct {
	uint8_t prefix;
	uint8_t data[];
} word_entry_t;

typedef struct {
	uint16_be_t word_indices[25];
	uint8_t data[];
} words_file_t;
#pragma pack(pop)

typedef struct {
	uint8_t script_type;
	uint8_t resource_number;
} script_entry_t;

typedef struct {
	vol_data_t loaded_logics[256];
	vol_data_t loaded_pics[256];
	vol_data_t loaded_views[256];
	vol_data_t loaded_sounds[256];
	script_entry_t* script_entries;
	int script_size;
	int script_entry_pos;

	item_file_t* item_file;
	words_file_t* words_file;
} agi_heap_t;

extern agi_heap_t heap_data;

void heap_reset();
void heap_write_script_entry(uint8_t script_type, uint8_t resource_no);
void heap_write_add_to_pic_script_entry(uint8_t view_no, uint8_t loop_no, uint8_t cel_no, uint8_t x, uint8_t y, uint8_t pri);
