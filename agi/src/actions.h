#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define OBJ state.objects[objNo]
#define UNIMPLEMENTED printf("(Logic: %d, pc: %d) Unimplemented command: %s\n", state.current_logic, state.pc, __func__);
#define WARNING(x) printf("Unimplemented command: %s - %s\n", __func__, x);

void pic_vis_set(int x, int y, int color);
int pic_vis_get(int x, int y);
void pic_pri_set(int x, int y, int priority);
int pic_pri_get(int x, int y);

void _decrypt_messages(uint8_t logic_no);
void _decrypt_item_file(uint8_t* item_file, size_t size);
uint8_t next_data();
void _update_all_active();
void _redraw_status_line();
void _set_dir_from_moveDistance(uint8_t objNo);

void load_pic_no(uint8_t pic_no);
void overlay_pic_no(uint8_t pic_no);
void draw_pic_no(uint8_t pic_no);
void discard_pic_no(uint8_t pic_no);
void load_logic_no_script_write(uint8_t num);

typedef struct {
	uint8_t numArgs;
	bool (*test)();
} test_t;

#define TEST(t, n) {.numArgs = n, .test = (bool (*)())t}

extern test_t tests[];

/* Test commands */
bool equaln(uint8_t var, uint8_t num);
bool equalv(uint8_t var, uint8_t var2);
bool lessn(uint8_t var, uint8_t num);
bool lessv(uint8_t var, uint8_t var2);
bool greatern(uint8_t var, uint8_t num);
bool greaterv(uint8_t var, uint8_t var2);
bool isset(uint8_t flag);
bool issetv(uint8_t var);
bool has(uint8_t item);
bool obj_in_room(uint8_t item, uint8_t var);
bool posn(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4);
bool controller(uint8_t ctr);
bool have_key();
bool said();
bool compare_strings(uint8_t str, uint8_t str2);
bool obj_in_box(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4);
bool center_posn(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4);
bool right_posn(uint8_t obj, uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4);

typedef struct {
	const char* name;
	uint8_t numArgs;
	void (*action)();
} action_t;

#define ACTION(_name, a, n) {_name, n, (void (*)())a }


extern action_t actions[];

