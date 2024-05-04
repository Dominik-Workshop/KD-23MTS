#include "cursors.h"
#include "ILI9488.h"
//arm_map

void resetCursors(){
	time_cursor_1 = 0;
	time_cursor_2 = SCOPE_X;
	voltage_cursor_1 = 0;
	voltage_cursor_2 = SCOPE_Y;
}


int measureTime(uint32_t time_per_grid){

	int32_t time_1, time_2,time;

	time_1 = (time_cursor_1*time_per_grid)/PX_PER_GRID_X;
	time_2 = (time_cursor_2*time_per_grid)/PX_PER_GRID_X;

	time  = time_1 - time_2;


	return time;
}



int drawCursorsTime(int32_t time_per_grid, uint8_t curs_num_flag, int32_t encoder){

	if(curs_num_flag == 0){

		time_cursor_1 = encoder;

		if(time_cursor_1 > SCOPE_X){time_cursor_1 = SCOPE_X;}
		if(time_cursor_1 < 0){time_cursor_1 = 0;}
	}

	if(curs_num_flag == 1){

		time_cursor_2 = encoder;

		if(time_cursor_2 > SCOPE_X){time_cursor_2 = SCOPE_X;}
		if(time_cursor_2 < 0){time_cursor_2 = 0;}

	}

	drawFastVLine(time_cursor_1, SCOPE_Y_OFFSET, SCOPE_Y, YELLOW);
	drawFastVLine(time_cursor_2, SCOPE_Y_OFFSET, SCOPE_Y, YELLOW);


	return measureTime(time_per_grid);
}


int measureVoltage(uint32_t volt_per_grid){

	uint32_t volt_1, volt_2,volt;

	volt_1 = (voltage_cursor_1*volt_per_grid)/PX_PER_GRID_Y;
	volt_2 = (voltage_cursor_2*volt_per_grid)/PX_PER_GRID_Y;

	volt  = volt_1 - volt_2;
	return volt;
}

int drawCursorsVoltage(int32_t volt_per_grid, uint8_t curs_num_flag, int32_t encoder){


	if(curs_num_flag == 0){

		voltage_cursor_1 = encoder;

			if(voltage_cursor_1 > SCOPE_Y){voltage_cursor_1 = SCOPE_Y;}
			if(voltage_cursor_1 < 0){voltage_cursor_1 = 0;}
		}

		if(curs_num_flag == 1){

			voltage_cursor_2 = encoder;
			if(voltage_cursor_2 > SCOPE_Y){voltage_cursor_2 = SCOPE_Y;}
			if(voltage_cursor_2 < 0){voltage_cursor_2 = 0;}

		}

	drawFastHLine(0, voltage_cursor_1 + SCOPE_Y_OFFSET, SCOPE_X, RED);
	drawFastHLine(0, voltage_cursor_2 + SCOPE_Y_OFFSET, SCOPE_X, RED);

	return measureVoltage(volt_per_grid);
}