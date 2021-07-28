#include "../state.h"

void reset(uint8_t flag) {
	state.flags[flag] = false;
}

void reset_v(uint8_t var) {
	state.flags[state.variables[var]] = false;
}

void set(uint8_t flag) {
	state.flags[flag] = true;
}

void set_v(uint8_t var) {
	state.flags[state.variables[var]] = true;
}

void toggle(uint8_t flag) {
	state.flags[flag] = !state.flags[flag];
}

void toggle_v(uint8_t var) {
	state.flags[state.variables[var]] = !state.flags[state.variables[var]];
}