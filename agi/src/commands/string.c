#include <string.h>

#include "../state.h"
#include "../actions.h"
#include "../platform_support.h"
#include "../text_display.h"
#include "../text_parser.h"

void get_string(uint8_t str, uint8_t msg, uint8_t col, uint8_t row, uint8_t maxLen) {
	UNIMPLEMENTED
}

void parse(uint8_t str) {
	UNIMPLEMENTED
}

void set_string(uint8_t str, uint8_t msg) {
	const char* message = get_message(state.current_logic, msg);
	strcpy(state.strings[str], message);
}

void word_to_string(uint8_t word, uint8_t str) {
	UNIMPLEMENTED
}