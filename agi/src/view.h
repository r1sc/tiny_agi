#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "state.h"

#pragma pack(push,1)
typedef struct {
	uint8_t width;
	uint8_t height;
	uint8_t transparent_color_and_mirroring;
	uint8_t pixel_data[];
} cell_t;

typedef struct {
	uint8_t num_cells;
	uint16_t cell_offsets[];
} loop_t;

typedef struct {
	uint16_t unk;
	uint8_t num_loops;
	uint16_t description_offset;
	int16_t loop_offsets[];
} view_t;
#pragma pack(pop)

uint8_t _view_num_loops(uint8_t viewNo);
uint8_t _view_num_cels(uint8_t viewNo, uint8_t loopNo);
cell_t* _get_cell(uint8_t viewNo, uint8_t loopNo, uint8_t cellNo);
cell_t* _object_cell(const object_t* obj);
void _draw_view(uint8_t viewNo, uint8_t loopNo, uint8_t cellNo, uint8_t x, uint8_t y, uint8_t priority, bool erase, bool addToPic);