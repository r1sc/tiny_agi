#include <stdlib.h>

#include "../state.h"
#include "../actions.h"
#include "../platform_support.h"

#define ANYTHING 0
#define WATER 1
#define LAND 2

#define NORMAL_CYCLE 1
#define REVERSE_CYCLE 2
#define REVERSE_LOOP 3
#define END_OF_LOOP 4

#define OBJ_MOVEMODE_NORMAL 0
#define OBJ_MOVEMODE_MOVE_TO 1
#define OBJ_MOVEMODE_WANDER 2

#pragma pack(push,1)
typedef struct {
	uint8_t width;
	uint8_t height;
	uint8_t transparent_color_and_mirroring;
	uint8_t pixel_data[];
} cell_t;

typedef struct {
	uint8_t num_cells;
	uint16_t cell_offsets[];
} loop_t;

typedef struct {
	uint16_t unk;
	uint8_t num_loops;
	uint16_t description_offset;
	int16_t loop_offsets[];
} view_t;
#pragma pack(pop)

uint8_t _get_pri(int x, int y, bool addToPic) {
	uint8_t pri;
	while ((pri = addToPic ? pic_pri_get(x, y) : priority_get(x, y)) < 3) {
		y++;
		if (y > 167)
			return 0;
	}
	return pri;
}

