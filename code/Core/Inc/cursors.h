#ifndef INC_CURSORS_H_
#define INC_CURSORS_H_

#include "main.h"
#include "tim.h"

#define SCOPE_X 420
#define GRIDS_X 10
#define PX_PER_GRID_X 42 //SCOPE_X/GRIDS_X

#define SCOPE_Y_OFFSET 32
#define SCOPE_Y 253
#define GRIDS_Y 6
#define PX_PER_GRID_Y  42//SCOPE_Y/GRIDS_Y

static int32_t time_cursor_1,time_cursor_2, voltage_cursor_1, voltage_cursor_2;
static int8_t encoder_button_state = 0;

int measureTime(uint32_t time_per_grid);
int drawCursorsTime(int32_t time_per_grid, uint8_t curs_num_flag, int32_t encoder);

int measureVoltage(uint32_t voltage_per_grid);
int drawCursorsVoltage(int32_t volt_per_grid, uint8_t curs_num_flag, int32_t encoder);



void resetCursors();
#endif
