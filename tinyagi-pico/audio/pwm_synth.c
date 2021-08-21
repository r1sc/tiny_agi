#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"

#include "pwm_synth.h"
#include "strings.h"

#include <stdio.h>

uint slice_num;

// Sine wave sample rate = 8272Hz, sample tone ~= 523Hz (C4)
const int sample_length = 6924;
const float sample_rate_hz = 22050.0f;
const float num_sine_curves_per_sec = sample_rate_hz / (float)25;

pwm_synth_audio_channel_state_t pwm_synth_channels[PWM_SYNTH_NUM_CHANNELS];

void __not_in_flash_func(pih)() {
    uint16_t combined_samples = 0;

    for (size_t i = 0; i < PWM_SYNTH_NUM_CHANNELS; i++)
    {
        pwm_synth_audio_channel_state_t* state = &pwm_synth_channels[i];
        if(state->hz == 0) {
            combined_samples += 128;
        } else {
            float steps_per_sample = state->hz / num_sine_curves_per_sec;

            // float steps_per_sample = (float)(multiplier * state->hz) / sample_rate_hz;
            uint16_t sample = strings[(int)state->sample_pos] + 128;

            state->sample_pos += steps_per_sample;
            if(state->sample_pos >= sample_length) {
                state->sample_pos -= sample_length;
            }
            combined_samples += sample;
        }
    }

    uint16_t result_sample = combined_samples / PWM_SYNTH_NUM_CHANNELS;

    pwm_set_both_levels(slice_num, result_sample, result_sample);
    pwm_clear_irq(slice_num);
}


void pwm_synth_init(int pwm_pin_base) {
    // Silence all channels
    for (size_t i = 0; i < PWM_SYNTH_NUM_CHANNELS; i++)
    {
        pwm_synth_channels[i].hz = pwm_synth_channels[i].sample_pos = 0;
    }    

    gpio_set_function(pwm_pin_base, GPIO_FUNC_PWM);
    gpio_set_function(pwm_pin_base + 1, GPIO_FUNC_PWM);
 
    // Find out which PWM slice is connected to pwm_pin_base
    slice_num = pwm_gpio_to_slice_num(pwm_pin_base);

    uint32_t clock_rate_hz = clock_get_hz(clk_sys);
    uint32_t wrap = 256;
    float divider = (clock_rate_hz / (float)wrap) / sample_rate_hz;
    // printf("PWM Sound init. Clock-rate: %d, divider: %f, wrap: %d, sample-rate: %f\n", clock_rate_hz, divider, wrap, sample_rate_hz);

    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, wrap); 

    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pih);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_set_enabled(slice_num, true);
}

void pwm_synth_silence_all_channels() {
    for (size_t i = 0; i < PWM_SYNTH_NUM_CHANNELS; i++)
    {
        pwm_synth_channels[i].hz = 0;
    }    
}