void _draw_view(uint8_t viewNo, uint8_t loopNo, uint8_t cellNo, uint8_t x, uint8_t y, uint8_t priority, bool erase, bool addToPic) {
	view_t* view = state.loaded_views[viewNo].buffer;
	loop_t* loop = (loop_t*)((uint8_t*)(view)+view->loop_offsets[loopNo]);
	cell_t* cell = (cell_t*)((uint8_t*)(loop)+loop->cell_offsets[cellNo]);
	uint8_t transparentColor = cell->transparent_color_and_mirroring & 0x0F;

	uint8_t mirroring = cell->transparent_color_and_mirroring >> 4;
	bool mirrored = (mirroring & 0x0F) && (loopNo != (mirroring & 7));

	//y += state.play_top;

	uint8_t* pixel_data = cell->pixel_data;
	for (uint8_t sy = 0; sy < cell->height; sy++)
	{
		uint8_t sx = x;
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
					uint8_t screenY = y - cell->height + sy + 1;
					uint8_t screenX = sx + (mirrored ? cell->width - 1 : 0);
					if (screenX < 160 && screenY < 168 && screenX >= 0 && screenY >= 0) {
						if (erase) {
							screen_set_160(screenX, screenY, pic_vis_get(screenX, screenY));
							priority_set(screenX, screenY, pic_pri_get(screenX, screenY));
						}
						else {
							uint8_t bgPri = _get_pri(screenX, screenY, addToPic);
							bool doDraw = priority >= bgPri;
							if (doDraw) {
								screen_set_160(screenX, screenY, color);
								priority_set(screenX, screenY, priority);

								if (addToPic) {
									pic_vis_set(screenX, screenY, color);
									//if (bgPri > 3) {
									pic_pri_set(screenX, screenY, priority);
									//}
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

int _random_between(int min, int maxExclusive) {
	return (rand() % (maxExclusive - min)) + min;
}

void _undraw_all() {
	for (size_t i = 0; i < 16; i++)
	{
		if (state.objects[i].active && state.objects[i].drawn) {
			_draw_view(
				state.objects[i].view_no, 
				state.objects[i].loop_no, 
				state.objects[i].cel_no, 
				state.objects[i].x, 
				state.objects[i].y, 0, true, false);
		}
	}
}

uint8_t _get_priority(uint8_t objNo) {
	if (OBJ.fixed_priority >= 0)
		return OBJ.fixed_priority;

	uint8_t priority = OBJ.y / 12 + 1;
	if (priority < 4)
		priority = 4;
	if (priority > 15)
		priority = 15;
	return priority;
}

bool _obj_baseline_on_pri(int x, int y, int width, uint8_t searchFor) {
	int controlLine = -1;
	int maxX = min(x + width, 160);
	for (int px = x; px < maxX; px++)
	{
		uint8_t pri = priority_get(px, y);
		if (pri == searchFor)
			return true;
	}
	return false;
}

uint8_t _view_num_loops(uint8_t viewNo) {
	view_t* view = state.loaded_views[viewNo].buffer;
	return view->num_loops;
}

uint8_t _view_num_cels(uint8_t viewNo, uint8_t loopNo) {
	view_t* view = state.loaded_views[viewNo].buffer;
	loop_t* loop = (loop_t*)((uint8_t*)(view)+view->loop_offsets[loopNo]);
	return loop->num_cells;
}

int _view_width(uint8_t viewNo, uint8_t loopNo, uint8_t cellNo) {
	view_t* view = (view_t*)state.loaded_views[viewNo].buffer;
	loop_t* loop = (loop_t*)((uint8_t*)(view)+view->loop_offsets[loopNo]);
	cell_t* cell = (cell_t*)((uint8_t*)(loop)+loop->cell_offsets[cellNo]);
	return cell->width;
}

void _set_loop_from_dir(uint8_t objNo, int dir) {
	if (!OBJ.fix_loop) {
		int numLoops = _view_num_loops(OBJ.view_no);

		if (numLoops < 5) {
			if (numLoops > 3) {
				if (dir == DIR_UP)
					OBJ.loop_no = 3;
				else if (dir == DIR_DOWN)
					OBJ.loop_no = 2;
			}
			if (numLoops > 1 && (dir == DIR_LEFT || dir == DIR_DOWN_LEFT || dir == DIR_UP_LEFT))
				OBJ.loop_no = 1;
			if (dir == DIR_RIGHT || dir == DIR_UP_RIGHT || dir == DIR_DOWN_RIGHT)
				OBJ.loop_no = 0;
		}

		int numCels = _view_num_cels(OBJ.view_no, OBJ.loop_no);
		OBJ.cel_no = OBJ.cel_no % numCels;
	}
}

void _set_dir_from_moveDistance(uint8_t objNo) {
	if (OBJ.move_distance_x < 0 && OBJ.move_distance_y < 0) {
		OBJ.direction = DIR_UP_LEFT;
	}
	else if (OBJ.move_distance_x < 0 && OBJ.move_distance_y > 0) {
		OBJ.direction = DIR_DOWN_LEFT;
	}
	else if (OBJ.move_distance_x > 0 && OBJ.move_distance_y < 0) {
		OBJ.direction = DIR_UP_RIGHT;
	}
	else if (OBJ.move_distance_x > 0 && OBJ.move_distance_y > 0) {
		OBJ.direction = DIR_DOWN_RIGHT;
	}
	else if (OBJ.move_distance_x < 0) {
		OBJ.direction = DIR_LEFT;
	}
	else if (OBJ.move_distance_x > 0) {
		OBJ.direction = DIR_RIGHT;
	}
	else if (OBJ.move_distance_y < 0) {
		OBJ.direction = DIR_UP;
	}
	else if (OBJ.move_distance_y > 0) {
		OBJ.direction = DIR_DOWN;
	}
}

inline bool point_in_rect(const int x_obj, const int y_obj, const rect_t rect) {
	return x_obj >= rect.x1 && y_obj >= rect.y1 && x_obj <= rect.x2 && y_obj <= rect.y2;
}

void _update_object(uint8_t objNo) {
	if (objNo == 0) {
		if (state.program_control) {
			state.variables[VAR_6_EGO_DIRECTION] = OBJ.direction;
		}
		else {
			OBJ.direction = state.variables[VAR_6_EGO_DIRECTION];
		}
	}

	if (OBJ.update) {
		int stepSize = OBJ.step_size;
		if (OBJ.move_mode == OBJ_MOVEMODE_MOVE_TO) {
			_set_dir_from_moveDistance(objNo);
			stepSize = OBJ.move_step_size;
		}

		_set_loop_from_dir(objNo, OBJ.direction);

		int width = _view_width(OBJ.view_no, OBJ.loop_no, OBJ.cel_no);

		if (OBJ.steps_to_next_update == 0) {

			if (OBJ.direction != DIR_STOPPED) {
				int newX = OBJ.x;
				int newY = OBJ.y;

				if (OBJ.direction == DIR_UP || OBJ.direction == DIR_UP_LEFT || OBJ.direction == DIR_UP_RIGHT) {
					newY -= stepSize;
					OBJ.move_distance_y += stepSize;
					if (OBJ.move_distance_y > 0)
						OBJ.move_distance_y = 0;
				}
				else if (OBJ.direction == DIR_DOWN || OBJ.direction == DIR_DOWN_LEFT || OBJ.direction == DIR_DOWN_RIGHT) {
					newY += stepSize;
					OBJ.move_distance_y -= stepSize;
					if (OBJ.move_distance_y < 0)
						OBJ.move_distance_y = 0;
				}
				if (OBJ.direction == DIR_LEFT || OBJ.direction == DIR_DOWN_LEFT || OBJ.direction == DIR_UP_LEFT) {
					newX -= stepSize;
					OBJ.move_distance_x += stepSize;
					if (OBJ.move_distance_x > 0)
						OBJ.move_distance_x = 0;
				}
				else if (OBJ.direction == DIR_RIGHT || OBJ.direction == DIR_DOWN_RIGHT || OBJ.direction == DIR_UP_RIGHT) {
					newX += stepSize;
					OBJ.move_distance_x -= stepSize;
					if (OBJ.move_distance_x < 0)
						OBJ.move_distance_x = 0;
				}

				bool unconditionalStop = _obj_baseline_on_pri(newX, newY, width, 0);
				bool conditionalStop = OBJ.collide_with_blocks && (_obj_baseline_on_pri(newX, newY, width, 1) || (state.block_active && point_in_rect(newX, newY, state.block)));
				bool confinedOnWater = (OBJ.allowed_on == WATER) && _obj_baseline_on_pri(newX, newY, width, 3);
				bool confinedOnLand = (OBJ.allowed_on == LAND) && _obj_baseline_on_pri(newX, newY, width, 3);
				bool stop = unconditionalStop || conditionalStop || confinedOnWater || confinedOnLand;

				if (stop) {
					newX = OBJ.x;
					newY = OBJ.y;

					if (OBJ.move_mode == OBJ_MOVEMODE_WANDER) {
						OBJ.direction = _random_between(1, 9);
					}
				}

				if (OBJ.move_mode == OBJ_MOVEMODE_WANDER && OBJ.move_distance_x == 0 && OBJ.move_distance_y == 0) {
					wander(objNo);
				}

				if (OBJ.move_mode == OBJ_MOVEMODE_MOVE_TO && OBJ.move_distance_x == 0 && OBJ.move_distance_y == 0) {
					OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
					OBJ.direction = DIR_STOPPED;
					state.flags[OBJ.move_done_flag] = true;
				}

				if (OBJ.observe_horizon && newY < state.horizon)
					newY = state.horizon;

				if (newY >= 167) {
					state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_BOTTOM;
					if (objNo != 0) {
						state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
					}
				} else if (newY <= 0 || (OBJ.observe_horizon && newY <= state.horizon)) {
					state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_TOP;
					if (objNo != 0) {
						state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
					}
				} else if (newX <= 0) {
					state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_LEFT;
					if (objNo != 0) {
						state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
					}
				} else if (newX >= 160) {
					state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_RIGHT;
					if (objNo != 0) {
						state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
					}
				}

				OBJ.x = newX;
				OBJ.y = newY;
			}

			OBJ.steps_to_next_update = OBJ.step_time;
		}
		OBJ.steps_to_next_update--;

		if (objNo == 0) {
			state.flags[FLAG_3_EGO_TOUCHED_TRIGGER] = _obj_baseline_on_pri(OBJ.x, OBJ.y, width, 2);
			state.flags[FLAG_0_EGO_ON_WATER] = _obj_baseline_on_pri(OBJ.x, OBJ.y, width, 3);
		}

		if (OBJ.is_cycling) {
			if (OBJ.cycles_to_next_update == 0) {
				int numCels = _view_num_cels(OBJ.view_no, OBJ.loop_no);

				if (OBJ.cycling_mode == REVERSE_CYCLE || OBJ.cycling_mode == REVERSE_LOOP) {
					if (OBJ.cel_no <= 0) {
						if (OBJ.cycling_mode == REVERSE_LOOP) {
							if (OBJ.end_of_loop_flag > -1) {
								state.flags[OBJ.end_of_loop_flag] = true;
								OBJ.end_of_loop_flag = -1;
							}
							OBJ.cel_no = 0;
							OBJ.is_cycling = false;
						}
						else {
							OBJ.cel_no = numCels - 1;
						}
					}
					else {
						OBJ.cel_no--;
					}
				}
				else {
					if (OBJ.cel_no >= numCels - 1) {
						if (OBJ.cycling_mode == END_OF_LOOP) {
							if (OBJ.end_of_loop_flag > -1) {
								state.flags[OBJ.end_of_loop_flag] = true;
								OBJ.end_of_loop_flag = -1;
							}
							OBJ.cel_no = numCels - 1;
							OBJ.is_cycling = false;
						}
						else {
							OBJ.cel_no = 0;
						}
					}
					else {
						OBJ.cel_no++;
					}
				}

				OBJ.cycles_to_next_update = OBJ.cycle_time;
			}
			OBJ.cycles_to_next_update--;
		}
	}
}

void _update_all_active() {
	for (uint8_t i = 0; i < 16; i++)
	{
		if (state.objects[i].active && state.objects[i].drawn) {
			_update_object(i);
		}
	}
}

void _draw_all_active() {
	for (int objNo = 15; objNo >= 0; objNo--)
	{
		if (state.objects[objNo].active && state.objects[objNo].drawn) {
			uint8_t priority = _get_priority(objNo);
			_draw_view(OBJ.view_no, OBJ.loop_no, OBJ.cel_no, OBJ.x, OBJ.y, priority, false, false); //draw
		}
	}
}


void add_to_pic(uint8_t viewNo, uint8_t loopNo, uint8_t celNo, uint8_t x, uint8_t y, uint8_t pri, uint8_t margin) {
	_draw_view(viewNo, loopNo, celNo, x, y, pri, false, true);
}

void add_to_pic_v(uint8_t vviewNo, uint8_t vloopNo, uint8_t vcelNo, uint8_t vx, uint8_t vy, uint8_t vpri, uint8_t vmargin) {
	add_to_pic(
		state.variables[vviewNo], 
		state.variables[vloopNo], 
		state.variables[vcelNo], 
		state.variables[vx], 
		state.variables[vy], 
		state.variables[vpri], 
		state.variables[vmargin]);
}

void animate_obj(uint8_t objNo) {
	OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
	OBJ.active = true;
	OBJ.drawn = false;
	OBJ.view_no = 0;
	OBJ.loop_no = 0;
	OBJ.cel_no = 0;
	OBJ.fix_loop = false;
	OBJ.fixed_priority = -1;
	OBJ.update = true;
	OBJ.observe_horizon = false;
	OBJ.allowed_on = ANYTHING;
	OBJ.cycle_time = 1;
	OBJ.cycles_to_next_update = OBJ.cycle_time;
	OBJ.step_time = 1;
	OBJ.steps_to_next_update = OBJ.step_time;
	OBJ.is_cycling = true;
	OBJ.cycling_mode = NORMAL_CYCLE;
	OBJ.collide_with_objects = true;
	OBJ.step_size = 1;
	OBJ.collide_with_blocks = true;
	OBJ.move_distance_x = 0;
	OBJ.move_distance_y = 0;
	OBJ.move_step_size = 0;
	OBJ.move_done_flag = -1;
	OBJ.end_of_loop_flag = -1;
	OBJ.direction = 0;
}

void block(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	state.block_active = true;
	state.block.x1 = x1;
	state.block.y1 = y1;
	state.block.x2 = x2;
	state.block.y2 = y2;
}

void current_cel(uint8_t objNo, uint8_t var) {
	state.variables[var] = OBJ.cel_no;
}

void current_loop(uint8_t objNo, uint8_t var) {
	state.variables[var] = OBJ.loop_no;
}

void current_view(uint8_t objNo, uint8_t var) {
	state.variables[var] = OBJ.view_no;
}

void cycle_time(uint8_t objNo, uint8_t var) {
	OBJ.cycle_time = state.variables[var];
}

void discard_view(uint8_t num) {
	UNIMPLEMENTED
}

void discard_view_v(uint8_t var) {
	UNIMPLEMENTED
}

void distance(uint8_t objNo, uint8_t objNo2, uint8_t var) {
	object_t _OBJ = state.objects[objNo];
	object_t OBJ2 = state.objects[objNo2];
	int deltaX = abs(_OBJ.x - OBJ2.x);
	int deltaY = abs(_OBJ.y - OBJ2.y);
	state.variables[var] = deltaX + deltaY;
}

void draw(uint8_t objNo) {
	OBJ.drawn = true;
}

void end_of_loop(uint8_t objNo, uint8_t flag) {
	state.flags[flag] = false;
	OBJ.end_of_loop_flag = flag;
	OBJ.cycling_mode = END_OF_LOOP;
	OBJ.is_cycling = true;
}

void erase(uint8_t objNo) {
	//_draw_view(OBJ.view_no, OBJ.loop_no, OBJ.cel_no, OBJ.x, OBJ.y, _get_priority(objNo), true, false);
	OBJ.drawn = false;
}

void fix_loop(uint8_t objNo) {
	OBJ.fix_loop = true;
}

void follow_ego(uint8_t objNo, uint8_t stepSize, uint8_t fDoneFlag) {
	UNIMPLEMENTED
}

void force_update(uint8_t objNo) {
	_draw_view(OBJ.view_no, OBJ.loop_no, OBJ.cel_no, OBJ.x, OBJ.y, 0, true, false); //undraw
	_update_object(objNo);
}

void get_dir(uint8_t objNo, uint8_t var) {
	state.variables[var] = OBJ.direction;
}

void get_posn(uint8_t objNo, uint8_t var, uint8_t var2) {
	state.variables[var] = OBJ.x;
	state.variables[var2] = OBJ.y;
}

void get_priority(uint8_t objNo, uint8_t var) {
	state.variables[var] = _get_priority(objNo);
}

void ignore_blocks(uint8_t objNo) {
	OBJ.collide_with_blocks = false;
}

void ignore_horizon(uint8_t objNo) {
	OBJ.observe_horizon = false;
}

void ignore_objs(uint8_t objNo) {
	OBJ.collide_with_objects = false;
}

void last_cel(uint8_t objNo, uint8_t var) {
	state.variables[var] = _view_num_cels(OBJ.view_no, OBJ.loop_no) - 1;
}

void load_view(uint8_t num) {
	if (state.loaded_views[num].buffer)
		return;

	state.loaded_views[num] = load_vol_data("viewdir", num, false);
}

void load_view_v(uint8_t var) {
	load_view(state.variables[var]);
}

void move_obj(uint8_t objNo, uint8_t x, uint8_t y, uint8_t stepSize, uint8_t fDoneFlag) {
	OBJ.move_mode = OBJ_MOVEMODE_MOVE_TO;
	OBJ.move_distance_x = x - OBJ.x;
	OBJ.move_distance_y = y - OBJ.y;
	OBJ.move_step_size = (stepSize == 0) ? OBJ.step_size : stepSize;
	OBJ.move_done_flag = fDoneFlag;
}

void move_obj_v(uint8_t objNo, uint8_t vx, uint8_t vy, uint8_t stepSize, uint8_t fDoneFlag) {
	move_obj(objNo, state.variables[vx], state.variables[vy], state.variables[stepSize], fDoneFlag);
}

void normal_cycle(uint8_t objNo) {
	OBJ.cycling_mode = NORMAL_CYCLE;
	OBJ.is_cycling = true;
}

void normal_motion(uint8_t objNo) {
	OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
}

void number_of_loops(uint8_t objNo, uint8_t var) {
	state.variables[var] = _view_num_loops(OBJ.view_no);
}

void object_on_anything(uint8_t objNo) {
	OBJ.allowed_on = ANYTHING;
}

void object_on_land(uint8_t objNo) {
	OBJ.allowed_on = LAND;
}

void object_on_water(uint8_t objNo) {
	OBJ.allowed_on = WATER;
}

void observe_blocks(uint8_t objNo) {
	OBJ.collide_with_blocks = true;
}

void observe_horizon(uint8_t objNo) {
	OBJ.observe_horizon = true;
}

void observe_objs(uint8_t objNo) {
	OBJ.collide_with_objects = true;
}

void position(uint8_t objNo, uint8_t x, uint8_t y) {
	OBJ.x = x;
	OBJ.y = y;
}

void position_v(uint8_t objNo, uint8_t var, uint8_t var2) {
	position(objNo, state.variables[var], state.variables[var2]);
}

void release_loop(uint8_t objNo) {
	OBJ.fix_loop = false;
}

void release_priority(uint8_t objNo) {
	OBJ.fixed_priority = -1;
}

void reposition(uint8_t objNo, uint8_t var, uint8_t var2) {
	int8_t dx = state.variables[var];
	int8_t dy = state.variables[var2];
	OBJ.x += dx;
	OBJ.y += dy;
}

void reposition_to(uint8_t objNo, uint8_t x, uint8_t y) {
	erase(objNo);
	position(objNo, x, y);
	draw(objNo);
}

void reposition_to_v(uint8_t objNo, uint8_t vx, uint8_t vy) {
	reposition_to(objNo, state.variables[vx], state.variables[vy]);
}

void reverse_cycle(uint8_t objNo) {
	OBJ.cycling_mode = REVERSE_CYCLE;
	OBJ.is_cycling = true;
}

void reverse_loop(uint8_t objNo, uint8_t flag) {
	OBJ.end_of_loop_flag = flag;
	state.flags[flag] = false;
	OBJ.cycling_mode = REVERSE_LOOP;
	OBJ.is_cycling = true;
}

void set_cel(uint8_t objNo, uint8_t num) {
	OBJ.cel_no = num;
}

void set_cel_v(uint8_t objNo, uint8_t var) {
	set_cel(objNo, state.variables[var]);
}

void set_dir(uint8_t objNo, uint8_t var) {
	OBJ.direction = state.variables[var];
}

void set_horizon(uint8_t num) {
	state.horizon = num;
}

void set_loop(uint8_t objNo, uint8_t num) {
	OBJ.loop_no = num;
	OBJ.cel_no = 0;
}

void set_loop_v(uint8_t objNo, uint8_t var) {
	set_loop(objNo, state.variables[var]);
}

void set_priority(uint8_t objNo, uint8_t num) {
	OBJ.fixed_priority = num;
}

void set_priority_v(uint8_t objNo, uint8_t var) {
	set_priority(objNo, state.variables[var]);
}

void set_upper_left(uint8_t num, uint8_t num2) {
	UNIMPLEMENTED
}

void set_view(uint8_t objNo, uint8_t viewNo) {
	OBJ.view_no = viewNo;
	OBJ.cel_no = 0;
	OBJ.loop_no = 0;
	OBJ.cycling_mode = NORMAL_CYCLE;
}

void set_view_v(uint8_t objNo, uint8_t var) {
	set_view(objNo, state.variables[var]);
}

void start_cycling(uint8_t objNo) {
	OBJ.is_cycling = true;
}

void start_motion(uint8_t objNo) {
	if (objNo == 0)
		player_control();
}

void start_update(uint8_t objNo) {
	OBJ.update = true;
}

void step_size(uint8_t objNo, uint8_t var) {
	OBJ.step_size = state.variables[var];
}

void step_time(uint8_t objNo, uint8_t var) {
	OBJ.step_time = state.variables[var];
}

void stop_cycling(uint8_t objNo) {
	OBJ.is_cycling = false;
}

void stop_motion(uint8_t objNo) {
	if (objNo == 0)
		program_control();
	OBJ.direction = DIR_STOPPED;
}

void stop_update(uint8_t objNo) {
	OBJ.update = false;
}

void unanimate_all() {
	for (size_t i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		state.objects[i].active = false;
	}
}

void unblock() {
	state.block_active = false;
}

void wander(uint8_t objNo) {
	if (objNo == 0) {
		program_control();
	}
	OBJ.move_mode = OBJ_MOVEMODE_WANDER;
	OBJ.move_distance_x = _random_between(6, 51);
	OBJ.move_distance_y = _random_between(6, 51);
	_set_dir_from_moveDistance(objNo);
}