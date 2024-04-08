/*
 * lcd.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#include "lcd.h"


/**
 *	0, 270  420
 */

#define GRID_WIDTH 300
#define GRID_HEIGHT 240

#define TFT_WIDTH   	((uint16_t)320)
#define TFT_HEIGHT		((uint16_t)240)

uint8_t hOffset = (TFT_WIDTH - GRID_WIDTH)/2 + 100;
uint8_t vOffset = 25;
uint8_t dHeight = GRID_HEIGHT/8;


void drawGrid(){
	// draw vertical lines
	for(int i = 0; i < 441; i += 44)
		drawFastVLine(i, 26, 291-26, GREY);
	// draw horizontal lines
	for(int i = 26; i < 291; i += 44)
		drawFastHLine(0, i, 440, GREY);

	for(int i = 21; i < 441; i += 44)
			drawFastVLine(i, 156, 5, GREY);
	for(int i = 47; i < 291; i += 44)
			drawFastHLine(218, i, 5, GREY);
}

void drawChanellVperDev(uint16_t x, uint8_t color){
	drawLine(x + 10, 295, x + 110, 295, color);	 // _____
	for(uint8_t i = 0; i < 30; ++i){
		drawLine(x + i+10, 295, x + i, 319, color);   //  /
	}

	drawLine(x + 110, 295, x +  100, 319, color); //		   /
	drawLine(x + 0, 319,x +  100, 319, color);	 // ____
}
