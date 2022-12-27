#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "heap.h"
#include "state.h"

bool _find_word_group_of_word(char* word, size_t len) {
	if (len == 0)
		return true;
	if (*word < 'a' || *word > 'z')
		return true;

	int first_letter_index = *word - 'a';

	uint16_t first_word_offset = 0;
	while (first_word_offset == 0) {
		uint16_be_t first_word_index = heap_data.words_file->word_indices[first_letter_index];
		first_word_offset = (uint16_t)(first_word_index.hi_byte << 8) | (uint16_t)first_word_index.lo_byte;
		first_letter_index--;
	}

	uint16_t next_word_offset = 0;
	while (next_word_offset <= first_word_offset) {
		uint16_be_t next_word_index = heap_data.words_file->word_indices[first_letter_index + 1];
		next_word_offset = (uint16_t)(next_word_index.hi_byte << 8) | (uint16_t)next_word_index.lo_byte;
		first_letter_index++;
	}

	uint8_t* word_entry = ((uint8_t*)heap_data.words_file) + first_word_offset;
	uint8_t* next_word_entry = ((uint8_t*)heap_data.words_file) + next_word_offset;

	char prev_word[40];
	int prev_word_len = 0;

	while (1) {
		prev_word_len = *(word_entry++);
		while (1) {
			char c = *(word_entry++) ^ 0x7F;
			prev_word[prev_word_len++] = c & 0x7F;
			if (c >> 7) {
				break;
			}
		}
		uint16_t word_num_hi = *(word_entry++);
		uint16_t word_num_lo = *(word_entry++);
		uint16_t word_num = (word_num_hi << 8) | word_num_lo;
		// end of word -- check match
		if (prev_word_len == len && strncmp(word, prev_word, len) == 0) {
			// Found match
			if (word_num > 0) { // Skip anyword
				system_state.parsed_word_groups[system_state.num_parsed_word_groups++] = word_num;
			}
			return true;
		}

		if (word_entry >= next_word_entry) {
			if (len == 1) {
				if (*word == 'a' || *word == 'i') {
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

void parse_word_groups() {
	system_state.num_parsed_word_groups = 0;

	system_state.input_buffer[system_state.input_pos] = 0;

	// Remove punctuation and convert to lower case
	char* c = system_state.input_buffer;
	while (*c) {
		if (*c == '.' || *c == ',' || *c == '!' || *c == '?' || *c == ';') {
			*c = ' ';
		} else {
			*c = tolower(*c);
		}
		c++;
	}

	// Trim excess spaces
	c = system_state.input_buffer;
	bool prev_was_whitespace = false;
	while (*c) {
		if (*c == ' ') {
			if (prev_was_whitespace && *(c + 1) != 0) {
				strcpy(c, c + 1);
			}
			prev_was_whitespace = true;
		} else {
			prev_was_whitespace = false;
		}
		c++;
	}

	// Right trim ending spaces
	c--;
	while (*c == ' ') {
		*c = 0;
		c--;
	}

	c = system_state.input_buffer;

	char* word_start = c;
	size_t word_len = 0;
	int word_i = 0;

	while (*c) {
		if (*c == ' ' || *c == '.') {
			// Find out word group to parsed word
			if (!_find_word_group_of_word(word_start, word_len)) {
				state.variables[VAR_9_MISSING_WORD_NO] = word_i;
			}
			c++;
			word_start = c;
			word_len = 0;
			word_i++;
		} else {
			word_len++;
			c++;
		}
	}
	if (!_find_word_group_of_word(word_start, word_len) && state.variables[VAR_9_MISSING_WORD_NO] == 0) {
		state.variables[VAR_9_MISSING_WORD_NO] = word_i;
	}
}