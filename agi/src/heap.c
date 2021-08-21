#include "heap.h"
#include "platform_support.h"
#include "state.h"
#include "decryption.h"

#include <stdlib.h>
#include <string.h>

agi_heap_t heap_data;


void heap_reload_object_file() {
	agi_file_t item_file = get_file("object");

	if (heap_data.item_file) {
		free(heap_data.item_file);
	}

	heap_data.item_file = (item_file_t*)malloc(item_file.size);
	if (!heap_data.item_file) {
		panic("Failed to allocate memory for object file!");
		return;
	}

	memcpy(heap_data.item_file, item_file.data, item_file.size);

	//free(item_file.data);

	decrypt_item_file((uint8_t*)heap_data.item_file, item_file.size);
}

void heap_load_words_file() {
	agi_file_t words_file = get_file("words.tok");
	heap_data.words_file = (words_file_t*)words_file.data;
}

void heap_reset_script_entries() {
	if (heap_data.script_entries) {
		free(heap_data.script_entries);
		heap_data.script_entries = NULL;
	}

	if (heap_data.script_size > 0) {
		heap_data.script_entries = malloc(heap_data.script_size * sizeof(script_entry_t));
	}

	heap_data.script_entry_pos = 0;
}

void heap_destroy_resources() {
	for (size_t i = 0; i < 256; i++)
	{
		if (i > 0) { // Don't destroy logic 0
			discard_vol_data(&(heap_data.loaded_logics[i]));
		}
		discard_vol_data(&(heap_data.loaded_pics[i]));
		discard_vol_data(&(heap_data.loaded_views[i]));
		discard_vol_data(&(heap_data.loaded_sounds[i]));
	}
}

void heap_reset() {
	heap_destroy_resources();
	heap_reset_script_entries();

	if (!heap_data.item_file) {
		heap_reload_object_file();
	}
	if (!heap_data.words_file) {
		heap_load_words_file();
	}
}

void heap_write_script_entry(uint8_t script_type, uint8_t resource_no) {
	if (state.flags[FLAG_7_SCRIPT_BUFFER_WRITE_LOCK]) {
		return;
	}
	heap_data.script_entries[heap_data.script_entry_pos].script_type = script_type;
	heap_data.script_entries[heap_data.script_entry_pos].resource_number = resource_no;
	heap_data.script_entry_pos++;
}

void heap_write_add_to_pic_script_entry(uint8_t view_no, uint8_t loop_no, uint8_t cel_no, uint8_t x, uint8_t y, uint8_t pri) {
	heap_write_script_entry(SCRIPT_ENTRY_ADD_TO_PIC, 0);
	heap_write_script_entry(view_no, loop_no);
	heap_write_script_entry(cel_no, x);
	heap_write_script_entry(y, pri);
}
