#include <string.h>

#include "state.h"
#include "heap.h"
#include "actions.h"
#include "platform_support.h"
#include "text_display.h"
#include "input_queue.h"

/* Test definitions */

test_t tests[] = {
	TEST(NULL, 0),
	TEST(equaln, 2),
	TEST(equalv, 2),
	TEST(lessn, 2),
	TEST(lessv, 2),
	TEST(greatern, 2),
	TEST(greaterv, 2),
	TEST(isset, 1),
	TEST(issetv, 1),
	TEST(has, 1),
	TEST(obj_in_room, 2),
	TEST(posn, 5),
	TEST(controller, 1),
	TEST(have_key, 0),
	TEST(said, 0),
	TEST(compare_strings, 2),
	TEST(obj_in_box, 5),
	TEST(center_posn, 5),
	TEST(right_posn, 5)};

/* Action definitions */

action_t actions[] = {
	ACTION("_return", _return, 0),
	ACTION("increment", increment, 1),
	ACTION("decrement", decrement, 1),
	ACTION("assignn", assignn, 2),
	ACTION("assignv", assignv, 2),
	ACTION("addn", addn, 2),
	ACTION("addv", addv, 2),
	ACTION("subn", subn, 2),
	ACTION("subv", subv, 2),
	ACTION("lindirectv", lindirectv, 2),
	ACTION("rindirect", rindirect, 2),
	ACTION("lindirectn", lindirectn, 2),
	ACTION("set", set, 1),
	ACTION("reset", reset, 1),
	ACTION("toggle", toggle, 1),
	ACTION("set_v", set_v, 1),
	ACTION("reset_v", reset_v, 1),
	ACTION("toggle_v", toggle_v, 1),
	ACTION("new_room", new_room, 1),
	ACTION("new_room_v", new_room_v, 1),
	ACTION("load_logics", load_logics, 1),
	ACTION("load_logics_v", load_logics_v, 1),
	ACTION("call", call, 1),
	ACTION("call_v", call_v, 1),
	ACTION("load_pic", load_pic, 1),
	ACTION("draw_pic", draw_pic, 1),
	ACTION("show_pic", show_pic, 0),
	ACTION("discard_pic", discard_pic, 1),
	ACTION("overlay_pic", overlay_pic, 1),
	ACTION("show_pri_screen", show_pri_screen, 0),
	ACTION("load_view", load_view, 1),
	ACTION("load_view_v", load_view_v, 1),
	ACTION("discard_view", discard_view, 1),
	ACTION("animate_obj", animate_obj, 1),
	ACTION("unanimate_all", unanimate_all, 0),
	ACTION("draw", draw, 1),
	ACTION("erase", erase, 1),
	ACTION("position", position, 3),
	ACTION("position_v", position_v, 3),
	ACTION("get_posn", get_posn, 3),
	ACTION("reposition", reposition, 3),
	ACTION("set_view", set_view, 2),
	ACTION("set_view_v", set_view_v, 2),
	ACTION("set_loop", set_loop, 2),
	ACTION("set_loop_v", set_loop_v, 2),
	ACTION("fix_loop", fix_loop, 1),
	ACTION("release_loop", release_loop, 1),
	ACTION("set_cel", set_cel, 2),
	ACTION("set_cel_v", set_cel_v, 2),
	ACTION("last_cel", last_cel, 2),
	ACTION("current_cel", current_cel, 2),
	ACTION("current_loop", current_loop, 2),
	ACTION("current_view", current_view, 2),
	ACTION("number_of_loops", number_of_loops, 2),
	ACTION("set_priority", set_priority, 2),
	ACTION("set_priority_v", set_priority_v, 2),
	ACTION("release_priority", release_priority, 1),
	ACTION("get_priority", get_priority, 2),
	ACTION("stop_update", stop_update, 1),
	ACTION("start_update", start_update, 1),
	ACTION("force_update", force_update, 1),
	ACTION("ignore_horizon", ignore_horizon, 1),
	ACTION("observe_horizon", observe_horizon, 1),
	ACTION("set_horizon", set_horizon, 1),
	ACTION("object_on_water", object_on_water, 1),
	ACTION("object_on_land", object_on_land, 1),
	ACTION("object_on_anything", object_on_anything, 1),
	ACTION("ignore_objs", ignore_objs, 1),
	ACTION("observe_objs", observe_objs, 1),
	ACTION("distance", distance, 3),
	ACTION("stop_cycling", stop_cycling, 1),
	ACTION("start_cycling", start_cycling, 1),
	ACTION("normal_cycle", normal_cycle, 1),
	ACTION("end_of_loop", end_of_loop, 2),
	ACTION("reverse_cycle", reverse_cycle, 1),
	ACTION("reverse_loop", reverse_loop, 2),
	ACTION("cycle_time", cycle_time, 2),
	ACTION("stop_motion", stop_motion, 1),
	ACTION("start_motion", start_motion, 1),
	ACTION("step_size", step_size, 2),
	ACTION("step_time", step_time, 2),
	ACTION("move_obj", move_obj, 5),
	ACTION("move_obj_v", move_obj_v, 5),
	ACTION("follow_ego", follow_ego, 3),
	ACTION("wander", wander, 1),
	ACTION("normal_motion", normal_motion, 1),
	ACTION("set_dir", set_dir, 2),
	ACTION("get_dir", get_dir, 2),
	ACTION("ignore_blocks", ignore_blocks, 1),
	ACTION("observe_blocks", observe_blocks, 1),
	ACTION("block", block, 4),
	ACTION("unblock", unblock, 0),
	ACTION("get", get, 1),
	ACTION("get_v", get_v, 1),
	ACTION("drop", drop, 1),
	ACTION("put", put, 2),
	ACTION("put_v", put_v, 2),
	ACTION("get_room_v", get_room_v, 2),
	ACTION("load_sound", load_sound, 1),
	ACTION("sound", sound, 2),
	ACTION("stop_sound", stop_sound, 0),
	ACTION("print", print, 1),
	ACTION("print_v", print_v, 1),
	ACTION("display", display, 3),
	ACTION("display_v", display_v, 3),
	ACTION("clear_lines", clear_lines, 3),
	ACTION("text_screen", text_screen, 0),
	ACTION("graphics", graphics, 0),
	ACTION("set_cursor_char", set_cursor_char, 1),
	ACTION("set_text_attribute", set_text_attribute, 2),
	ACTION("shake_screen", shake_screen, 1),
	ACTION("configure_screen", configure_screen, 3),
	ACTION("status_line_on", status_line_on, 0),
	ACTION("status_line_off", status_line_off, 0),
	ACTION("set_string", set_string, 2),
	ACTION("get_string", get_string, 5),
	ACTION("word_to_string", word_to_string, 2),
	ACTION("parse", parse, 1),
	ACTION("get_num", get_num, 2),
	ACTION("prevent_input", prevent_input, 0),
	ACTION("accept_input", accept_input, 0),
	ACTION("set_key", set_key, 3),
	ACTION("add_to_pic", add_to_pic, 7),
	ACTION("add_to_pic_v", add_to_pic_v, 7),
	ACTION("status", status, 0),
	ACTION("save_game", save_game, 0),
	ACTION("restore_game", restore_game, 0),
	ACTION("init_disk", init_disk, 0),
	ACTION("restart_game", restart_game, 0),
	ACTION("show_obj", show_obj, 1),
	ACTION("random", _random, 3),
	ACTION("program_control", program_control, 0),
	ACTION("player_control", player_control, 0),
	ACTION("obj_status_v", obj_status_v, 1),
	ACTION("quit", quit, 1),
	ACTION("show_mem", show_mem, 0),
	ACTION("pause", pause, 0),
	ACTION("echo_line", echo_line, 0),
	ACTION("cancel_line", cancel_line, 0),
	ACTION("init_joy", init_joy, 0),
	ACTION("toggle_monitor", toggle_monitor, 0),
	ACTION("version", version, 0),
	ACTION("script_size", script_size, 1),
	ACTION("set_game_id", set_game_id, 1),
	ACTION("log", _log, 1),
	ACTION("set_scan_start", set_scan_start, 0),
	ACTION("reset_scan_start", reset_scan_start, 0),
	ACTION("reposition_to", reposition_to, 3),
	ACTION("reposition_to_v", reposition_to_v, 3),
	ACTION("trace_on", trace_on, 0),
	ACTION("trace_info", trace_info, 3),
	ACTION("print_at", print_at, 4),
	ACTION("print_at_v", print_at_v, 4),
	ACTION("discard_view_v", discard_view_v, 1),
	ACTION("clear_text_rect", clear_text_rect, 5),
	ACTION("set_upper_left", set_upper_left, 2),
	ACTION("set_menu", set_menu, 1),
	ACTION("set_menu_item", set_menu_item, 2),
	ACTION("submit_menu", submit_menu, 0),
	ACTION("enable_item", enable_item, 1),
	ACTION("disable_item", disable_item, 1),
	ACTION("menu_input", menu_input, 0),
	ACTION("show_obj_v", show_obj_v, 1),
	ACTION("open_dialogue", open_dialogue, 0),
	ACTION("close_dialogue", close_dialogue, 0),
	ACTION("mul_n", mul_n, 2),
	ACTION("mul_v", mul_v, 2),
	ACTION("div_n", div_n, 2),
	ACTION("div_v", div_v, 2),
	ACTION("close_window", close_window, 0),
	ACTION("set_simple", set_simple, 1),
	ACTION("push_script", push_script, 0),
	ACTION("pop_script", pop_script, 0),
	ACTION("hold_key", hold_key, 0)};

