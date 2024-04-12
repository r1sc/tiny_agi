#include "agi_sound.h"
#include "waveout.h"
#include "strings.h"

typedef struct
{
	uint32_t hz;
	double duration;
	uint8_t attenuation;
} chan_note_t;

#define NUM_CHANNELS 3

typedef struct {
	uint32_t hz;
	uint16_t offset;
	chan_note_t note;
	float sample_pos;
	float steps_per_sample_sliding;
} synth_channel_t;

static synth_channel_t channels[NUM_CHANNELS];

static uint8_t* sound_data;

#define read_16(x) ((sound_data[x + 1] << 8) | sound_data[x])
#define SAMPLE_RATE 22050
#define BUFFER_SIZE 512

// Sine wave sample rate = 8272Hz, sample tone ~= 523Hz (C4)
const int sample_length = 6924;
const float num_sine_curves_per_sec = SAMPLE_RATE / (float)25;

void agi_sound_init() {
	waveout_initialize(SAMPLE_RATE, BUFFER_SIZE);
}

chan_note_t read_chan(uint32_t offset) {
	chan_note_t note;

	note.duration = (double)(read_16(offset));
	if (note.duration == 0xFFFF)
		return note;

	offset += 2;
	uint8_t freq1 = sound_data[offset++];
	uint8_t freq2 = sound_data[offset++];
	uint16_t hz = ((freq1 & 0x3F) << 4) + (freq2 & 0x0F);
	note.hz = hz <= 0 ? 0 : 111860 / hz;
	note.attenuation = sound_data[offset];

	if ((note.attenuation & 0x0F) == 0x0F) {
		note.hz = 0;
	}

	return note;
}

bool agi_sound_fill() {
	if (sound_data == NULL) return true;

	double ms_per_sample = 60.0 / (double)(SAMPLE_RATE);

	int16_t* buffer = waveout_get_current_buffer();
	if (buffer != NULL) {
		int16_t* end = (int16_t*)(buffer + BUFFER_SIZE);

		while (buffer < end) {
			int combined_buffer = 0;
			int num_channels_done = 0;

			for (size_t i = 0; i < NUM_CHANNELS; i++) {
				if (channels[i].note.duration <= 0) {
					channels[i].note = read_chan(channels[i].offset);
					if (channels[i].note.duration != 0xFFFF) {
						channels[i].offset += 5;
					}
				}

				if (channels[i].note.duration == 0xFFFF) {
					// End of channel
					channels[i].note.hz >>= 1;
					num_channels_done++;
				} else {
					channels[i].note.duration -= ms_per_sample;
				}

				if ((channels[i].note.attenuation & 0x0F) == 0x0F) {
					//channels[i].note.hz = 0;
				}

				float steps_per_sample = (float)(channels[i].note.hz) / num_sine_curves_per_sec;
				// slide towards steps_per_sample
				channels[i].steps_per_sample_sliding += (steps_per_sample - channels[i].steps_per_sample_sliding) * 0.05f;

				int16_t sample = (int16_t)(strings[(int)channels[i].sample_pos] * 127);

				channels[i].sample_pos += channels[i].steps_per_sample_sliding;
				while (channels[i].sample_pos >= (float)sample_length) {
					channels[i].sample_pos -= (float)sample_length;
				}

				combined_buffer += sample >> (channels[i].note.attenuation & 0x0F);
			}

			if (num_channels_done == NUM_CHANNELS) {
				sound_data = NULL;
				return true;
			}

			*buffer++ = (int16_t)(combined_buffer / NUM_CHANNELS);
		}
		waveout_queue_buffer();
	}

	return false;
}

void agi_sound_start(uint8_t* new_sound_data) {
	sound_data = new_sound_data;
	for (size_t i = 0; i < NUM_CHANNELS; i++) {
		/* code */
		channels[i].note.duration = 0;
		channels[i].offset = read_16(2 * i);
		channels[i].hz = 0;
		channels[i].sample_pos = 0;
		channels[i].steps_per_sample_sliding = 0;
	}
}

void agi_sound_stop() {
	sound_data = NULL;
}