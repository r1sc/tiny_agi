#include <stdio.h>
#include "agi_sound.h"
#include "waveout.h"
#include <math.h>

typedef struct
{
    uint32_t hz;
    double duration;
    uint8_t attenuation;
} chan_note_t;

#define NUM_CHANNELS 3
static uint16_t channel_offsets[NUM_CHANNELS];
static chan_note_t channel_notes[NUM_CHANNELS];

static uint8_t* sound_data;

#define read_16(x) (sound_data[x + 1] << 8) | sound_data[x]
#define SAMPLE_RATE 22050
#define BUFFER_SIZE 512

void agi_sound_init() {
	waveout_initialize(SAMPLE_RATE, BUFFER_SIZE);
}

chan_note_t read_chan(uint32_t offset)
{
    chan_note_t note;

    note.duration = (double)(read_16(offset));
    if(note.duration == 0xFFFF)
        return note;

    offset += 2;
    uint8_t freq1 = sound_data[offset++];
    uint8_t freq2 = sound_data[offset++];
    uint16_t hz = ((freq1 & 0x3F) << 4) + (freq2 & 0x0F);
    note.hz = hz <= 0 ? 0 : 111860 / hz;
    note.attenuation = sound_data[offset++];
    if((note.attenuation & 0x0F) == 0x0F) {
        note.hz = 0;
    }

    return note;
}

bool agi_sound_fill()
{
	double DEG2RAD = 3.141592 / 180.0;
	static double an = 0.0;
	double ms_per_sample = 60.0 / (double)(SAMPLE_RATE);
	size_t i = 0;
	bool channel_done = false;

	int16_t* buffer = waveout_get_current_buffer();
	if(buffer != NULL) {
		int16_t* end = (int16_t*)(buffer + BUFFER_SIZE);

		while (buffer < end) {
			if (sound_data == NULL) {
				channel_notes[i].hz = 0;
				channel_done = true;
			} else {
				if (channel_notes[i].duration <= 0)
				{
					channel_notes[i] = read_chan(channel_offsets[i]);
					if (channel_notes[i].duration != 0xFFFF) {
						channel_offsets[i] += 5;
					}
				}

				if (channel_notes[i].duration == 0xFFFF) {
					// End of channel
					channel_notes[i].hz = 0;
					channel_done = true;
				} else {
					channel_notes[i].duration -= ms_per_sample;
				}

				if ((channel_notes[i].attenuation & 0x0F) == 0x0F) {
					channel_notes[i].hz = 0;
				}
			}

			*buffer++ = (int16_t)(sin(an * DEG2RAD) * INT16_MAX);
			an += (360.0 * (double)channel_notes[i].hz) / (double)SAMPLE_RATE;
		}
		waveout_queue_buffer();
	}

    if(channel_done){
		sound_data = NULL;
        return true;
    }

    return false;
}

void agi_sound_start(uint8_t* new_sound_data)
{
    sound_data = new_sound_data;
    for (size_t i = 0; i < NUM_CHANNELS; i++)
    {
        /* code */
        channel_notes[i].duration = 0;
        channel_offsets[i] = read_16(2 * i);
    }
}

void agi_sound_stop() {
    sound_data = NULL;
}