char *get_message(uint8_t message_no)
{
	message_no--;
	uint8_t *buffer = heap_data.loaded_logics[state.current_logic].buffer;
	uint8_t *message_section = buffer + ((buffer[1] << 8) | buffer[0]) + 2;

	uint8_t *message_offset = (message_section + 3 + 2 * message_no);
	uint16_t offset = *(message_offset) | (*(message_offset + 1) << 8);

	char *message = ((char *)message_section) + offset + 1;

	return message;
}

uint8_t next_data()
{
	return *(((uint8_t*)heap_data.loaded_logics[state.current_logic].buffer) + 2 + (state.pc++));
}

void jump()
{
	int16_t jump = next_data() | (next_data() << 8);
	state.pc += jump;
}

bool doTest(uint8_t opcode)
{
	switch (tests[opcode].numArgs)
	{
	case 0:
		return tests[opcode].test();
	case 1:
		return tests[opcode].test(next_data());
	case 2:
	{
		uint8_t d1 = next_data();
		uint8_t d2 = next_data();
		return tests[opcode].test(d1, d2);
	}
	case 3:
	{
		uint8_t d1 = next_data();
		uint8_t d2 = next_data();
		uint8_t d3 = next_data();
		return tests[opcode].test(d1, d2, d3);
	}
	case 4:
	{
		uint8_t d1 = next_data();
		uint8_t d2 = next_data();
		uint8_t d3 = next_data();
		uint8_t d4 = next_data();
		return tests[opcode].test(d1, d2, d3, d4);
	}
	case 5:
	{
		uint8_t d1 = next_data();
		uint8_t d2 = next_data();
		uint8_t d3 = next_data();
		uint8_t d4 = next_data();
		uint8_t d5 = next_data();
		return tests[opcode].test(d1, d2, d3, d4, d5);
	}
	default:
		panic("Logic test with unimplemented number of parameters");
		return false;
	}
}

