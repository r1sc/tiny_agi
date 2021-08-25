#include "../state.h"
#include "../heap.h"
#include "../actions.h"
#include "../text_display.h"
#include "../view.h"
#include "../platform_support.h"

void drop(uint8_t item) {
	heap_data.item_file->items[item].room_no = 0;
}

void get(uint8_t item) {
	heap_data.item_file->items[item].room_no = 255;
}

void get_room_v(uint8_t v_item, uint8_t v_room) {
	uint8_t item_no = state.variables[v_item];
	state.variables[v_room] = heap_data.item_file->items[item_no].room_no;
}

void get_v(uint8_t var) {
	get(state.variables[var]);
}

void put(uint8_t item, uint8_t v_roomno) {
	heap_data.item_file->items[item].room_no = state.variables[v_roomno];
}

void put_v(uint8_t v_item, uint8_t v_room) {
	put(state.variables[v_item], v_room);
}

void show_obj(uint8_t num) {
	load_view(num);
	viewinfo_t info = view_get_show_obj_info(num);
	int center_x = 80 - (info.width >> 1);
	_draw_view(num, 0, 0, center_x, 167, 15, false, false);
	print_message_box(info.description, 30, 0, 0);
	wait_for_enter();
	show_pic();
}

void show_obj_v(uint8_t var) {
	show_obj(state.variables[var]);
}

void status() {
	UNIMPLEMENTED
}