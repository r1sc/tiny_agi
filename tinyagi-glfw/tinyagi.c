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

agi_save_data_file_ptr agi_save_data_open(const char* mode) {
	FILE *f = fopen("savegame", mode);
	return (agi_save_data_file_ptr*)f;
}

void agi_save_data_write(agi_save_data_file_ptr file_ptr, void* data, size_t size) {
	FILE *f = (FILE*)file_ptr;
	fwrite(data, size, 1, f);
}

void agi_save_data_read(agi_save_data_file_ptr file_ptr, void* destination, size_t size) {
	FILE *f = (FILE*)file_ptr;
	fread(destination, size, 1, f);
}

void agi_save_data_close(agi_save_data_file_ptr file_ptr) {
	FILE *f = (FILE*)file_ptr;
	fclose(f);
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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) && action == GLFW_PRESS) {
		state.enter_pressed = true;
	}

	if ((key == GLFW_KEY_UP || key == GLFW_KEY_KP_8) && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_UP);
	else if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_KP_2) && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_DOWN);
	else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_KP_4) && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_LEFT);
	else if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_KP_6) && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_RIGHT);
	else if (key == GLFW_KEY_KP_7 && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_HOME);
	else if (key == GLFW_KEY_KP_9 && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_PGUP);
	else if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_END);
	else if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
		agi_input_queue_push_keypress(0, AGI_KEY_PGDN);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		show_priority = !show_priority;
	}

	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		agi_input_queue_push_keypress('\b', 0);
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		agi_input_queue_push_keypress((char)27, 0);
	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	char c = (char)codepoint;
	agi_input_queue_push_keypress(c, 0);
}

void agi_stop_sound() {}
void agi_play_sound(uint8_t* sound_data) {
	//state.flags[state.sound_flag] = true;
}

GLFWwindow* window;

void render() {
	if (show_priority) {
		for (size_t y = 0; y < 168; y++) {
			for (size_t x = 0; x < 160; x++) {
				screen_set_160(x, y, priority_get(x, y));
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

void check_key() {
	render();
	glfwPollEvents();
	Sleep(10);
}

int main() {
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(320 * 3, 200 * 3, "TinyAGI", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, window_resize);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);

	glEnable(GL_TEXTURE_2D);

	agi_initialize();

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
	while (!glfwWindowShouldClose(window)) {
		double now = glfwGetTime() * 1000;

		agi_logic_run_cycle((uint32_t)now);
		agi_draw_all_active();
		render();

		glfwPollEvents();
		Sleep(1);
	}

	glfwTerminate();
	return 0;
}
