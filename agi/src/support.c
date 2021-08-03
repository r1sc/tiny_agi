#include "actions.h"
#include "state.h"

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
