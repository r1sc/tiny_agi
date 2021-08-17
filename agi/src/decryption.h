#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct
{
	uint8_t num_messages;
	uint16_t messages_end;
} agi_messages_header_t;
#pragma pack(pop)


void decrypt_item_file(uint8_t* item_file, size_t size);
void decrypt_messages(uint8_t logic_no);