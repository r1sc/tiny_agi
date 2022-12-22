#pragma once

#include <stdint.h>

typedef struct {
    uint16_t hz;
    float sample_pos;
} pwm_synth_audio_channel_state_t;

#define PWM_SYNTH_NUM_CHANNELS 3
extern pwm_synth_audio_channel_state_t pwm_synth_channels[PWM_SYNTH_NUM_CHANNELS];

extern void pwm_synth_init(int pwm_pin_base);
extern void pwm_synth_silence_all_channels();