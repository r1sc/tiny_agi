#include <stdlib.h>

#include "../state.h"
#include "../heap.h"
#include "../actions.h"
#include "../platform_support.h"
#include "../view.h"
#include "../constants.h"

#pragma region Utils
static int random_between(int min, int maxExclusive) {
	return (rand() % (maxExclusive - min)) + min;
}

static uint8_t y_to_priority(uint8_t y) {
	uint8_t priority = y / 12 + 1;
	if (priority < 4)
		priority = 4;
	if (priority > 15)
		priority = 15;
	return priority;
}

static uint8_t calculate_priority(uint8_t objNo) {
	object_t* obj = &state.objects[objNo];
	if (obj->has_fixed_priority)
		return obj->fixed_priority;

	return y_to_priority(obj->y);
}

#define MIN(x, y) (x < y ? x : y)
static bool obj_baseline_on_pri(int x, int y, int width, bool check_entire_baseline, uint8_t searchFor) {
	if (check_entire_baseline) {
		// Check entire baseline
		int maxX = MIN(x + width, 160);
		for (int px = x; px < maxX; px++) {
			uint8_t pri = pic_pri_get(px, y);
			if (pri == searchFor)
				return true;
		}
		return false;
	}

	// Just check target pixel
	uint8_t pri = pic_pri_get(x, y);
	return pri == searchFor;
}

static void set_loop_from_dir(uint8_t objNo, int dir) {
	object_t* obj = &state.objects[objNo];
	if (!obj->fix_loop) {
		int numLoops = _view_num_loops(obj->view_no);

		if (numLoops < 5) {
			if (numLoops > 3) {
				if (dir == DIR_UP)
					obj->loop_no = 3;
				else if (dir == DIR_DOWN)
					obj->loop_no = 2;
			}
			if (numLoops > 1 && (dir == DIR_LEFT || dir == DIR_DOWN_LEFT || dir == DIR_UP_LEFT))
				obj->loop_no = 1;
			if (dir == DIR_RIGHT || dir == DIR_UP_RIGHT || dir == DIR_DOWN_RIGHT)
				obj->loop_no = 0;
		}

		int numCels = _view_num_cels(obj->view_no, obj->loop_no);
		obj->cel_no = obj->cel_no % numCels;
	}
}


static inline bool point_on_block(const int x_obj, const int y_obj, const rect_t rect) {
	return
		((y_obj >= rect.y1 && y_obj <= rect.y2) && (x_obj == rect.x1 || x_obj == rect.x2)) ||
		((x_obj >= rect.x1 && x_obj <= rect.x2) && (y_obj == rect.y1 || y_obj == rect.y2));
}

