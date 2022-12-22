#include <stdlib.h>
#include <string.h>

#include "../actions.h"
#include "../state.h"
#include "../platform_support.h"
#include "../text_display.h"
#include "../input_queue.h"

void addn(uint8_t var, uint8_t num) {
	state.variables[var] += num;
}

void addv(uint8_t var1, uint8_t var2) {
	state.variables[var1] += state.variables[var2];
}

void assignn(uint8_t var, uint8_t num) {
	state.variables[var] = num;
}

void assignv(uint8_t var1, uint8_t var2) {
	state.variables[var1] = state.variables[var2];
}

void decrement(uint8_t var) {
	if (state.variables[var] > 0)
		state.variables[var]--;
}

void div_n(uint8_t var, uint8_t num) {
	state.variables[var] /= num;
}

void div_v(uint8_t var, uint8_t var2) {
	state.variables[var] /= state.variables[var2];
}

void get_num(uint8_t msg_no, uint8_t var) {
	const char* msg = get_message(state.current_logic, msg_no);
	redraw_prompt(msg);

	system_state.input_buffer[0] = '\0';
	system_state.input_pos = 0;
	state.enter_pressed = false;

	bool done = false;
	while (!done) {
		check_key();

		for (int i = 0; i < queue_pos; i++) {
			input_queue_entry_t entry = queue[i];

			if (system_state.input_pos < 2 && entry.ascii >= '0' && entry.ascii <= '9') {
				system_state.input_buffer[system_state.input_pos++] = entry.ascii;
				system_state.input_buffer[system_state.input_pos] = '\0';
				redraw_prompt(msg);
			}
			else if (entry.ascii == '\b' && system_state.input_pos > 0) {
				system_state.input_buffer[system_state.input_pos--] = '\0';
				redraw_prompt(msg);
			}
			else if (entry.ascii == 27) {
				state.variables[var] = 0;
				done = true;
			}
		}

		queue_pos = 0;		

		if (state.enter_pressed) {
			uint8_t value = (uint8_t)atoi(system_state.input_buffer);
			state.variables[var] = value;
			done = true;			
		}
	}	

	system_state.input_buffer[0] = '\0';
	system_state.input_pos = 0;
	redraw_prompt(state.strings[0]);
}

void increment(uint8_t var) {
	if (state.variables[var] < 255)
		state.variables[var]++;
}

void lindirectn(uint8_t var1, uint8_t num) {
	uint8_t vc = state.variables[var1];
	state.variables[vc] = num;
}

void lindirectv(uint8_t var1, uint8_t var2) {
	uint8_t vc = state.variables[var1];
	state.variables[vc] = state.variables[var2];
}

void mul_n(uint8_t var, uint8_t num) {
	state.variables[var] *= num;
}

void mul_v(uint8_t var, uint8_t var2) {
	state.variables[var] *= state.variables[var2];
}

void _random(uint8_t num, uint8_t num2, uint8_t var) {
	state.variables[var] = rand() % (num2 - num) + num;
}

void rindirect(uint8_t var1, uint8_t var2) {
	uint8_t vc = state.variables[var2];
	state.variables[var1] = state.variables[vc];
}

void subn(uint8_t var, uint8_t num) {
	state.variables[var] -= num;
}

void subv(uint8_t var1, uint8_t var2) {
	state.variables[var1] -= state.variables[var2];
}