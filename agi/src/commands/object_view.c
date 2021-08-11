#include <stdlib.h>

#include "../state.h"
#include "../actions.h"
#include "../platform_support.h"
#include "../view.h"
#include "../constants.h"

int _random_between(int min, int maxExclusive)
{
	return (rand() % (maxExclusive - min)) + min;
}

uint8_t _get_priority(uint8_t objNo)
{
	if (OBJ.fixed_priority >= 0)
		return OBJ.fixed_priority;

	uint8_t priority = OBJ.y / 12 + 1;
	if (priority < 4)
		priority = 4;
	if (priority > 15)
		priority = 15;
	return priority;
}

#define MIN(x, y) (x < y ? x : y)
bool _obj_baseline_on_pri(int x, int y, int width, uint8_t searchFor)
{
	int controlLine = -1;
	int maxX = MIN(x + width, 160);
	for (int px = x; px < maxX; px++)
	{
		uint8_t pri = pic_pri_get(px, y);
		if (pri == searchFor)
			return true;
	}
	return false;
}

void _set_loop_from_dir(uint8_t objNo, int dir)
{
	if (!OBJ.fix_loop)
	{
		int numLoops = _view_num_loops(OBJ.view_no);

		if (numLoops < 5)
		{
			if (numLoops > 3)
			{
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


inline bool point_in_rect(const int x_obj, const int y_obj, const rect_t rect)
{
	return x_obj >= rect.x1 && y_obj >= rect.y1 && x_obj <= rect.x2 && y_obj <= rect.y2;
}

void _update_object(uint8_t objNo)
{
	if (OBJ.update)
	{
		int stepSize_x = OBJ.step_size;
		int stepSize_y = OBJ.step_size;

		if (OBJ.move_mode == OBJ_MOVEMODE_MOVE_TO)
		{
			stepSize_x = OBJ.move_step_size;
			stepSize_y = OBJ.move_step_size;
		}

		_set_loop_from_dir(objNo, OBJ.direction);

		int width = _view_width(OBJ.view_no, OBJ.loop_no, OBJ.cel_no);

		if (OBJ.observe_horizon && state.flags[FLAG_5_ROOM_EXECUTED_FIRST_TIME] && OBJ.y <= state.horizon)
			OBJ.y = state.horizon + 1;

		if (OBJ.steps_to_next_update == 0)
		{

			//if (OBJ.direction != DIR_STOPPED) {
			int newX = OBJ.x;
			int newY = OBJ.y;

			for (size_t xx = 0; xx < stepSize_x; xx++)
			{

				if (OBJ.direction == DIR_UP || OBJ.direction == DIR_UP_LEFT || OBJ.direction == DIR_UP_RIGHT)
				{
					newY -= 1;
					OBJ.move_distance_y += 1;
					if (OBJ.move_distance_y > 0)
						OBJ.move_distance_y = 0;
				}
				else if (OBJ.direction == DIR_DOWN || OBJ.direction == DIR_DOWN_LEFT || OBJ.direction == DIR_DOWN_RIGHT)
				{
					newY += 1;
					OBJ.move_distance_y -= 1;
					if (OBJ.move_distance_y < 0)
						OBJ.move_distance_y = 0;
				}
				if (OBJ.direction == DIR_LEFT || OBJ.direction == DIR_DOWN_LEFT || OBJ.direction == DIR_UP_LEFT)
				{
					newX -= 1;
					OBJ.move_distance_x += 1;
					if (OBJ.move_distance_x > 0)
						OBJ.move_distance_x = 0;
				}
				else if (OBJ.direction == DIR_RIGHT || OBJ.direction == DIR_DOWN_RIGHT || OBJ.direction == DIR_UP_RIGHT)
				{
					newX += 1;
					OBJ.move_distance_x -= 1;
					if (OBJ.move_distance_x < 0)
						OBJ.move_distance_x = 0;
				}

				bool unconditionalStop = _obj_baseline_on_pri(newX, newY, width, 0);
				bool conditionalStop = OBJ.collide_with_blocks && (_obj_baseline_on_pri(newX, newY, width, 1) || (state.block_active && point_in_rect(newX, newY, state.block)));
				bool confinedOnWater = (OBJ.allowed_on == OBJ_ON_WATER) && _obj_baseline_on_pri(newX, newY, width, 3);
				bool confinedOnLand = (OBJ.allowed_on == OBJ_ON_LAND) && _obj_baseline_on_pri(newX, newY, width, 3);
				bool stop = unconditionalStop || conditionalStop || confinedOnWater || confinedOnLand;

				if (OBJ.collide_with_objects) {
					for (size_t i = 0; i < MAX_NUM_OBJECTS; i++)
					{
						if (i != objNo && state.objects[i].active && state.objects[i].drawn && state.objects[i].collide_with_objects) {
							if (newY == state.objects[i].y
								&& newX >= state.objects[i].x
								&& newX < state.objects[i].x + _view_width(state.objects[i].view_no, state.objects[i].loop_no, state.objects[i].cel_no)
								)
							{
								stop = true;
							}
						}
					}
				}

				if (stop)
				{
					newX = OBJ.x;
					newY = OBJ.y;

					if (OBJ.move_mode == OBJ_MOVEMODE_WANDER)
					{
						OBJ.direction = _random_between(1, 9);
					}
					break;
				}
			}

			if (OBJ.move_mode == OBJ_MOVEMODE_WANDER && OBJ.move_distance_x == 0 && OBJ.move_distance_y == 0)
			{
				wander(objNo);
			}

			if (OBJ.move_mode == OBJ_MOVEMODE_MOVE_TO && OBJ.move_distance_x == 0 && OBJ.move_distance_y == 0)
			{
				OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
				OBJ.direction = DIR_STOPPED;
				state.flags[OBJ.move_done_flag] = true;
			}


			if (newY >= 167)
			{
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_BOTTOM;
				if (objNo != 0)
				{
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			}
			else if (newY <= 0 || (OBJ.observe_horizon && newY <= state.horizon))
			{
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_TOP;
				if (objNo != 0)
				{
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			}
			else if (newX <= 0)
			{
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_LEFT;
				if (objNo != 0)
				{
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			}
			else if (newX >= 160)
			{
				state.variables[objNo == 0 ? VAR_2_EGO_BORDER_CODE : VAR_5_OBJ_BORDER_CODE] = BORDER_RIGHT;
				if (objNo != 0)
				{
					state.variables[VAR_4_OBJ_BORDER_OBJNO] = objNo;
				}
			}

			OBJ.x = newX;
			OBJ.y = newY;
			//}

			OBJ.steps_to_next_update = OBJ.step_time;
		}
		OBJ.steps_to_next_update--;

		if (objNo == 0)
		{
			state.flags[FLAG_3_EGO_TOUCHED_TRIGGER] = _obj_baseline_on_pri(OBJ.x, OBJ.y, width, 2);
			state.flags[FLAG_0_EGO_ON_WATER] = _obj_baseline_on_pri(OBJ.x, OBJ.y, width, 3);
		}

		if (OBJ.is_cycling)
		{
			if (OBJ.cycles_to_next_update == 0)
			{
				int numCels = _view_num_cels(OBJ.view_no, OBJ.loop_no);

				if (OBJ.cycling_mode == CYCLE_MODE_REVERSE_CYCLE || OBJ.cycling_mode == CYCLE_MODE_REVERSE_LOOP)
				{
					if (OBJ.cel_no <= 0)
					{
						if (OBJ.cycling_mode == CYCLE_MODE_REVERSE_LOOP)
						{
							if (OBJ.end_of_loop_flag > -1)
							{
								state.flags[OBJ.end_of_loop_flag] = true;
								OBJ.end_of_loop_flag = -1;
							}
							OBJ.cel_no = 0;
							OBJ.is_cycling = false;
						}
						else
						{
							OBJ.cel_no = numCels - 1;
						}
					}
					else
					{
						OBJ.cel_no--;
					}
				}
				else
				{
					if (OBJ.cel_no >= numCels - 1)
					{
						if (OBJ.cycling_mode == CYCLE_MODE_END_OF_LOOP)
						{
							if (OBJ.end_of_loop_flag > -1)
							{
								state.flags[OBJ.end_of_loop_flag] = true;
								OBJ.end_of_loop_flag = -1;
							}
							OBJ.cel_no = numCels - 1;
							OBJ.is_cycling = false;
						}
						else
						{
							OBJ.cel_no = 0;
						}
					}
					else
					{
						OBJ.cel_no++;
					}
				}

				OBJ.cycles_to_next_update = OBJ.cycle_time;
			}
			OBJ.cycles_to_next_update--;
		}
	}
}

void _update_all_active()
{
	for (uint8_t i = 0; i < 16; i++)
	{
		if (state.objects[i].active && state.objects[i].drawn)
		{
			_update_object(i);
		}
	}
}

int cmpfunc(const void* a, const void* b) {
	object_t* obj_a = &state.objects[*(uint8_t*)a];
	object_t* obj_b = &state.objects[*(uint8_t*)b];
	return (obj_a->y - obj_b->y);
}

void _draw_all_active()
{
	uint8_t objs_sorted[16];
	int num_sorted = 0;

	for (int objNo = 15; objNo >= 0; objNo--)
	{
		if (OBJ.active)
		{
			if (OBJ.old_view_no > -1) {
				_draw_view(OBJ.old_view_no, OBJ.old_loop_no, OBJ.old_cel_no, OBJ.old_x, OBJ.old_y, 0, true, false); //erase
				OBJ.old_view_no = -1;
			}
			if (OBJ.drawn) {
				objs_sorted[num_sorted++] = objNo;
			}
		}
	}

	qsort(objs_sorted, num_sorted, sizeof(uint8_t), cmpfunc);

	for (int i = 0; i < num_sorted; i++)
	{
		int objNo = objs_sorted[i];
		uint8_t priority = _get_priority(objNo);
		_draw_view(OBJ.view_no, OBJ.loop_no, OBJ.cel_no, OBJ.x, OBJ.y, priority, false, false); //draw
		OBJ.old_view_no = OBJ.view_no;
		OBJ.old_loop_no = OBJ.loop_no;
		OBJ.old_cel_no = OBJ.cel_no;
		OBJ.old_x = OBJ.x;
		OBJ.old_y = OBJ.y;
	}
}

void add_to_pic(uint8_t viewNo, uint8_t loopNo, uint8_t celNo, uint8_t x, uint8_t y, uint8_t pri, uint8_t margin)
{
	_draw_view(viewNo, loopNo, celNo, x, y, pri, false, true);
	if (margin < 4) {
		int width = _view_width(viewNo, loopNo, celNo);
		for (size_t xm = 0; xm < width; xm++)
		{
			pic_pri_set(x + xm, y, margin);
			pic_pri_set(x + xm, y - 2, margin);
		}
		pic_pri_set(x, y - 1, margin);
		pic_pri_set(x + width - 1, y - 1, margin);
	}
}

void add_to_pic_v(uint8_t vviewNo, uint8_t vloopNo, uint8_t vcelNo, uint8_t vx, uint8_t vy, uint8_t vpri, uint8_t vmargin)
{
	add_to_pic(
		state.variables[vviewNo],
		state.variables[vloopNo],
		state.variables[vcelNo],
		state.variables[vx],
		state.variables[vy],
		state.variables[vpri],
		state.variables[vmargin]);
}

void animate_obj(uint8_t objNo)
{
	if (OBJ.active)
		return;

	OBJ.active = true;
	OBJ.is_cycling = true;
	OBJ.update = true;
	OBJ.drawn = false;

	OBJ.cycling_mode = CYCLE_MODE_NORMAL;
	OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
	OBJ.direction = DIR_STOPPED;
	OBJ.old_view_no = -1;
	OBJ.cycle_time = 1;
	OBJ.cycles_to_next_update = OBJ.cycle_time;

	OBJ.step_size = 1;
	OBJ.step_time = 1;
	OBJ.steps_to_next_update = OBJ.step_time;

	OBJ.fixed_priority = -1;
	OBJ.move_done_flag = -1;
	OBJ.end_of_loop_flag = -1;

	// OBJ.view_no = 0;
	// OBJ.loop_no = 0;
	// OBJ.cel_no = 0;
	// OBJ.fix_loop = false;
	OBJ.observe_horizon = true;
	OBJ.allowed_on = OBJ_ON_ANYTHING;
	OBJ.collide_with_objects = true;
	OBJ.collide_with_blocks = true;
	OBJ.move_distance_x = 0;
	OBJ.move_distance_y = 0;
	OBJ.move_step_size = 0;

}

void block(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	state.block_active = true;
	state.block.x1 = x1;
	state.block.y1 = y1;
	state.block.x2 = x2;
	state.block.y2 = y2;
}

void current_cel(uint8_t objNo, uint8_t var)
{
	state.variables[var] = OBJ.cel_no;
}

void current_loop(uint8_t objNo, uint8_t var)
{
	state.variables[var] = OBJ.loop_no;
}

void current_view(uint8_t objNo, uint8_t var)
{
	state.variables[var] = OBJ.view_no;
}

void cycle_time(uint8_t objNo, uint8_t var)
{
	OBJ.cycle_time = state.variables[var];
}

void discard_view(uint8_t num)
{
	UNIMPLEMENTED
}

void discard_view_v(uint8_t var)
{
	UNIMPLEMENTED
}

void distance(uint8_t objNo, uint8_t objNo2, uint8_t var)
{
	object_t _OBJ = state.objects[objNo];
	object_t OBJ2 = state.objects[objNo2];
	int deltaX = abs(_OBJ.x - OBJ2.x);
	int deltaY = abs(_OBJ.y - OBJ2.y);
	state.variables[var] = deltaX + deltaY;
}

void draw(uint8_t objNo)
{
	OBJ.drawn = true;
}

void end_of_loop(uint8_t objNo, uint8_t flag)
{
	state.flags[flag] = false;
	OBJ.end_of_loop_flag = flag;
	OBJ.cycling_mode = CYCLE_MODE_END_OF_LOOP;
	OBJ.is_cycling = true;
}

void erase(uint8_t objNo)
{
	//_draw_view(OBJ.view_no, OBJ.loop_no, OBJ.cel_no, OBJ.x, OBJ.y, 0, true, false);
	OBJ.drawn = false;
}

void fix_loop(uint8_t objNo)
{
	OBJ.fix_loop = true;
}

void follow_ego(uint8_t objNo, uint8_t stepSize, uint8_t fDoneFlag)
{
	UNIMPLEMENTED
}

void force_update(uint8_t objNo)
{
	_draw_view(OBJ.view_no, OBJ.loop_no, OBJ.cel_no, OBJ.x, OBJ.y, 0, true, false); //undraw
	_update_object(objNo);
}

void get_dir(uint8_t objNo, uint8_t var)
{
	state.variables[var] = OBJ.direction;
}

void get_posn(uint8_t objNo, uint8_t var, uint8_t var2)
{
	state.variables[var] = OBJ.x;
	state.variables[var2] = OBJ.y;
}

void get_priority(uint8_t objNo, uint8_t var)
{
	state.variables[var] = _get_priority(objNo);
}

void ignore_blocks(uint8_t objNo)
{
	OBJ.collide_with_blocks = false;
}

void ignore_horizon(uint8_t objNo)
{
	OBJ.observe_horizon = false;
}

void ignore_objs(uint8_t objNo)
{
	OBJ.collide_with_objects = false;
}

void last_cel(uint8_t objNo, uint8_t var)
{
	state.variables[var] = _view_num_cels(OBJ.view_no, OBJ.loop_no) - 1;
}

void load_view(uint8_t num)
{
	if (state.loaded_views[num].buffer)
		return;

	state.loaded_views[num] = load_vol_data("viewdir", num, false);
}

void load_view_v(uint8_t var)
{
	load_view(state.variables[var]);
}

void move_obj(uint8_t objNo, uint8_t x, uint8_t y, uint8_t stepSize, uint8_t fDoneFlag)
{
	reset(fDoneFlag);
	OBJ.move_mode = OBJ_MOVEMODE_MOVE_TO;
	OBJ.move_distance_x = x - OBJ.x;
	OBJ.move_distance_y = y - OBJ.y;
	OBJ.move_step_size = (stepSize == 0) ? OBJ.step_size : stepSize;
	OBJ.move_done_flag = fDoneFlag;
	if (objNo == 0)
	{
		program_control();
	}
}

void move_obj_v(uint8_t objNo, uint8_t vx, uint8_t vy, uint8_t stepSize, uint8_t fDoneFlag)
{
	move_obj(objNo, state.variables[vx], state.variables[vy], state.variables[stepSize], fDoneFlag);
}

void normal_cycle(uint8_t objNo)
{
	OBJ.cycling_mode = CYCLE_MODE_NORMAL;
	OBJ.is_cycling = true;
}

void normal_motion(uint8_t objNo)
{
	OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
}

void number_of_loops(uint8_t objNo, uint8_t var)
{
	state.variables[var] = _view_num_loops(OBJ.view_no);
}

void object_on_anything(uint8_t objNo)
{
	OBJ.allowed_on = OBJ_ON_ANYTHING;
}

void object_on_land(uint8_t objNo)
{
	OBJ.allowed_on = OBJ_ON_LAND;
}

void object_on_water(uint8_t objNo)
{
	OBJ.allowed_on = OBJ_ON_WATER;
}

void observe_blocks(uint8_t objNo)
{
	OBJ.collide_with_blocks = true;
}

void observe_horizon(uint8_t objNo)
{
	OBJ.observe_horizon = true;
}

void observe_objs(uint8_t objNo)
{
	OBJ.collide_with_objects = true;
}

void position(uint8_t objNo, uint8_t x, uint8_t y)
{
	OBJ.x = x;
	OBJ.y = y;
}

void position_v(uint8_t objNo, uint8_t var, uint8_t var2)
{
	position(objNo, state.variables[var], state.variables[var2]);
}

void release_loop(uint8_t objNo)
{
	OBJ.fix_loop = false;
}

void release_priority(uint8_t objNo)
{
	OBJ.fixed_priority = -1;
}

void reposition(uint8_t objNo, uint8_t var, uint8_t var2)
{
	int8_t dx = state.variables[var];
	int8_t dy = state.variables[var2];
	OBJ.x += dx;
	OBJ.y += dy;
}

void reposition_to(uint8_t objNo, uint8_t x, uint8_t y)
{
	erase(objNo);
	position(objNo, x, y);
	draw(objNo);
}

void reposition_to_v(uint8_t objNo, uint8_t vx, uint8_t vy)
{
	reposition_to(objNo, state.variables[vx], state.variables[vy]);
}

void reverse_cycle(uint8_t objNo)
{
	OBJ.cycling_mode = CYCLE_MODE_REVERSE_CYCLE;
	OBJ.is_cycling = true;
}

void reverse_loop(uint8_t objNo, uint8_t flag)
{
	OBJ.end_of_loop_flag = flag;
	state.flags[flag] = false;
	OBJ.cycling_mode = CYCLE_MODE_REVERSE_LOOP;
	OBJ.is_cycling = true;
}

void set_cel(uint8_t objNo, uint8_t num)
{
	OBJ.cel_no = num;
}

void set_cel_v(uint8_t objNo, uint8_t var)
{
	set_cel(objNo, state.variables[var]);
}

void set_dir(uint8_t objNo, uint8_t var)
{
	OBJ.direction = state.variables[var];
}

void set_horizon(uint8_t num)
{
	state.horizon = num;
}

void set_loop(uint8_t objNo, uint8_t num)
{
	OBJ.loop_no = num;
	OBJ.cel_no = 0;
}

void set_loop_v(uint8_t objNo, uint8_t var)
{
	set_loop(objNo, state.variables[var]);
}

void set_priority(uint8_t objNo, uint8_t num)
{
	OBJ.fixed_priority = num;
}

void set_priority_v(uint8_t objNo, uint8_t var)
{
	set_priority(objNo, state.variables[var]);
}

void set_upper_left(uint8_t num, uint8_t num2)
{
	UNIMPLEMENTED
}

void set_view(uint8_t objNo, uint8_t viewNo)
{
	OBJ.view_no = viewNo;
	OBJ.loop_no = 0;
	OBJ.cel_no = 0;
	OBJ.cycling_mode = CYCLE_MODE_NORMAL;
}

void set_view_v(uint8_t objNo, uint8_t var)
{
	set_view(objNo, state.variables[var]);
}

void start_cycling(uint8_t objNo)
{
	OBJ.is_cycling = true;
}

void start_motion(uint8_t objNo)
{
	if (objNo == 0) {
		player_control();
		state.variables[VAR_6_EGO_DIRECTION] = DIR_STOPPED;
	}
	OBJ.move_mode = OBJ_MOVEMODE_NORMAL;
}

void start_update(uint8_t objNo)
{
	OBJ.update = true;
}

void step_size(uint8_t objNo, uint8_t var)
{
	OBJ.step_size = state.variables[var];
}

void step_time(uint8_t objNo, uint8_t var)
{
	OBJ.step_time = state.variables[var];
}

void stop_cycling(uint8_t objNo)
{
	OBJ.is_cycling = false;
}

void stop_motion(uint8_t objNo)
{
	if (objNo == 0)
		program_control();
	OBJ.direction = DIR_STOPPED;
}

void stop_update(uint8_t objNo)
{
	OBJ.update = false;
}

void unanimate_all()
{
	for (size_t i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		state.objects[i].active = false;
		state.objects[i].drawn = false;
	}
}

void unblock()
{
	state.block_active = false;
}

void wander(uint8_t objNo)
{
	if (objNo == 0)
	{
		program_control();
	}
	OBJ.move_mode = OBJ_MOVEMODE_WANDER;
	OBJ.move_distance_x = _random_between(6, 51);
	OBJ.move_distance_y = _random_between(6, 51);
	_set_dir_from_moveDistance(objNo);
}