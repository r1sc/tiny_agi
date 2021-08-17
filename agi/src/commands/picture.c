#include <stdlib.h>

#include "../state.h"
#include "../platform_support.h"

bool vis_enabled = false;
bool pri_enabled = false;
uint8_t vis_color = 0;
uint8_t pri_color = 0;
uint8_t pic_vispri[160 * 168];

void pic_vis_set(int x, int y, int color) {
	pic_vispri[y * 160 + x] = (pic_vispri[y * 160 + x] & 0x0F) | (color << 4);
}

int pic_vis_get(int x, int y) {
	return pic_vispri[y * 160 + x] >> 4;
}

void pic_pri_set(int x, int y, int priority) {
	pic_vispri[y * 160 + x] = (pic_vispri[y * 160 + x] & 0xF0) | priority;
}

int pic_pri_get(int x, int y) {
	return pic_vispri[y * 160 + x] & 0x0F;
}

void _clear_screen() {
	for (size_t y = 0; y < 168; y++)
	{
		for (size_t x = 0; x < 160; x++)
		{
			pic_vis_set(x, y, 15);
			pic_pri_set(x, y, 4);
		}
	}
}

void _pset(uint8_t x, uint8_t y) {
	if (vis_enabled)
		pic_vis_set(x, y, vis_color);
	if (pri_enabled)
		pic_pri_set(x, y, pri_color);
}

void _draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	int dx = abs(x2 - x1);
	int sx = x1 < x2 ? 1 : -1;
	int dy = -abs(y2 - y1);
	int sy = y1 < y2 ? 1 : -1;
	int err = dx + dy;
	while (1) {
		_pset(x1, y1);
		if (x1 == x2 && y1 == y2)
			break;
		int e2 = err << 1;
		if (e2 >= dy) {
			err += dy;
			x1 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y1 += sy;
		}
	}
}

void _flood_fill(uint8_t startX, uint8_t startY)
{
	if (!vis_enabled && !pri_enabled) return;
	if (vis_enabled && vis_color == 15) return;

#define QUEUE_LEN 4096
	uint8_t queue[QUEUE_LEN];
	unsigned int wp = 0;
	unsigned int rp = 0;

#define enqueue(a) queue[wp]=a; wp=(wp+1)%QUEUE_LEN;
#define pop() queue[rp]; rp=(rp+1)%QUEUE_LEN;
#define push(x,y) { enqueue(x); enqueue(y); }

	push(startX, startY);
	while (wp != rp) {
		uint8_t x = pop();
		uint8_t y = pop();
		if (vis_enabled && pic_vis_get(x, y) != 15)
			continue;
		if (pri_enabled && pic_pri_get(x, y) != 4)
			continue;

		_pset(x, y);

		if (x > 0) push(x - 1, y);
		if (x < 159) push(x + 1, y);

		if (y > 0) push(x, y - 1);
		if (y < 167) push(x, y + 1);
	}
}

#define PIC_SET_COLOR 0xF0
#define PIC_DISABLE 0xF1
#define PRI_SET_COLOR 0xF2
#define PRI_DISABLE 0xF3
#define DRAW_Y_CORNER 0xF4
#define DRAW_X_CORNER 0xF5
#define DRAW_ABS 0xF6
#define DRAW_REL 0xF7
#define DRAW_FILL 0xF8
#define SET_PEN 0xF9
#define DRAW_PEN 0xFA
#define END 0xFF

