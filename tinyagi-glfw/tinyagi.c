#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include <math.h>

#include <GLFW/glfw3.h>
#include <gl/GL.h>

#include "agi.h"
#include "winmm_audio.h"
#include "synth.h"

uint8_t screen_priority[160 * 168];
uint32_t framebuffer[320 * 200];

void panic(const char* fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	char buffer[256];
	vsprintf(buffer, fmt, vl);
	printf("Panic! %s\n", buffer);
	exit(1);
}

const char* game_path = "C:\\classic\\sierra\\kq3";
agi_file_t get_file(const char* filename) {
	char path[256];
	sprintf(path, "%s\\%s\0", game_path, filename);


	agi_file_t result;
	FILE* f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	result.size = ftell(f);
	result.data = (uint8_t*)malloc(result.size);
	if (result.data) {
		fseek(f, 0, SEEK_SET);
		fread(result.data, 1, result.size, f);
	}
	else {
		panic("Failed to allocate memory for file %s, of size %d", path, result.size);
	}

	fclose(f);
	return result;
}

void free_file(agi_file_t file) {
	free(file.data);
}

const unsigned int palette[16] = {
	0xFF000000,
	0xFF0000AA,
	0xFF00AA00,
	0xFF00AAAA,
	0xFFAA0000,
	0xFFAA00AA,
	0xFFAA5500,
	0xFFAAAAAA,
	0xFF555555,
	0xFF5555FF,
	0xFF55FF55,
	0xFF55FFFF,
	0xFFFF5555,
	0xFFFF55FF,
	0xFFFFFF55,
	0xFFFFFFFF
};

inline void screen_set_160(int x, int y, int color) {
	int x2 = x << 1;
	y += state.play_top * 8;
	framebuffer[y * 320 + x2 + 0] = palette[color];
	framebuffer[y * 320 + x2 + 1] = palette[color];
}

inline void screen_set_320(int x, int y, int color) {
	framebuffer[y * 320 + x] = palette[color];
}

inline void priority_set(int x, int y, int priority) {
	screen_priority[y * 160 + x] = priority;
}

inline int priority_get(int x, int y) {
	return screen_priority[y * 160 + x];
}


void window_resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

bool show_priority = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) && action == GLFW_PRESS)
		state.enter_pressed = true;

	if (!state.program_control) {
		if ((key == GLFW_KEY_UP || key == GLFW_KEY_KP_8) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP ? DIR_STOPPED : DIR_UP;
		else if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_KP_2) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN ? DIR_STOPPED : DIR_DOWN;
		else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_KP_4) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_LEFT ? DIR_STOPPED : DIR_LEFT;
		else if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_KP_6) && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_RIGHT ? DIR_STOPPED : DIR_RIGHT;
		else if (key == GLFW_KEY_KP_7 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_LEFT ? DIR_STOPPED : DIR_UP_LEFT;
		else if (key == GLFW_KEY_KP_9 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_RIGHT ? DIR_STOPPED : DIR_UP_RIGHT;
		else if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_LEFT ? DIR_STOPPED : DIR_DOWN_LEFT;
		else if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_RIGHT ? DIR_STOPPED : DIR_DOWN_RIGHT;
		else if (key == GLFW_KEY_KP_5 && action == GLFW_PRESS)
			state.variables[VAR_6_EGO_DIRECTION] = DIR_STOPPED;
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		show_priority = !show_priority;
	}

	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		agi_push_char('\b');
	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	char c = (char)codepoint;
	agi_push_char(c);
}

GLFWwindow* window;

void render() {
	if (show_priority) {
		for (size_t y = 0; y < 168; y++)
		{
			for (size_t x = 0; x < 160; x++)
			{				
				unsigned int c = palette[priority_get(x, y)];
				framebuffer[y * 320 + (x << 1) + 0] = c;
				framebuffer[y * 320 + (x << 1) + 1] = c;
			}
		}
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_BGRA_EXT, GL_UNSIGNED_BYTE, framebuffer);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0);	glVertex2f(320, 0);
	glTexCoord2f(1, 1);	glVertex2f(320, 200);
	glTexCoord2f(0, 1);	glVertex2f(0, 200);
	glEnd();

	/* Swap front and back buffers */
	glfwSwapBuffers(window);
}

void wait_for_enter() {
	render();

	state.enter_pressed = false;
	while (!state.enter_pressed) {
		Sleep(10);
		glfwPollEvents();
	}
}

void agi_ext_sound_update(uint16_t channel_hz[4]) {
	/*for (size_t i = 0; i < 4; i++)
	{
		synth_set_frequency(i, channel_hz[i]);
	}
	synth_get_sample_points()*/
}

#define PI 3.14159265358979323846
int main() {

	// Init audio
	/*winmm_audio_init();
	
	uint8_t sample[64];
	for (size_t i = 0; i < 64; i++)
	{
		sample[i] = (uint8_t)(sin((i / 64.0f) * PI * 2) * 255);
	}
	synth_set_sample(sample, 64);*/


	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 400, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window, window_resize);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);

	glEnable(GL_TEXTURE_2D);

	agi_reset();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 320, 200, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 200, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, framebuffer);

	int clock_counter = 0;
	double last_s = 0;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double delta = now - last_s;

		if (delta >= 0.06) {
			last_s = now;

			clock_counter++;
			if (clock_counter > 20) {
				state.variables[VAR_11_CLOCK_SECONDS]++;
				if (state.variables[VAR_11_CLOCK_SECONDS] == 60) {
					state.variables[VAR_11_CLOCK_SECONDS] = 0;
					state.variables[VAR_12_CLOCK_MINUTES]++;
					if (state.variables[VAR_12_CLOCK_MINUTES] == 60) {
						state.variables[VAR_12_CLOCK_MINUTES] = 0;
						state.variables[VAR_13_CLOCK_HOURS]++;
						if (state.variables[VAR_13_CLOCK_HOURS] == 24) {
							state.variables[VAR_13_CLOCK_HOURS] = 0;
							state.variables[VAR_14_CLOCK_DAYS]++;
						}
					}
				}
				clock_counter = 0;
			}

			_undraw_all();
			agi_logic_run_cycle();
			_draw_all_active();
			render();
			state.enter_pressed = false;
		}

		/* Poll for and process events */
		glfwPollEvents();

		Sleep(1);
	}

	glfwTerminate();
	return 0;
}
