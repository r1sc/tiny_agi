#include "../actions.h"
#include "../state.h"
#include "../vol.h"
#include "../platform_support.h"

void load_sound(uint8_t num) {
	if(state.loaded_sounds[num])
		return;
	state.loaded_sounds[num] = load_vol_data("snddir", num, false).buffer;
}

void sound(uint8_t num, uint8_t flag) {
	state.sound_flag = flag;
	state.flags[flag] = false;
	agi_play_sound(state.loaded_sounds[num]);
}

void stop_sound() {
	if(state.sound_flag > -1) {
		state.flags[state.sound_flag] = true;
		state.sound_flag = -1;
	}
	agi_stop_sound();
}