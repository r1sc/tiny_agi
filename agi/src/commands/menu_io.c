#include <stdlib.h>

#include "../actions.h"
#include "../state.h"
#include "../heap.h"
#include "../platform_support.h"
#include "../menu.h"

void disable_item(uint8_t ctr) {
	UNIMPLEMENTED
}

void enable_item(uint8_t ctr) {
	UNIMPLEMENTED
}

void hold_key() {
	UNIMPLEMENTED
}

void menu_input() {
	if (state.flags[FLAG_14_MENU_ENABLED]) {
		state.in_menu = true;
		state.current_menu = &state.first_menu;
		state.current_menu_item = &state.first_menu->first_item;
		redraw_menu();
	}
}

void set_key(uint8_t ascii, uint8_t scan_code, uint8_t ctrl) {
	controller_assignment_t* controller_assignment = *state.current_controller_assignment = (controller_assignment_t*)malloc(sizeof(controller_assignment_t));
	if (!controller_assignment) {
		panic("Failed to alloc memory for controller assignment");
		return;
	}
	controller_assignment->controller = ctrl;
	controller_assignment->ascii = ascii;
	controller_assignment->scancode = scan_code;
	controller_assignment->next = NULL;

	state.current_controller_assignment = &controller_assignment->next;
}

void set_menu(uint8_t msg) {
	menu_header_t* menu = *state.current_menu = (menu_header_t*)malloc(sizeof(menu_header_t));
	if (!menu) {
		panic("Failed to alloc memory for menu");
		return;
	}

	menu->text.logic_no = state.current_logic;
	menu->text.message_no = msg;
	menu->first_item = NULL;
	menu->prev = state.prev_menu;
	menu->next = NULL;
	
	state.prev_menu = menu;
	state.prev_menu_item = NULL;
	state.current_menu = &menu->next;
	state.current_menu_item = &menu->first_item;
}

void set_menu_item(uint8_t msg, uint8_t ctr) {
	menu_item_t* menu_item = *state.current_menu_item = (menu_item_t*)malloc(sizeof(menu_item_t));
	if (!menu_item) {
		panic("Failed to alloc memory for menu item");
		return;
	}

	menu_item->text.logic_no = state.current_logic;
	menu_item->text.message_no = msg;
	menu_item->controller = ctr;
	menu_item->prev = state.prev_menu_item;
	menu_item->next = NULL;

	state.prev_menu_item = menu_item;
	state.current_menu_item = &menu_item->next;
}

void submit_menu() {
	state.current_menu = &state.first_menu;
	state.current_menu_item = &state.first_menu->first_item;
}