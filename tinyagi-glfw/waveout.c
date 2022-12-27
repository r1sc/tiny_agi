#include <Windows.h>
#include <stdio.h>

#include "waveout.h"

#define NUM_BUFFERS 4

HWAVEOUT waveout;
WAVEHDR audio_headers[NUM_BUFFERS];
int16_t* buffers[NUM_BUFFERS];

int16_t* queue[NUM_BUFFERS];
size_t queue_read = 0;
size_t queue_write = 0;
int rw = 0;

void push_free_buffer(int16_t* buffer_ptr) {
	rw++;
	queue[queue_write++] = buffer_ptr;
	if (queue_write == NUM_BUFFERS) {
		queue_write = 0;
	}
}

// Returns NULL if all buffers are full
int16_t* waveout_get_current_buffer() {
	if (rw == 0) return NULL;
	return queue[queue_read];
}

void waveout_queue_buffer() {
	rw--;

	WAVEHDR* audio_header = &audio_headers[queue_read++];
	if (queue_read == NUM_BUFFERS) {
		queue_read = 0;
	}

	MMRESULT result = waveOutPrepareHeader(waveout, audio_header, sizeof(WAVEHDR));
	if (result != MMSYSERR_NOERROR) {
		printf("Error preparing header: %d\n", result);
		return;
	}
	result = waveOutWrite(waveout, audio_header, sizeof(WAVEHDR));
	if (result != MMSYSERR_NOERROR) {
		printf("Error writing wave data: %d\n", result);
		return;
	}
}

BOOL shutting_down = FALSE;
void waveout_free() {
	shutting_down = TRUE;
	waveOutReset(waveout);
	for (size_t i = 0; i < NUM_BUFFERS; i++) {
		MMRESULT result = waveOutUnprepareHeader(waveout, &audio_headers[i], sizeof(WAVEHDR));
		if (result != MMSYSERR_NOERROR) {
			printf("Error freeing buffer!\n");
			exit(1);
		}
	}
	waveOutClose(waveout);
	for (size_t i = 0; i < NUM_BUFFERS; i++) {
		free(buffers[i]);
	}
}

void CALLBACK waveOutProc(
	HWAVEOUT  hwo,
	UINT      uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
) {
	if (uMsg == WOM_DONE && !shutting_down) {
		LPWAVEHDR audio_header = (LPWAVEHDR)dwParam1;
		MMRESULT result = waveOutUnprepareHeader(waveout, audio_header, sizeof(WAVEHDR));
		if (result != MMSYSERR_NOERROR) {
			printf("Error unpreparing header: %d\n", result);
			return;
		}

		int16_t* buffer = (int16_t*)audio_header->lpData;
		push_free_buffer(buffer);
	}
}

void waveout_initialize(unsigned int sample_rate, unsigned buffer_size) {
	WORD nChannels = 1;
	WORD wBitsPerSample = 16;
	WORD nBlockAlign = (nChannels * wBitsPerSample) / 8;
	DWORD nAvgBytesPerSec = sample_rate * nBlockAlign;

	WAVEFORMATEX format = {
		.wFormatTag = WAVE_FORMAT_PCM,
		.nChannels = nChannels, // mono
		.nSamplesPerSec = sample_rate,
		.nAvgBytesPerSec = nAvgBytesPerSec,
		.nBlockAlign = nBlockAlign,
		.wBitsPerSample = wBitsPerSample,
		.cbSize = 0
	};

	MMRESULT result = waveOutOpen(&waveout, WAVE_MAPPER, &format, (DWORD_PTR)waveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR) {
		printf("Error opening device: %d\n", result);
		return;
	}

	// Prepare buffers
	for (size_t i = 0; i < NUM_BUFFERS; i++) {
		buffers[i] = (int16_t*)calloc(buffer_size, sizeof(int16_t));

		ZeroMemory(&audio_headers[i], sizeof(WAVEHDR));
		audio_headers[i].lpData = (LPSTR)buffers[i];
		audio_headers[i].dwBufferLength = buffer_size * sizeof(int16_t);

		push_free_buffer(buffers[i]);
	}
}