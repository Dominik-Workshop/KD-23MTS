#include "cursors.h"
#include "ILI9488.h"
//arm_map

void drawCursorsTime(Channel_cursors * cursors, uint32_t time_per_grid, enum colors color){

	uint32_t time_1, time_2;
	//char buf[20];

	time_1 = (cursors->time_cursor_1*time_per_grid)/PX_PER_GRID_X;
	time_2 = (cursors->time_cursor_2*time_per_grid)/PX_PER_GRID_X;

	cursors->calculated_time = time_1 - time_2;

	drawFastVLine(cursors->time_cursor_1, SCOPE_Y_OFFSET, SCOPE_Y, color);
	drawFastVLine(cursors->time_cursor_2, SCOPE_Y_OFFSET, SCOPE_Y, color);

	/*sprintf(buf,"%d", cursors->calculated_time);
	LCD_Font(426, 280, buf, _Open_Sans_Bold_12, 1, color);*/
}


void drawCursorsVoltage(Channel_cursors * cursors,  uint32_t volt_per_grid, enum colors color){

	uint32_t volt_1, volt_2;
	//char buf[20];

	volt_1 = (cursors->voltage_cursor_1*volt_per_grid)/PX_PER_GRID_Y;
	volt_2 = (cursors->voltage_cursor_2*volt_per_grid)/PX_PER_GRID_Y;


	cursors->calculated_voltage  = volt_1 - volt_2;

	drawFastHLine(0, cursors->voltage_cursor_1 + SCOPE_Y_OFFSET, SCOPE_X, color);
	drawFastHLine(0, cursors->voltage_cursor_2 + SCOPE_Y_OFFSET, SCOPE_X, color);
	/*sprintf(buf,"%d", cursors->calculated_voltage);
	LCD_Font(426, 280, buf, _Open_Sans_Bold_12, 1, color);*/

}
