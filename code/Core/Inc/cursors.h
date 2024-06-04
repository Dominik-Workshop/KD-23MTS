#ifndef INC_CURSORS_H_
#define INC_CURSORS_H_

#include "main.h"
#include "tim.h"
#include "ILI9488.h"


#define SCOPE_X 420
#define GRIDS_X 10
#define PX_PER_GRID_X 42 //SCOPE_X/GRIDS_X

#define SCOPE_Y_OFFSET 32
#define SCOPE_Y 253
#define GRIDS_Y 6
#define PX_PER_GRID_Y  42//SCOPE_Y/GRIDS_Y




enum ActiveCursorType{
	CursorType_TIME,
	CursorType_VOLTAGE,
	CursorType_DISABLE
};

typedef struct CH_cursors{

	enum ActiveCursorType cursor_type;

	uint16_t time_cursor_1;
	uint16_t time_cursor_2;

	uint16_t voltage_cursor_1;
	uint16_t voltage_cursor_2;

	int32_t calculated_time;
	int32_t calculated_voltage;

	uint8_t num_cursor_flag;  //pietrwszy lub drugi

}Channel_cursors;



void drawCursorsTime(Channel_cursors * cursors, uint32_t time_per_grid, enum colors color);
void drawCursorsVoltage(Channel_cursors * cursors,  uint32_t volt_per_grid, enum colors color);


//void resetCursors();
#endif
