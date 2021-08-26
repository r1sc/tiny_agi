#include <stdlib.h>

#include "../actions.h"
#include "../state.h"

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

void get_num(uint8_t str, uint8_t var) {
	state.game_state = STATE_GET_NUM;	
	state.target_var = var;
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