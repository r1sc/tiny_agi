#include "view.h"
#include "state.h"
#include "heap.h"
#include "platform_support.h"
#include "actions.h"

uint8_t _view_num_loops(uint8_t view_no) {
	view_t* view = (view_t*)(heap_data.loaded_views[view_no].buffer);
	return view->num_loops;
}

uint8_t _view_num_cels(uint8_t view_no, uint8_t loop_no) {
	view_t* view = (view_t*)(heap_data.loaded_views[view_no].buffer);
	loop_t* loop = (loop_t*)((uint8_t*)(view)+view->loop_offsets[loop_no]);
	return loop->num_cells;
}

cell_t* _get_cell(uint8_t view_no, uint8_t loop_no, uint8_t cel_no) {
	view_t* view = (view_t*)(heap_data.loaded_views[view_no].buffer);
	loop_t* loop = (loop_t*)((uint8_t*)(view)+view->loop_offsets[loop_no]);
	cell_t* cell = (cell_t*)((uint8_t*)(loop)+loop->cell_offsets[cel_no]);
	return cell;
}

cell_t* _object_cell(const object_t* obj) {
	return _get_cell(obj->view_no, obj->loop_no, obj->cel_no);
}

uint8_t _get_pri(int x, int y, bool add_to_pic) {
	uint8_t pri;
	while ((pri = add_to_pic ? pic_pri_get(x, y) : priority_get(x, y)) < 3) {
		y++;
		if (y > 167)
			return 0;
	}
	return pri;
}

void _draw_view(uint8_t view_no, uint8_t loop_no, uint8_t cel_no, uint8_t x, uint8_t y, uint8_t priority, bool erase, bool add_to_pic) {
	view_t* view = (view_t*)(heap_data.loaded_views[view_no].buffer);
	loop_t* loop = (loop_t*)((uint8_t*)(view)+view->loop_offsets[loop_no]);
	cell_t* cell = (cell_t*)((uint8_t*)(loop)+loop->cell_offsets[cel_no]);
	uint8_t transparentColor = cell->transparent_color_and_mirroring & 0x0F;

	uint8_t mirroring = cell->transparent_color_and_mirroring >> 4;
	bool mirrored = (mirroring & 0x0F) && (loop_no != (mirroring & 7));

	//y += state.play_top;

	uint8_t* pixel_data = cell->pixel_data;
	for (uint8_t sy = 0; sy < cell->height; sy++)
	{
		uint8_t sx = mirrored ? x + cell->width - 1 : x;
		while (1) {
			uint8_t chunk = *(pixel_data++);
			if (chunk == 0)
				break;
			uint8_t color = chunk >> 4;
			uint8_t repeat = chunk & 0x0F;

			if (color == transparentColor)
			{
				if (mirrored)
					sx -= repeat;
				else
					sx += repeat;
			}
			else {
				for (size_t i = 0; i < repeat; i++)
				{
					uint8_t screenY = y - (cell->height - 1) + sy;
					uint8_t screenX = sx; // + (mirrored ? cell->width - 1 : 0);
					if (screenX < 160 && screenY < 168 && screenX >= 0 && screenY >= 0) {
						if (erase) {
							screen_set_160(screenX, screenY, pic_vis_get(screenX, screenY));
							priority_set(screenX, screenY, pic_pri_get(screenX, screenY));
						}
						else {
							uint8_t bgPri = _get_pri(screenX, screenY, add_to_pic);
							bool doDraw = priority >= bgPri;
							if (doDraw) {
								screen_set_160(screenX, screenY, color);
								priority_set(screenX, screenY, priority);
								
								if (add_to_pic) {
									pic_vis_set(screenX, screenY, color);
									if (pic_pri_get(screenX, screenY) >= 3) {
										pic_pri_set(screenX, screenY, priority);
									}
								}
							}
						}
					}
					if (mirrored)
						sx--;
					else
						sx++;
				}
			}
		}
	}
}