void step()
{
	uint8_t opcode = next_data();

	if (opcode == 0xFF)
	{
		if (state.test)
		{
			bool testResult = state.and_result && state.or_result;
			if (!testResult)
			{
				jump();
			}
			else
			{
				state.pc += 2;
			}
		}
		else
		{
			state.and_result = true;
			state.or_result = true;
		}
		state.test = !state.test;
	}
	else if (opcode == 0xFE)
	{
		jump();
	}
	else if (opcode == 0xFD)
	{
		state.negate = true;
	}
	else if (opcode == 0xFC)
	{
		state.or = !state.or ;
		if (state.or)
			state.or_result = false;
	}
	else
	{
		if (state.test)
		{
			bool pass = doTest(opcode);
			if (state.negate)
				pass = !pass;

			state.negate = false;

			if (state.or)
				state.or_result = state.or_result || pass;
			else
				state.and_result = state.and_result && pass;
		}
		else
		{
			//printf("Logic %d: %s\n", state.current_logic, actions[opcode].name);
			switch (actions[opcode].numArgs)
			{
			case 0:
				actions[opcode].action();
				break;
			case 1:
				actions[opcode].action(next_data());
				break;
			case 2:
			{
				uint8_t a = next_data();
				uint8_t b = next_data();
				actions[opcode].action(a, b);
			}
			break;
			case 3:
			{
				uint8_t a = next_data();
				uint8_t b = next_data();
				uint8_t c = next_data();
				actions[opcode].action(a, b, c);
			}
			break;
			case 4:
			{
				uint8_t a = next_data();
				uint8_t b = next_data();
				uint8_t c = next_data();
				uint8_t d = next_data();
				actions[opcode].action(a, b, c, d);
			}
			break;
			case 5:
			{
				uint8_t a = next_data();
				uint8_t b = next_data();
				uint8_t c = next_data();
				uint8_t d = next_data();
				uint8_t e = next_data();
				actions[opcode].action(a, b, c, d, e);
			}
			break;
			case 7:
			{
				uint8_t a = next_data();
				uint8_t b = next_data();
				uint8_t c = next_data();
				uint8_t d = next_data();
				uint8_t e = next_data();
				uint8_t f = next_data();
				uint8_t g = next_data();
				actions[opcode].action(a, b, c, d, e, f, g);
			}
			break;
			default:
				panic("Logic action with unimplemented number of parameters");
			}
		}
	}
}

