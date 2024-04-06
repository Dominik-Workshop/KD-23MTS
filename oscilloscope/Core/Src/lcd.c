/*
 * lcd.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#include "lcd.h"

void drawGrid(){
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
}

