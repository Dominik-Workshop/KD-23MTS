/*
 * lcd.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#include "lcd.h"

#define GRID_WIDTH 300
#define GRID_HEIGHT 240

#define TFT_WIDTH   	((uint16_t)320)
#define TFT_HEIGHT		((uint16_t)240)
uint8_t hOffset = (TFT_WIDTH - GRID_WIDTH)/2;
uint8_t vOffset = (TFT_HEIGHT - GRID_HEIGHT)/2;
uint8_t dHeight = GRID_HEIGHT/8;

void drawGrid(){
	/*
	// vertical lines
	for(int i = 0; i < 480; i+=60){
		for(int j = 20; j < 320; j+=2)
			drawPixel(i, j, ILI9488_DARKGREY);
	}
	for(int j = 20; j < 320; j+=2)
				drawPixel(479, j, ILI9488_DARKGREY);

	// horizontal lines
	for(int i = 20; i < 320; i+=60){
		for(int j = 0; j < 480; j+=2)
			drawPixel(j, i, ILI9488_DARKGREY);
	}
	for(int j = 0; j < 480; j+=2)
				drawPixel(j, 319, ILI9488_DARKGREY);
				*/
	uint8_t hPacing = GRID_WIDTH / 12;
		uint8_t vPacing = GRID_HEIGHT / 8;
	for(int i = 1; i < 12; i++)
			drawFastVLine(i * hPacing + hOffset, vOffset, GRID_HEIGHT, 1);

		for(int i = 1; i < 8; i++)
			drawFastHLine(hOffset, i * vPacing + vOffset, GRID_WIDTH, 1);

		//for(int i = 1; i < 2*8; i++)
		//	drawFastHLine(hOffset + GRID_WIDTH/2 - 3, i * vPacing/2 + vOffset, 7, ILI9488_DARKGREY);

		//for(int i = 1; i < 2*12; i++)
		//	drawFastVLine(i * hPacing/2 + hOffset, vOffset + GRID_HEIGHT/2 - 4, 7, ILI9488_DARKGREY);
}

