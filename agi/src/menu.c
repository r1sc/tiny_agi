#include <string.h>

#include "menu.h"
#include "state.h"
#include "text_display.h"
#include "actions.h"
#include "constants.h"

void redraw_menu() {	
	uint8_t row = 0;
	uint8_t col = 1;
	_draw_char(0, 0, ' ', 0, 15);

	menu_header_t* menu = state.first_menu;
	int left_col = 0;

	while (menu != NULL) {
		const char* message = get_message(menu->text.logic_no, menu->text.message_no);
		
		uint8_t fg = 0;
		uint8_t bg = 15;

		if (menu == *state.current_menu) {
			fg = 15;
			bg = 0;
			left_col = col - 1; // subtract 1 for the border
		}

		_draw_text(&row, &col, message, fg, bg);
		_draw_text(&row, &col, " ", 0, 15);


		menu = menu->next;
	}

	for (size_t i = col; i < 40; i++) {
		_draw_char(i * 8, 0, ' ', 0, 15);
	}

	
	menu = *state.current_menu;
	menu_item_t* menu_item = menu->first_item;
	int right_col = 0;
	row = 2;

	while (menu_item != NULL) {
		const char* message = get_message(menu_item->text.logic_no, menu_item->text.message_no);
		if (menu_item == menu->first_item) {
			int width = strlen(message) + 1; // add borders
			right_col = left_col + width;
			if (right_col > 39) {
				int amount = right_col - 39;
				left_col -= amount;
				right_col -= amount;
			}
			for (col = left_col; col <= right_col; col++) {
				if (col == left_col) {
					_draw_char(col * 8, 8, CORNER_NW_BAR, 0, 15);
				}
				else if (col == right_col) {
					_draw_char(col * 8, 8, CORNER_NE_BAR, 0, 15);
				}
				else {
					_draw_char(col * 8, 8, HORIZONTAL_BAR, 0, 15);
				}
			}
		}

		uint8_t fg = 0;
		uint8_t bg = 15;

		if (menu_item == *state.current_menu_item) {
			fg = 15;
			bg = 0;
		}

		col = left_col + 1;
		_draw_char(left_col * 8, row * 8, VERTICAL_BAR, 0, 15);
		_draw_text(&row, &col, message, fg, bg);
		_draw_char(right_col * 8, row * 8, VERTICAL_BAR, 0, 15);

		row++;
		menu_item = menu_item->next;
	}

	for (col = left_col; col <= right_col; col++) {
		if (col == left_col) {
			_draw_char(col * 8, row * 8, CORNER_SW_BAR, 0, 15);
		}
		else if (col == right_col) {
			_draw_char(col * 8, row * 8, CORNER_SE_BAR, 0, 15);
		}
		else {
			_draw_char(col * 8, row * 8, HORIZONTAL_BAR, 0, 15);
		}
	}
}
