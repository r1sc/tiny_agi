#include "../state.h"
#include "../actions.h"

void drop(uint8_t item) {
	state.item_file->items[item].room_no = 0;
}

void get(uint8_t item) {
	state.item_file->items[item].room_no = 255;
}

void get_room_v(uint8_t v_item, uint8_t v_room) {
	uint8_t item_no = state.variables[v_item];
	state.variables[v_room] = state.item_file->items[item_no].room_no;
}

void get_v(uint8_t var) {
	get(state.variables[var]);
}

void put(uint8_t item, uint8_t room) {
	state.item_file->items[item].room_no = room;
}

void put_v(uint8_t v_item, uint8_t v_room) {
	put(state.variables[v_item], state.variables[v_room]);
}

void show_obj(uint8_t num) {
	const char* description = (const char*)(((uint8_t*)state.item_file->items) + state.item_file->items[num].name_offset);
	UNIMPLEMENTED
}

void show_obj_v(uint8_t var) {
	UNIMPLEMENTED
}

void status() {
	UNIMPLEMENTED
}