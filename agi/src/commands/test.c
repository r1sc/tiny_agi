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
	// TODO
	return false;
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
	// TODO
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
	// TODO
	return false;
}

bool obj_in_box(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4) {
	// TODO
	return false;
}

bool center_posn(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4) {
	// TODO
	return false;
}

bool right_posn(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4) {
	// TODO
	return false;
}