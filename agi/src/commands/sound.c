#include "../actions.h"
#include "../state.h"
#include "../heap.h"
#include "../vol.h"
#include "../platform_support.h"

void load_sound(uint8_t num) {
	heap_write_script_entry(SCRIPT_ENTRY_LOAD_SOUND, num);

	if(heap_data.loaded_sounds[num].buffer)
		return;

	heap_data.loaded_sounds[num] = load_vol_data("snddir", num, false);
}

void sound(uint8_t num, uint8_t flag) {
	state.sound_flag = flag;
	state.flags[flag] = false;
	agi_play_sound(heap_data.loaded_sounds[num].buffer);
}

void stop_sound() {
	if(state.sound_flag > -1) {
		state.flags[state.sound_flag] = true;
		state.sound_flag = -1;
	}
	agi_stop_sound();
}