static void update_object(uint8_t objNo) {
	object_t *obj = &state.objects[objNo];
	if (obj->active && obj->update && obj->drawn) {
		int stepSize = obj->step_size;

		if (obj->move_mode == OBJ_MOVEMODE_MOVE_TO) {
			set_dir_from_move_distance(objNo);
			stepSize = obj->move_step_size;
		}

		set_loop_from_dir(objNo, obj->direction);

		cell_t* cell = _object_cell(obj);

		if (!obj->ignore_horizon && state.flags[FLAG_5_ROOM_EXECUTED_FIRST_TIME] && obj->y <= state.horizon)
			obj->y = state.horizon + 1;

		if (obj->steps_to_next_update == 0) {

			int newX = obj->x;
			int newY = obj->y;

			if (obj->move_mode == OBJ_MOVEMODE_WANDER) {
				obj->wander_distance--;
				if (obj->wander_distance == 0 || obj->direction == DIR_STOPPED) {
					obj->direction = random_between(1, 9);
					obj->wander_distance = random_between(6, 51);
				}
			}

			if (obj->direction == DIR_UP || obj->direction == DIR_UP_LEFT || obj->direction == DIR_UP_RIGHT) {
				newY -= stepSize;
				obj->move_distance_y += stepSize;
				if (obj->move_distance_y > 0)
					obj->move_distance_y = 0;
			} else if (obj->direction == DIR_DOWN || obj->direction == DIR_DOWN_LEFT || obj->direction == DIR_DOWN_RIGHT) {
				newY += stepSize;
				obj->move_distance_y -= stepSize;
				if (obj->move_distance_y < 0)
					obj->move_distance_y = 0;
			}
			if (obj->direction == DIR_LEFT || obj->direction == DIR_DOWN_LEFT || obj->direction == DIR_UP_LEFT) {
				newX -= stepSize;
				obj->move_distance_x += stepSize;
				if (obj->move_distance_x > 0)
					obj->move_distance_x = 0;
			} else if (obj->direction == DIR_RIGHT || obj->direction == DIR_DOWN_RIGHT || obj->direction == DIR_UP_RIGHT) {
				newX += stepSize;
				obj->move_distance_x -= stepSize;
				if (obj->move_distance_x < 0)
					obj->move_distance_x = 0;
			}

			bool check_entire_baseline = obj->y != newY;
			int check_x = MIN(159, (check_entire_baseline ? newX : newX > obj->x ? obj->x + cell->width : newX));

			bool unconditionalStop = obj_baseline_on_pri(check_x, newY, cell->width, check_entire_baseline, 0);
			bool conditionalStop = !obj->ignore_blocks && (obj_baseline_on_pri(check_x, newY, cell->width, check_entire_baseline, 1) || (state.block_active && point_on_block(newX, newY, state.block)));
			bool confinedOnWater = (obj->allowed_on == OBJ_ON_WATER) && !obj_baseline_on_pri(check_x, newY, cell->width, check_entire_baseline, 3);
			bool confinedOnLand = (obj->allowed_on == OBJ_ON_LAND) && obj_baseline_on_pri(check_x, newY, cell->width, check_entire_baseline, 3);
			bool stop = unconditionalStop || conditionalStop || confinedOnWater || confinedOnLand;

			if (obj->move_mode == OBJ_MOVEMODE_WANDER && stop) {
				obj->direction = random_between(1, 9);
			}

			if (!obj->ignore_objects) {
				for (size_t i = 0; i < MAX_NUM_OBJECTS; i++) {
					object_t other_object = state.objects[i];
					if (i != objNo && other_object.active && other_object.drawn && !other_object.ignore_objects) {
						if (newY == other_object.y
							&& newX + cell->width > other_object.x
							&& newX < other_object.x + _object_cell(&other_object)->width
							) {
							stop = true;
						}
					}
				}
			}

			if (obj->move_mode == OBJ_MOVEMODE_MOVE_TO && obj->move_distance_x == 0 && obj->move_distance_y == 0) {
				obj->move_mode = OBJ_MOVEMODE_NORMAL;
				obj->direction = DIR_STOPPED;
				state.flags[obj->move_done_flag] = true;
			}

			if (newY > 167) {
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_BOTTOM;
				if (objNo != 0) {
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			} else if (newY <= 0 || (!obj->ignore_horizon && newY <= state.horizon)) {
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_TOP;
				if (objNo != 0) {
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			} else if (newX <= 0) {
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_LEFT;
				if (objNo != 0) {
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			} else if (newX + cell->width >= 160) {
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_RIGHT;
				if (objNo != 0) {
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			}

			if (stop) {
				newX = obj->x;
				newY = obj->y;

				if (obj->move_mode == OBJ_MOVEMODE_WANDER) {
					wander(objNo);
				}
			}

			obj->x = newX;
			obj->y = newY;

			obj->steps_to_next_update = obj->step_time;
		}
		obj->steps_to_next_update--;

		if (objNo == 0) {
			state.flags[FLAG_3_EGO_TOUCHED_TRIGGER] = obj_baseline_on_pri(obj->x, obj->y, cell->width, true, 2);
			state.flags[FLAG_0_EGO_ON_WATER] = obj_baseline_on_pri(obj->x, obj->y, cell->width, true, 3);
		}

		if (obj->is_cycling) {
			if (obj->cycles_to_next_update == 0) {
				int numCels = _view_num_cels(obj->view_no, obj->loop_no);

				if (obj->cycling_mode == CYCLE_MODE_REVERSE_CYCLE || obj->cycling_mode == CYCLE_MODE_REVERSE_LOOP) {
					if (obj->cel_no <= 0) {
						if (obj->cycling_mode == CYCLE_MODE_REVERSE_LOOP) {
							if (obj->end_of_loop_flag > -1) {
								state.flags[obj->end_of_loop_flag] = true;
								obj->end_of_loop_flag = -1;
							}
							obj->cel_no = 0;
							obj->is_cycling = false;
							obj->direction = DIR_STOPPED;
							obj->cycling_mode = CYCLE_MODE_NORMAL;
						} else {
							obj->cel_no = numCels - 1;
						}
					} else {
						obj->cel_no--;
					}
				} else {
					if (obj->cel_no >= numCels - 1) {
						if (obj->cycling_mode == CYCLE_MODE_END_OF_LOOP) {
							if (obj->end_of_loop_flag > -1) {
								state.flags[obj->end_of_loop_flag] = true;
								obj->end_of_loop_flag = -1;
							}
							obj->cel_no = numCels - 1;
							obj->is_cycling = false;
							obj->direction = DIR_STOPPED;
							obj->cycling_mode = CYCLE_MODE_NORMAL;
						} else {
							obj->cel_no = 0;
						}
					} else {
						obj->cel_no++;
					}
				}

				obj->cycles_to_next_update = obj->cycle_time;
			}
			obj->cycles_to_next_update--;
		}
	}
}

void update_all_active() {
	for (uint8_t i = 0; i < MAX_NUM_OBJECTS; i++) {
		if (state.objects[i].active && state.objects[i].drawn) {
			update_object(i);
		}
	}
}

static uint8_t pri_2_coord(uint8_t pri) {
	return (48 + 12 * (pri - 4 - 1));
}

static int cmpfunc(const void* a, const void* b) {
	object_t* obj_a = &state.objects[*(uint8_t*)a];
	object_t* obj_b = &state.objects[*(uint8_t*)b];
	uint8_t pri_a = obj_a->has_fixed_priority ? pri_2_coord(obj_a->fixed_priority) : obj_a->y;
	uint8_t pri_b = obj_b->has_fixed_priority ? pri_2_coord(obj_b->fixed_priority) : obj_b->y;
	return (pri_a - pri_b);
}


void agi_draw_all_active() {
	uint8_t objs_sorted[MAX_NUM_OBJECTS];
	int num_sorted = 0;

	for (int objNo = MAX_NUM_OBJECTS - 1; objNo >= 0; objNo--) {
		object_t* obj = &state.objects[objNo];
		if (obj->active && obj->drawn) {
			if (obj->old_view_no > -1) {
				_draw_view(obj->old_view_no, obj->old_loop_no, obj->old_cel_no, obj->old_x, obj->old_y, 0, true, false); //erase
				obj->old_view_no = -1;
			}
			objs_sorted[num_sorted++] = objNo;
		}
	}

	qsort(objs_sorted, num_sorted, sizeof(uint8_t), cmpfunc);

	for (int i = 0; i < num_sorted; i++) {
		int objNo = objs_sorted[i];
		object_t* obj = &state.objects[objNo];
		uint8_t priority = calculate_priority(objNo);

		_draw_view(obj->view_no, obj->loop_no, obj->cel_no, obj->x, obj->y, priority, false, false); //draw

		obj->old_view_no = obj->view_no;
		obj->old_loop_no = obj->loop_no;
		obj->old_cel_no = obj->cel_no;
		obj->old_x = obj->x;
		obj->old_y = obj->y;
	}
}
#pragma endregion

void add_to_pic(uint8_t viewNo, uint8_t loopNo, uint8_t celNo, uint8_t x, uint8_t y, uint8_t pri, uint8_t margin) {
	heap_write_add_to_pic_script_entry(viewNo, loopNo, celNo, x, y, pri);

	_draw_view(viewNo, loopNo, celNo, x, y, pri, false, true);
	if (margin < 4) {
		int width = _get_cell(viewNo, loopNo, celNo)->width;
		uint8_t start_priority = y_to_priority(y);
		uint8_t ym;
		for (ym = y; y_to_priority(ym) == start_priority; ym--) {
			pic_pri_set(x, ym, margin);
			pic_pri_set(x + width, ym, margin);
		}
		for (int xm = 0; xm < width; xm++) {
			pic_pri_set(x + xm, y, margin);
			pic_pri_set(x + xm, ym + 1, margin);
		}
	}
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
	object_t* obj = &state.objects[objNo];
	if (obj->active)
		return;

	obj->active = true;
	obj->update = true;
	obj->is_cycling = true;
	obj->move_mode = OBJ_MOVEMODE_NORMAL;
	obj->cycling_mode = CYCLE_MODE_NORMAL;
	obj->direction = DIR_STOPPED;

	obj->drawn = false;
	obj->has_fixed_priority = false;
	obj->ignore_horizon = false;
	obj->allowed_on = OBJ_ON_ANYTHING;
	obj->ignore_objects = false;
	obj->ignore_blocks = false;
	obj->fix_loop = false;
}

void block(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	state.block_active = true;
	state.block.x1 = x1;
	state.block.y1 = y1;
	state.block.x2 = x2;
	state.block.y2 = y2;
}

void current_cel(uint8_t objNo, uint8_t var) {
	state.variables[var] = state.objects[objNo].cel_no;
}

void current_loop(uint8_t objNo, uint8_t var) {
	state.variables[var] = state.objects[objNo].loop_no;
}

void current_view(uint8_t objNo, uint8_t var) {
	state.variables[var] = state.objects[objNo].view_no;
}

void cycle_time(uint8_t objNo, uint8_t var) {
	state.objects[objNo].cycle_time = state.variables[var];
}

void discard_view(uint8_t num) {
    //heap_write_script_entry(SCRIPT_ENTRY_DISCARD_VIEW, num);
	if (!discard_vol_data(&(heap_data.loaded_views[num]))) {
		panic("discard_view: view %d not loaded!", num);
	}
}

void discard_view_v(uint8_t var) {
	discard_view(state.variables[var]);
}

void distance(uint8_t objNo, uint8_t objNo2, uint8_t var) {
	object_t* obj = &state.objects[objNo];
	object_t* obj2 = &state.objects[objNo2];

	if (!obj->active || !obj->drawn || !obj2->active || !obj2->drawn) {
		state.variables[var] = 255;
		return;
	}

	cell_t* cel1 = _object_cell(obj);
	cell_t* cel2 = _object_cell(obj2);
	state.variables[var] = abs(obj->y - obj2->y) + abs((obj->x + (cel1->width >> 1)) - (obj2->x + (cel2->width >> 1)));
}

void draw(uint8_t objNo) {
	state.objects[objNo].drawn = true;
}

void end_of_loop(uint8_t objNo, uint8_t flag) {
	object_t* obj = &state.objects[objNo];

	state.flags[flag] = false;

	obj->end_of_loop_flag = flag;
	obj->cycling_mode = CYCLE_MODE_END_OF_LOOP;
	obj->update = true;
	obj->is_cycling = true;
}

void erase(uint8_t objNo) {
	object_t* obj = &state.objects[objNo];

	if (obj->old_view_no > -1) {
		_draw_view(obj->old_view_no, obj->old_loop_no, obj->old_cel_no, obj->old_x, obj->old_y, 0, true, false);
	}
	obj->drawn = false;
	obj->old_view_no = -1;
}

void fix_loop(uint8_t objNo) {
	state.objects[objNo].fix_loop = true;
}

void follow_ego(uint8_t objNo, uint8_t stepSize, uint8_t fDoneFlag) {
	UNIMPLEMENTED
}

void force_update(uint8_t objNo) {
	object_t* obj = &state.objects[objNo];

	_draw_view(obj->view_no, obj->loop_no, obj->cel_no, obj->x, obj->y, 0, true, false); //undraw
	update_object(objNo);
}

void get_dir(uint8_t objNo, uint8_t var) {
	state.variables[var] = state.objects[objNo].direction;
}

void get_posn(uint8_t objNo, uint8_t var, uint8_t var2) {
	object_t* obj = &state.objects[objNo];

	state.variables[var] = obj->x;
	state.variables[var2] = obj->y;
}

void get_priority(uint8_t objNo, uint8_t var) {
	state.variables[var] = calculate_priority(objNo);
}

void ignore_blocks(uint8_t objNo) {
	state.objects[objNo].ignore_blocks = true;
}

void ignore_horizon(uint8_t objNo) {
	state.objects[objNo].ignore_horizon = true;
}

void ignore_objs(uint8_t objNo) {
	state.objects[objNo].ignore_objects = true;
}

void last_cel(uint8_t objNo, uint8_t var) {
	object_t* obj = &state.objects[objNo];

	state.variables[var] = _view_num_cels(obj->view_no, obj->loop_no) - 1;
}

void load_view(uint8_t num) {
	heap_write_script_entry(SCRIPT_ENTRY_LOAD_VIEW, num);

	if (heap_data.loaded_views[num].buffer)
		return;

	heap_data.loaded_views[num] = load_vol_data("viewdir", num, false);
}

void load_view_v(uint8_t var) {
	load_view(state.variables[var]);
}

void move_obj(uint8_t objNo, uint8_t x, uint8_t y, uint8_t stepSize, uint8_t fDoneFlag) {
	object_t* obj = &state.objects[objNo];

	if (obj->x == x && obj->y == y) {
		set(fDoneFlag);
		return;
	}

	reset(fDoneFlag);
	obj->move_mode = OBJ_MOVEMODE_MOVE_TO;
	obj->move_distance_x = x - obj->x;
	obj->move_distance_y = y - obj->y;
	obj->move_step_size = stepSize == 0 ? obj->step_size : stepSize;
	obj->move_done_flag = fDoneFlag;

	if (objNo == 0) {
		program_control();
	}
}

void move_obj_v(uint8_t objNo, uint8_t vx, uint8_t vy, uint8_t stepSize, uint8_t fDoneFlag) {
	move_obj(objNo, state.variables[vx], state.variables[vy], state.variables[stepSize], fDoneFlag);
}

void normal_cycle(uint8_t objNo) {
	object_t* obj = &state.objects[objNo];

	obj->cycling_mode = CYCLE_MODE_NORMAL;
	obj->is_cycling = true;
}

void normal_motion(uint8_t objNo) {
	state.objects[objNo].move_mode = OBJ_MOVEMODE_NORMAL;
}

void number_of_loops(uint8_t objNo, uint8_t var) {
	state.variables[var] = _view_num_loops(state.objects[objNo].view_no);
}

void object_on_anything(uint8_t objNo) {
	state.objects[objNo].allowed_on = OBJ_ON_ANYTHING;
}

void object_on_land(uint8_t objNo) {
	state.objects[objNo].allowed_on = OBJ_ON_LAND;
}

void object_on_water(uint8_t objNo) {
	state.objects[objNo].allowed_on = OBJ_ON_WATER;
}

void observe_blocks(uint8_t objNo) {
	state.objects[objNo].ignore_blocks = false;
}

void observe_horizon(uint8_t objNo) {
	state.objects[objNo].ignore_horizon = false;
}

void observe_objs(uint8_t objNo) {
	state.objects[objNo].ignore_objects = false;
}

void position(uint8_t objNo, uint8_t x, uint8_t y) {
	object_t* obj = &state.objects[objNo];

	obj->x = x;
	obj->y = y;
}

void position_v(uint8_t objNo, uint8_t var, uint8_t var2) {
	position(objNo, state.variables[var], state.variables[var2]);
}

void release_loop(uint8_t objNo) {
	state.objects[objNo].fix_loop = false;
}

void release_priority(uint8_t objNo) {
	state.objects[objNo].has_fixed_priority = false;
}

void reposition(uint8_t objNo, uint8_t var, uint8_t var2) {
	object_t* obj = &state.objects[objNo];

	int8_t dx = state.variables[var];
	int8_t dy = state.variables[var2];
	obj->x += dx;
	obj->y += dy;
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
	object_t* obj = &state.objects[objNo];

	obj->cycling_mode = CYCLE_MODE_REVERSE_CYCLE;
	obj->is_cycling = true;
}

void reverse_loop(uint8_t objNo, uint8_t flag) {
	object_t* obj = &state.objects[objNo];

	state.flags[flag] = false;

	obj->end_of_loop_flag = flag;
	obj->cycling_mode = CYCLE_MODE_REVERSE_LOOP;
	obj->is_cycling = true;
	obj->update = true;
}

void set_cel(uint8_t objNo, uint8_t num) {
	state.objects[objNo].cel_no = num;
}

void set_cel_v(uint8_t objNo, uint8_t var) {
	set_cel(objNo, state.variables[var]);
}

void set_dir(uint8_t objNo, uint8_t var) {
	state.objects[objNo].direction = state.variables[var];
}

void set_horizon(uint8_t num) {
	state.horizon = num;
}

void set_loop(uint8_t objNo, uint8_t num) {
	object_t* obj = &state.objects[objNo];

	obj->loop_no = num;
	obj->cel_no = 0;
}

void set_loop_v(uint8_t objNo, uint8_t var) {
	set_loop(objNo, state.variables[var]);
}

void set_priority(uint8_t objNo, uint8_t num) {
	object_t* obj = &state.objects[objNo];
	obj->has_fixed_priority = true;
	obj->fixed_priority = num;
}

void set_priority_v(uint8_t objNo, uint8_t var) {
	set_priority(objNo, state.variables[var]);
}

void set_upper_left(uint8_t num, uint8_t num2) {
	UNIMPLEMENTED
}

void set_view(uint8_t objNo, uint8_t viewNo) {
	object_t* obj = &state.objects[objNo];

	obj->view_no = viewNo;
	uint8_t num_loops = _view_num_loops(viewNo);
	if (obj->loop_no >= num_loops) {
		obj->loop_no = 0;
	}

	uint8_t num_cels = _view_num_cels(viewNo, obj->loop_no);
	if (obj->cel_no >= num_cels) {
		obj->cel_no = 0;
	}

	obj->cycling_mode = CYCLE_MODE_NORMAL;
}

void set_view_v(uint8_t objNo, uint8_t var) {
	set_view(objNo, state.variables[var]);
}

void start_cycling(uint8_t objNo) {
	state.objects[objNo].is_cycling = true;
}

void start_motion(uint8_t objNo) {
	if (objNo == 0) {
		player_control();
		state.variables[VAR_6_EGO_DIRECTION] = DIR_STOPPED;
	}
	normal_motion(objNo);
}

void start_update(uint8_t objNo) {
	state.objects[objNo].update = true;
}

void step_size(uint8_t objNo, uint8_t var) {
	state.objects[objNo].step_size = state.variables[var];
}

void step_time(uint8_t objNo, uint8_t var) {
	state.objects[objNo].step_time = state.variables[var];
}

void stop_cycling(uint8_t objNo) {
	state.objects[objNo].is_cycling = false;
}

void stop_motion(uint8_t objNo) {
	state.objects[objNo].direction = DIR_STOPPED;
	if (objNo == 0) {
		program_control();
		state.variables[VAR_6_EGO_DIRECTION] = DIR_STOPPED;
	}
}

void stop_update(uint8_t objNo) {
	state.objects[objNo].update = false;
}

void unanimate_all() {
	for (size_t i = 0; i < MAX_NUM_OBJECTS; i++) {
		state.objects[i].active = false;
		state.objects[i].drawn = false;
	}
}

void unblock() {
	state.block_active = false;
}

void wander(uint8_t objNo) {
	if (objNo == 0) {
		program_control();
	}

	state.objects[objNo].move_mode = OBJ_MOVEMODE_WANDER;
}