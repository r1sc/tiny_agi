#include <Windows.h>

#include "synth.h"
#include "panic.h"

static int sample_rate = 8000;
static int num_channels = 2;
static int bits_per_sample = 8;

static uint8_t buffer[8000]; // 1s sound
static uint8_t *buffer_pos;
static uint8_t* write_pos;

static HWAVEOUT waveout;

void write_next_buffer() {
	size_t len = buffer_pos - write_pos;
	WAVEHDR wave_header = {
		.lpData = write_pos,
		.dwBufferLength = len,
		.dwBytesRecorded = 0,
		.dwUser = 0,
		.dwFlags = 0,
		.dwLoops = 0,
		.lpNext = 0,
		.reserved = 0
	};
	int result;
	if ((result = waveOutPrepareHeader(waveout, &wave_header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR) {
		panic("waveOutPrepareHeader: %d\n", result);
	}

	if ((result = waveOutWrite(waveout, &wave_header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR) {
		panic("waveOutWrite: %d\n", result);
	}

	write_pos += len;
	if (write_pos >= buffer + 8000) {
		write_pos = buffer;
	}
}

void CALLBACK wave_out_callback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (uMsg == WOM_DONE) {
		WAVEHDR* wave_header = (WAVEHDR*)dwParam1;
		waveOutUnprepareHeader(waveout, wave_header, sizeof(WAVEHDR));
	}
}

#define NUM_CHANNELS 4
static size_t raw_sample_size;
static uint8_t* raw_sample;
static synth_channel_t channels[NUM_CHANNELS];


uint8_t step_channel(synth_channel_t* channel) {
	if (channel->frequency == 0) {
		return 128;
	}

	uint8_t sample_point = raw_sample[(int)channel->sample_pos];
	channel->sample_pos += channel->sample_step;
	if (channel->sample_pos >= raw_sample_size) {
		channel->sample_pos -= raw_sample_size;
	}
	return sample_point;
}

uint8_t mix() {
	uint16_t sample_point_acc = 0;
	for (size_t c = 0; c < NUM_CHANNELS; c++)
	{
		sample_point_acc += step_channel(&channels[c]);
	}
	return (uint8_t)(sample_point_acc / NUM_CHANNELS);
}

void synth_set_frequencies(uint16_t frequencies[NUM_CHANNELS], size_t num_samples) {
	for (size_t channel_no = 0; channel_no < NUM_CHANNELS; channel_no++)
	{
		uint16_t frequency = frequencies[channel_no];
		if (channels[channel_no].frequency != frequency) {
			channels[channel_no].frequency = frequency;
			channels[channel_no].sample_step = 64 / (8000 / (float)frequency);
		}
	}

	for (size_t i = 0; i < num_samples; i++)
	{
		uint8_t mixed_sample = mix();
		*(buffer_pos++) = mixed_sample;

	}

	if (buffer_pos >= buffer + 8000) {
		buffer_pos = buffer;
		write_next_buffer();
	}
}

void synth_init() {
	WORD nBlockAlign = (num_channels * bits_per_sample) / 8;
	DWORD nAvgBytesPerSec = sample_rate * nBlockAlign;

	WAVEFORMATEX wave_format = {
		.wFormatTag = WAVE_FORMAT_PCM,
		.nChannels = (WORD)num_channels, // Stereo
		.nSamplesPerSec = (DWORD)sample_rate,
		.nAvgBytesPerSec = nAvgBytesPerSec,
		.nBlockAlign = nBlockAlign,
		.wBitsPerSample = (WORD)bits_per_sample,
		.cbSize = 0
	};

	int result;
	if ((result = waveOutOpen(&waveout, WAVE_MAPPER, &wave_format, wave_out_callback, NULL, CALLBACK_FUNCTION)) != MMSYSERR_NOERROR) {
		panic("Failed to open wave out device. Error: %d\n", result);
	}
}