#include "decryption.h"
#include "heap.h"

const char* decryptionKey = "Avis Durgan";

void decrypt_item_file(uint8_t* item_file, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		item_file[i] ^= decryptionKey[i % 11];
	}
}

void decrypt_messages(uint8_t logic_no)
{
	uint8_t* buffer = heap_data.loaded_logics[logic_no].buffer;
	agi_messages_header_t* message_section = (agi_messages_header_t*)(buffer + ((buffer[1] << 8) | buffer[0]) + 2);

	int ptr_table_len = message_section->num_messages * 2;
	char* message_ptr = ((uint8_t*)message_section) + 3 + ptr_table_len;
	char* messages_end = ((uint8_t*)message_section) + message_section->messages_end;

	uint8_t decI = 0;

	while (message_ptr < messages_end)
	{
		*(message_ptr++) ^= decryptionKey[decI++];
		if (decI == 11)
			decI = 0;
	}
	*message_ptr = '\0';
}