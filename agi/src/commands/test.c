#include <string.h>

#include "../actions.h"
#include "../state.h"
#include "../heap.h"
#include "../view.h"

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
	return heap_data.item_file->items[item].room_no == 255;
}

bool obj_in_room(uint8_t item, uint8_t var) {	
	return state.variables[var] == heap_data.item_file->items[item].room_no;
}

bool posn(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	return OBJ.x >= x1 && OBJ.x <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}

bool controller(uint8_t ctr) {
	return state.controllers[ctr];
}

bool have_key() {
	return state.enter_pressed;
}

#define MIN(x, y) (x < y ? x : y)
bool said() {
	uint8_t numparams = next_data();
	bool match = true;
	bool rol = false;
	for (size_t i = 0; i < numparams; i++)
	{
		uint16_t a = next_data();
		uint16_t b = next_data();
		uint16_t logic_word_no = a | (b << 8);
		if (rol || logic_word_no == 9999) {
			rol = true;
			continue;
		}
		else if (match && i < system_state.num_parsed_word_groups) {
			if (system_state.parsed_word_groups[i] != logic_word_no) {
				match = false;
			}
		}
		else {
			match = false;
		}
	}

	if (!state.or_result && !state.and_result)
		return false;

	if(!state.or && state.and_result == false) { // Skip checking if in AND-testing mode and the previous result was false
		match = false;
	}	
	else if(!state.flags[FLAG_2_COMMAND_ENTERED]) {
		match = false; // No command entered
	}
	else if(state.flags[FLAG_4_SAID_ACCEPTED_INPUT]) {
		match = false; // Already accepted earlier
	}
	else if (state.variables[VAR_9_MISSING_WORD_NO] > 0) {
		match = false;
	}
	else if (!rol && system_state.num_parsed_word_groups > numparams) {
		match = false;
	}	
	else if (match) {
		state.flags[FLAG_4_SAID_ACCEPTED_INPUT] = true;
	}
	return match;
}

void _trim_string(char* destination, const char* original) {
	while (1)
	{
		char c = *(original++);
		if(c == ' ' || 
			c == '\t' ||
			c == '.' ||
			c == ',' ||
			c == ';' ||
			c == ':' ||
			c == '\'' ||
			c == '!' ||
			c == '-'
		) {
			continue;
		}
		if(c >= 'a' && c <= 'z') {
			c -= ('a' - 'A'); // To lower case
		}
		*(destination++) = c;
		if(c == '\0')
			return;		
	}	
}

bool compare_strings(uint8_t str, uint8_t str2) {
	char temp1[40], temp2[40];
	_trim_string(temp1, state.strings[str]);
	_trim_string(temp2, state.strings[str2]);
	return strcmp(temp1, temp2) == 0;
}

bool obj_in_box(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	cell_t* cell = _object_cell(&OBJ);
	return (OBJ.x + cell->width) > x1 && OBJ.x <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}

bool center_posn(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	cell_t* cell = _object_cell(&OBJ);
	int midX = OBJ.x + (cell->width >> 1);
	return midX >= x1 && midX <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}

bool right_posn(uint8_t objNo, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	cell_t* cell = _object_cell(&OBJ);
	int right_x = OBJ.x + cell->width;
	return right_x >= x1 && right_x <= x2 && OBJ.y >= y1 && OBJ.y <= y2;
}