bool _find_word_group_of_word(char *word, size_t len)
{
	int first_letter_index = *word - 'a';
	uint16_be_t first_word_index = heap_data.words_file->word_indices[first_letter_index];
	uint16_be_t next_word_index = heap_data.words_file->word_indices[first_letter_index + 1];
	uint16_t first_word_offset = (uint16_t)(first_word_index.hi_byte << 8) | (uint16_t)first_word_index.lo_byte;
	uint16_t next_word_offset = (uint16_t)(next_word_index.hi_byte << 8) | (uint16_t)next_word_index.lo_byte;

	uint8_t *word_entry = ((uint8_t *)heap_data.words_file) + first_word_offset;
	uint8_t *next_word_entry = ((uint8_t *)heap_data.words_file) + next_word_offset;

	char prev_word[40];
	int prev_word_len = 0;

	while (1)
	{
		prev_word_len = *(word_entry++);
		while (1)
		{
			char c = *(word_entry++) ^ 0x7F;
			prev_word[prev_word_len++] = c & 0x7F;
			if (c >> 7)
			{
				break;
			}
		}
		uint16_t word_num_hi = *(word_entry++);
		uint16_t word_num_lo = *(word_entry++);
		uint16_t word_num = (word_num_hi << 8) | word_num_lo;
		// end of word -- check match
		if (prev_word_len == len && strncmp(word, prev_word, len) == 0)
		{
			// Found match
			if (word_num > 0)
			{ // Skip anyword
				state.parsed_word_groups[state.num_parsed_word_groups++] = word_num;
			}
			return true;
		}

		if (word_entry >= next_word_entry)
		{
			if (len == 1)
			{
				if (*word == 'a' || *word == 'i')
				{
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

void _parse_word_groups()
{
	state.num_parsed_word_groups = 0;

	char *c = state.input_buffer;
	char *word_start = c;
	size_t word_len = 0;
	int word_i = 0;

	while (c < state.input_buffer + state.input_pos)
	{
		if (*c == ' ' || *c == '.')
		{
			// Find out word group to parsed word
			if (!_find_word_group_of_word(word_start, word_len))
			{
				state.variables[VAR_9_MISSING_WORD_NO] = word_i;
			}
			c++;
			word_start = c;
			word_len = 0;
			word_i++;
		}
		else
		{
			word_len++;
			c++;
		}
	}
	if (!_find_word_group_of_word(word_start, word_len) && state.variables[VAR_9_MISSING_WORD_NO] == 0)
	{
		state.variables[VAR_9_MISSING_WORD_NO] = word_i;
	}
}

void set_dir_from_move_distance(uint8_t objNo)
{
	if (OBJ.move_distance_x < 0 && OBJ.move_distance_y < 0)
	{
		OBJ.direction = DIR_UP_LEFT;
	}
	else if (OBJ.move_distance_x < 0 && OBJ.move_distance_y > 0)
	{
		OBJ.direction = DIR_DOWN_LEFT;
	}
	else if (OBJ.move_distance_x > 0 && OBJ.move_distance_y < 0)
	{
		OBJ.direction = DIR_UP_RIGHT;
	}
	else if (OBJ.move_distance_x > 0 && OBJ.move_distance_y > 0)
	{
		OBJ.direction = DIR_DOWN_RIGHT;
	}
	else if (OBJ.move_distance_x < 0)
	{
		OBJ.direction = DIR_LEFT;
	}
	else if (OBJ.move_distance_x > 0)
	{
		OBJ.direction = DIR_RIGHT;
	}
	else if (OBJ.move_distance_y < 0)
	{
		OBJ.direction = DIR_UP;
	}
	else if (OBJ.move_distance_y > 0)
	{
		OBJ.direction = DIR_DOWN;
	}
}

void execute_logic_cycle()
{
	load_logic_no_script_write(0);
	state.current_logic = 0;
	state.pc = state.scan_start[0];
	state.stack_ptr = 0;

	state.cycle_complete = false;
	while (!state.cycle_complete)
	{
		step();
	}
}

void process_input_queue()
{
	for (int i = 0; i < queue_pos; i++)
	{
		input_queue_entry_t entry = queue[i];

		if (entry.scancode == AGI_KEY_HOME)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_LEFT ? DIR_STOPPED : DIR_UP_LEFT;
		}
		else if (entry.scancode == AGI_KEY_UP)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP ? DIR_STOPPED : DIR_UP;
		}
		else if (entry.scancode == AGI_KEY_PGUP)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_UP_RIGHT ? DIR_STOPPED : DIR_UP_RIGHT;
		}
		else if (entry.scancode == AGI_KEY_RIGHT)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_RIGHT ? DIR_STOPPED : DIR_RIGHT;
		}
		else if (entry.scancode == AGI_KEY_PGDN)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_RIGHT ? DIR_STOPPED : DIR_DOWN_RIGHT;
		}
		else if (entry.scancode == AGI_KEY_DOWN)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN ? DIR_STOPPED : DIR_DOWN;
		}
		else if (entry.scancode == AGI_KEY_END)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_DOWN_LEFT ? DIR_STOPPED : DIR_DOWN_LEFT;
		}
		else if (entry.scancode == AGI_KEY_LEFT)
		{
			state.variables[VAR_6_EGO_DIRECTION] = state.variables[VAR_6_EGO_DIRECTION] == DIR_LEFT ? DIR_STOPPED : DIR_LEFT;
		}
		else
		{
			bool found_controller = false;
			for (size_t j = 0; j < 50; j++)
			{
				controller_assignment_t controller_assignment = state.controller_assignments[j];
				if ((controller_assignment.ascii > 0 && controller_assignment.ascii == entry.ascii) ||
					(controller_assignment.scancode > 0 && controller_assignment.scancode == entry.scancode))
				{
					state.controllers[controller_assignment.controller_no] = true;
					found_controller = true;
				}
			}

			if (!found_controller)
			{
				if (entry.ascii)
				{
					if (entry.ascii == '\b')
					{
						if (state.input_pos > 0)
						{
							state.input_pos--;
						}
					}
					else
					{
						state.input_buffer[state.input_pos++] = entry.ascii;
					}
					state.input_buffer[state.input_pos] = '\0';
					_redraw_prompt();
				}
			}
		}
	}

	queue_pos = 0;
}

