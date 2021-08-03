#include <string.h>

#include "../actions.h"
#include "../state.h"

bool equaln(uint8_t var, uint8_t num) {
	return state.variables[var] == num;
}

bool equalv(uint8_t var1, uint8_t var2) {
	return state.variables[var1] == state.variables[var2];
}

bool lessn(uint8_t var, uint8_t num) {
	return state.variables[var] < num;
}

bool lessv(uint8_t var, uint8_t var2) {
	return state.variables[var] < state.variables[var2];
}

bool greatern(uint8_t var, uint8_t num) {
	return state.variables[var] > num;
}

bool greaterv(uint8_t var, uint8_t var2) {
	return state.variables[var] > state.variables[var2];
}

bool isset(uint8_t flag) {
	return state.flags[flag];
}

bool issetv(uint8_t var) {
	return state.flags[state.variables[var]];
}

bool has(uint8_t item) {
	return state.item_file->items[item].room_no == 255;
}

bool obj_in_room(uint8_t item, uint8_t var) {	
	return state.variables[var] == state.item_file->items[item].room_no;
}

bool posn(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	return OBJ.x >= x1 && OBJ.x <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}

bool controller(uint8_t ctr) {
	// TODO
	return false;
}

bool have_key() {
	return state.enter_pressed;
}

bool said() {
	uint8_t numparams = next_data();
	for (size_t i = 0; i < numparams; i++)
	{
		uint16_t wordNo = next_data() | (next_data() << 8);
	}
	// TODO
	return false;
}

bool compare_strings(uint8_t str, uint8_t str2) {
	return strcmp(state.strings[str], state.strings[str2]) == 0;
}

bool obj_in_box(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	int width = _view_width(OBJ.view_no, OBJ.loop_no, OBJ.cel_no);
	return (OBJ.x + width) > x1 && OBJ.x <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}

bool center_posn(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	int width = _view_width(OBJ.view_no, OBJ.loop_no, OBJ.cel_no);
	int midX = OBJ.x + (width / 2);
	return midX >= x1 && midX <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}

bool right_posn(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	int width = _view_width(OBJ.view_no, OBJ.loop_no, OBJ.cel_no);
	int right_x = OBJ.x + width;
	return right_x >= x1 && right_x <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}