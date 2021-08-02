#include "../actions.h"
#include "../state.h"

void load_sound(uint8_t num) {
	UNIMPLEMENTED
}

void sound(uint8_t num, uint8_t flag) {
	state.flags[flag] = true;
	UNIMPLEMENTED
}

void stop_sound() {
	UNIMPLEMENTED
}