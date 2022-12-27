#include "sound.h"
#include "platform_support.h"

chan_note_t channel_notes[AGI_NUM_CHANNELS];
static uint16_t channel_offsets[AGI_NUM_CHANNELS];

static uint8_t* sound_data;

#define read_16(x) (sound_data[x + 1] << 8) | sound_data[x]

chan_note_t read_chan(uint32_t offset)
{
    chan_note_t note;

    note.duration = read_16(offset);
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

void agi_sound_tick(int delta_ms)
{
    int num_channels_done = 0;
    for (size_t i = 0; i < AGI_NUM_CHANNELS; i++)
    {       
        if(sound_data == NULL) {
			channel_notes[i].hz = 0;
            continue;
        }

        if (channel_notes[i].duration <= 0)
        {
            channel_notes[i] = read_chan(channel_offsets[i]);
            if(channel_notes[i].duration != 0xFFFF) {
                channel_offsets[i] += 5;
            }
        }
        
        if(channel_notes[i].duration == 0xFFFF){
            // End of channel
            channel_notes[i].hz = 0;
            num_channels_done++;
        }
        else {            
            channel_notes[i].duration -= delta_ms;
        }
        
        if((channel_notes[i].attenuation & 0x0F) == 0x0F) {
            channel_notes[i].hz = 0;
        }
    }
}

void agi_sound_start(uint8_t* new_sound_data)
{
    sound_data = new_sound_data;
    for (size_t i = 0; i < AGI_NUM_CHANNELS; i++)
    {
        /* code */
        channel_notes[i].duration = 0;
        channel_offsets[i] = read_16(2 * i);
    }
}

void agi_sound_stop() {
	sound_data = NULL;
}