/* Actions */
void _return();
void increment(uint8_t var);
void decrement(uint8_t var);
void assignn(uint8_t var, uint8_t num);
void assignv(uint8_t v1, uint8_t v2);
void addn(uint8_t var, uint8_t num);
void addv(uint8_t v1, uint8_t v2);
void subn(uint8_t var, uint8_t num);
void subv(uint8_t v1, uint8_t v2);
void lindirectv(uint8_t v1, uint8_t v2);
void rindirect(uint8_t v1, uint8_t v2);
void lindirectn(uint8_t var, uint8_t num);
void set(uint8_t flag);
void reset(uint8_t flag);
void toggle(uint8_t flag);
void set_v(uint8_t var);
void reset_v(uint8_t var);
void toggle_v(uint8_t var);
void new_room(uint8_t num);
void new_room_v(uint8_t var);
void load_logics(uint8_t num);
void load_logics_v(uint8_t var);
void call(uint8_t num);
void call_v(uint8_t var);
void load_pic(uint8_t var);
void draw_pic(uint8_t var);
void show_pic();
void discard_pic(uint8_t var);
void overlay_pic(uint8_t var);
void show_pri_screen();
void load_view(uint8_t num);
void load_view_v(uint8_t var);
void discard_view(uint8_t num);
void animate_obj(uint8_t obj);
void unanimate_all();
void draw(uint8_t obj);
void erase(uint8_t obj);
void position(uint8_t obj, uint8_t num, uint8_t num2);
void position_v(uint8_t obj, uint8_t var, uint8_t var2);
void get_posn(uint8_t obj, uint8_t var, uint8_t var2);
void reposition(uint8_t obj, uint8_t var, uint8_t var2);
void set_view(uint8_t obj, uint8_t num);
void set_view_v(uint8_t obj, uint8_t var);
void set_loop(uint8_t obj, uint8_t num);
void set_loop_v(uint8_t obj, uint8_t var);
void fix_loop(uint8_t obj);
void release_loop(uint8_t obj);
void set_cel(uint8_t obj, uint8_t num);
void set_cel_v(uint8_t obj, uint8_t var);
void last_cel(uint8_t obj, uint8_t var);
void current_cel(uint8_t obj, uint8_t var);
void current_loop(uint8_t obj, uint8_t var);
void current_view(uint8_t obj, uint8_t var);
void number_of_loops(uint8_t obj, uint8_t var);
void set_priority(uint8_t obj, uint8_t num);
void set_priority_v(uint8_t obj, uint8_t var);
void release_priority(uint8_t obj);
void get_priority(uint8_t obj, uint8_t var);
void stop_update(uint8_t obj);
void start_update(uint8_t obj);
void force_update(uint8_t obj);
void ignore_horizon(uint8_t obj);
void observe_horizon(uint8_t obj);
void set_horizon(uint8_t num);
void object_on_water(uint8_t obj);
void object_on_land(uint8_t obj);
void object_on_anything(uint8_t obj);
void ignore_objs(uint8_t obj);
void observe_objs(uint8_t obj);
void distance(uint8_t obj, uint8_t obj2, uint8_t var);
void stop_cycling(uint8_t obj);
void start_cycling(uint8_t obj);
void normal_cycle(uint8_t obj);
void end_of_loop(uint8_t obj, uint8_t flag);
void reverse_cycle(uint8_t obj);
void reverse_loop(uint8_t obj, uint8_t flag);
void cycle_time(uint8_t obj, uint8_t var);
void stop_motion(uint8_t obj);
void start_motion(uint8_t obj);
void step_size(uint8_t obj, uint8_t var);
void step_time(uint8_t obj, uint8_t var);
void move_obj(uint8_t objNo, uint8_t x, uint8_t y, uint8_t stepSize, uint8_t fDoneFlag);
void move_obj_v(uint8_t objNo, uint8_t vx, uint8_t vy, uint8_t stepSize, uint8_t fDoneFlag);
void follow_ego(uint8_t objNo, uint8_t stepSize, uint8_t fDoneFlag);
void wander(uint8_t obj);
void normal_motion(uint8_t obj);
void set_dir(uint8_t obj, uint8_t var);
void get_dir(uint8_t obj, uint8_t var);
void ignore_blocks(uint8_t obj);
void observe_blocks(uint8_t obj);
void block(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void unblock();
void get(uint8_t item);
void get_v(uint8_t var);
void drop(uint8_t item);
void put(uint8_t item, uint8_t room);
void put_v(uint8_t var, uint8_t var2);
void get_room_v(uint8_t var, uint8_t var2);
void load_sound(uint8_t num);
void sound(uint8_t num, uint8_t flag);
void stop_sound();
void print(uint8_t msg);
void print_v(uint8_t var);
void display(uint8_t num, uint8_t num2, uint8_t msg);
void display_v(uint8_t var, uint8_t var2, uint8_t var3);
void clear_lines(uint8_t top, uint8_t bottom, uint8_t color);
void text_screen();
void graphics();
void set_cursor_char(uint8_t msg);
void set_text_attribute(uint8_t num, uint8_t num2);
void shake_screen(uint8_t num);
void configure_screen(uint8_t num, uint8_t num2, uint8_t num3);
void status_line_on();
void status_line_off();
void set_string(uint8_t str, uint8_t msg);
void get_string(uint8_t str, uint8_t msg, uint8_t col, uint8_t row, uint8_t maxLen);
void word_to_string(uint8_t word, uint8_t str);
void parse(uint8_t str);
void get_num(uint8_t str, uint8_t var);
void prevent_input();
void accept_input();
void set_key(uint8_t num, uint8_t num2, uint8_t num3);
void add_to_pic(uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4, uint8_t num5, uint8_t num6, uint8_t num7);
void add_to_pic_v(uint8_t var, uint8_t var2, uint8_t var3, uint8_t var4, uint8_t var5, uint8_t var6, uint8_t var7);
void status();
void save_game();
void restore_game();
void init_disk();
void restart_game();
void show_obj(uint8_t num);
void _random(uint8_t num, uint8_t num2, uint8_t var);
void program_control();
void player_control();
void obj_status_v(uint8_t var);
void quit(uint8_t num);
void show_mem();
void pause();
void echo_line();
void cancel_line();
void init_joy();
void toggle_monitor();
void version();
void script_size(uint8_t num);
void set_game_id(uint8_t msg);
void _log(uint8_t msg);
void set_scan_start();
void reset_scan_start();
void reposition_to(uint8_t obj, uint8_t num, uint8_t num2);
void reposition_to_v(uint8_t obj, uint8_t var, uint8_t var2);
void trace_on();
void trace_info(uint8_t num, uint8_t num2, uint8_t num3);
void print_at(uint8_t msg, uint8_t num, uint8_t num2, uint8_t num3);
void print_at_v(uint8_t var, uint8_t num, uint8_t num2, uint8_t num3);
void discard_view_v(uint8_t var);
void clear_text_rect(uint8_t num, uint8_t num2, uint8_t num3, uint8_t num4, uint8_t num5);
void set_upper_left(uint8_t num, uint8_t num2);
void set_menu(uint8_t msg);
void set_menu_item(uint8_t msg, uint8_t ctr);
void submit_menu();
void enable_item(uint8_t ctr);
void disable_item(uint8_t ctr);
void menu_input();
void show_obj_v(uint8_t var);
void open_dialogue();
void close_dialogue();
void mul_n(uint8_t var, uint8_t num);
void mul_v(uint8_t var, uint8_t var2);
void div_n(uint8_t var, uint8_t num);
void div_v(uint8_t var, uint8_t var2);
void close_window();
void set_simple(uint8_t a);
void push_script();
void pop_script();
void hold_key();