void _pic_draw(uint8_t pic_no) {
	vis_enabled = false;
	pri_enabled = false;
	vis_color = 0;
	pri_color = 0;

	uint8_t* buffer = heap_data.loaded_pics[pic_no].buffer;
	if (!buffer) {
		panic("No picture loaded!");
		return;
	}

	for (size_t i = 0; i < heap_data.loaded_pics[pic_no].size;)
	{
		uint8_t value = buffer[i++];
		switch (value) {
		case PIC_SET_COLOR:
			vis_enabled = true;
			vis_color = buffer[i++];
			break;
		case PIC_DISABLE:
			vis_enabled = false;
			break;
		case PRI_SET_COLOR:
			pri_enabled = true;
			pri_color = buffer[i++];
			break;
		case PRI_DISABLE:
			pri_enabled = false;
			break;
		case DRAW_Y_CORNER:
		{
			uint8_t x1 = buffer[i++];
			uint8_t y1 = buffer[i++];
			_pset(x1, y1);
			while (1) {
				uint8_t y2 = buffer[i];
				if (y2 >= 0xF0)
					break;
				i++;
				_draw_line(x1, y1, x1, y2);
				y1 = y2;
				uint8_t x2 = buffer[i];
				if (x2 >= 0xF0)
					break;
				i++;
				_draw_line(x1, y1, x2, y1);
				x1 = x2;
			}
		}
		break;
		case DRAW_X_CORNER:
		{
			uint8_t x1 = buffer[i++];
			uint8_t y1 = buffer[i++];
			_pset(x1, y1);
			while (1) {
				uint8_t x2 = buffer[i];
				if (x2 >= 0xF0)
					break;
				i++;
				_draw_line(x1, y1, x2, y1);
				x1 = x2;
				uint8_t y2 = buffer[i];
				if (y2 >= 0xF0)
					break;
				i++;
				_draw_line(x1, y1, x1, y2);
				y1 = y2;
			}
		}
		break;
		case DRAW_ABS:
		{
			uint8_t x1 = buffer[i++];
			uint8_t y1 = buffer[i++];
			_pset(x1, y1);
			while (1) {
				uint8_t x2 = buffer[i];
				if (x2 >= 0xF0)
					break;
				i++;
				uint8_t y2 = buffer[i++];
				_draw_line(x1, y1, x2, y2);
				x1 = x2;
				y1 = y2;
			}

		}
		break;
		case DRAW_REL:
		{
			uint8_t x1 = buffer[i++];
			uint8_t y1 = buffer[i++];
			_pset(x1, y1);

			while (1) {
				uint8_t xd = buffer[i];
				if (xd >= 0xF0)
					break;
				i++;

				int xDisp = (xd >> 4) & 7;
				if (xd & 0x80)
					xDisp = -xDisp;

				int yDisp = xd & 7;
				if (xd & 8)
					yDisp = -yDisp;

				uint8_t x2 = x1 + xDisp;
				uint8_t y2 = y1 + yDisp;
				_draw_line(x1, y1, x2, y2);
				x1 = x2;
				y1 = y2;
			}
		}
		break;
		case DRAW_FILL:
		{
			while (1) {
				uint8_t x = buffer[i];
				if (x >= 0xF0)
					break;
				i++;
				uint8_t y = buffer[i++];
				_flood_fill(x, y);
			}

		}
		break;
		case SET_PEN:
			i++;
			break;
		case DRAW_PEN:
			while (buffer[i] < 0xF0) {
				uint8_t x = buffer[i++];
				uint8_t y = buffer[i++];
				_pset(x, y);
			}
			break;
		case END:
			return;
		default:
			panic("Unimplemented pic command %x!", value);
			break;
		}
	}
	panic("No end of picture marker found!");
}

/* COMMANDS */

void show_pic() {
	for (size_t y = 0; y < 168; y++)
	{
		for (size_t x = 0; x < 160; x++)
		{
			screen_set_160(x, y, pic_vis_get(x, y));
			priority_set(x, y, pic_pri_get(x, y));
		}
	}
}

void discard_pic_no(uint8_t pic_no) {
	write_script_entry(SCRIPT_ENTRY_DISCARD_PIC, pic_no);

	if(!agi_discard(&heap_data.loaded_pics[pic_no])) {
		panic("discard_pic: Pic %d not loaded!", pic_no);
	}
}

void discard_pic(uint8_t var) {
	discard_pic_no(state.variables[var]);
}

void draw_pic_no(uint8_t pic_no) {
	write_script_entry(SCRIPT_ENTRY_DRAW_PIC, pic_no);

	_clear_screen();
	_pic_draw(pic_no);
}

void draw_pic(uint8_t var) {
	draw_pic_no(state.variables[var]);
}

void load_pic_no(uint8_t pic_no) {
	write_script_entry(SCRIPT_ENTRY_LOAD_PIC, pic_no);

	if (heap_data.loaded_pics[pic_no].buffer)
		return;

	heap_data.loaded_pics[pic_no] = load_vol_data("picdir", pic_no, false);
}

void load_pic(uint8_t var) {
	load_pic_no(state.variables[var]);
}

void overlay_pic_no(uint8_t pic_no) {
	write_script_entry(SCRIPT_ENTRY_OVERLAY_PIC, pic_no);
	_pic_draw(pic_no);
}

void overlay_pic(uint8_t var) {
	overlay_pic_no(state.variables[var]);
}
