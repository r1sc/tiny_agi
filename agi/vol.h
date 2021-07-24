#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	void* buffer;
	uint16_t size;
} vol_data_t;

vol_data_t load_vol_data(const char* dir_filename, const uint8_t res_no, bool is_logic);