void agi_logic_run_cycle()
{
	for (size_t i = 0; i < MAX_NUM_CONTROLLERS; i++)
	{
		state.controllers[i] = false;
	}
	
	state.flags[FLAG_2_COMMAND_ENTERED] = false;
	state.flags[FLAG_4_SAID_ACCEPTED_INPUT] = false;
	state.variables[VAR_19_KEYBOARD_KEY_PRESSED] = 0;
	state.variables[VAR_9_MISSING_WORD_NO] = 0;

	process_input_queue();

	if (state.enter_pressed)
	{
		state.num_parsed_word_groups = 0;
		if (state.input_pos > 0)
		{
			_parse_word_groups();
			if (state.num_parsed_word_groups > 0)
			{
				state.flags[FLAG_2_COMMAND_ENTERED] = true;
			}
		}
		state.input_pos = 0;
		state.input_buffer[state.input_pos] = '\0';
		_redraw_prompt();
	}

	if (state.program_control)
	{
		state.variables[VAR_6_EGO_DIRECTION] = EGO.direction;
	}
	else
	{
		EGO.direction = state.variables[VAR_6_EGO_DIRECTION];
	}

	// recalculate direction of motion
	for (uint8_t objNo = 0; objNo < MAX_NUM_OBJECTS; objNo++)
	{
		if (OBJ.active && OBJ.update && OBJ.drawn && OBJ.move_mode == OBJ_MOVEMODE_MOVE_TO)
		{
			set_dir_from_move_distance(objNo);
		}
	}

	uint8_t previous_score = state.variables[VAR_3_SCORE];
	bool previous_sound_status = state.flags[FLAG_9_SOUND_ENABLED];

	execute_logic_cycle();

	EGO.direction = state.variables[VAR_6_EGO_DIRECTION];

	if (previous_score != state.variables[VAR_3_SCORE] || previous_sound_status != state.flags[FLAG_9_SOUND_ENABLED])
	{
		redraw_status_line();
	}

	state.variables[VAR_4_OBJ_BORDER_OBJNO] = 0;
	state.variables[VAR_5_OBJ_BORDER_CODE] = 0;

	state.flags[FLAG_5_ROOM_EXECUTED_FIRST_TIME] = false;
	state.flags[FLAG_6_RESTART_GAME_EXECUTED] = false;
	state.flags[FLAG_12_GAME_RESTORED] = false;

	update_all_active();
}

void agi_initialize() {
	state_reset();
	state.flags[FLAG_5_ROOM_EXECUTED_FIRST_TIME] = true;
}