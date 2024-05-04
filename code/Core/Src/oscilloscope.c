/*
 * oscilloscope.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#include "stdio.h"
#include "oscilloscope.h"
#include "sprites.h"
#include "012_Open_Sans_Bold.h"
#include "014_Open_Sans_Bold.h"

#define _Open_Sans_Bold_12      &Open_Sans_Bold_12
#define _Open_Sans_Bold_14      &Open_Sans_Bold_14

void calibration(void);

void oscilloscopeInit(Oscilloscope* osc){
	oscilloscope_channel_init(&(osc->ch1), YELLOW);
	osc->ch1.isOn = 1;
	osc->ch1.number = 1;
	oscilloscope_channel_init(&(osc->ch2), BLUE);
	osc->ch2.number = 2;
	osc->timeBase_us = 1000;

	osc->ch2.y_scale_mV = 500;

	BSP_TS_Init(ILI9488_TFTHEIGHT, ILI9488_TFTWIDTH);
	ts_calib();
	calibration();

}

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color){
	ch->x_offset = 0;
	ch->y_offset = 0;
	ch->y_scale_mV= 1000;
	ch->color = color;
	ch->isOn = 0;
}


int calculate_peak_to_peak(int16_t waveform[MEMORY_DEPTH]){
	uint32_t max=0, min=4096;
	for(int i = 0; i < MEMORY_DEPTH; ++i){
		if(waveform[i]<min)
			min=waveform[i];
		if(waveform[i]>max)
			max=waveform[i];
	}
	return max-min;
}

int calculate_RMS(int16_t waveform[MEMORY_DEPTH]) {
    int64_t sum_of_squares = 0;
    for (int i = 0; i < MEMORY_DEPTH; ++i) {
        sum_of_squares += (int32_t)waveform[i] * waveform[i];
    }
    double mean_of_squares = (double)sum_of_squares / MEMORY_DEPTH;
    double rms = sqrt(mean_of_squares);
    return (int)rms;
}


void draw_waveform(Oscilloscope_channel* ch){
	for(int i = 0; i < 420; ++i)
				ch->waveform_display[i] = ch->waveform[i];
	for(int i = 0; i < 420-1; ++i){
		//ch->waveform_display[i] = ch->waveform[i];
		int x0 = i;
		int x1 = i+1;
		int y0 = CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i]/40;
		int y1 = CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i+1]/40;
		if(y0 < 32)
			y0 = 32;
		if(y0 > 284)
			y0 = 284;
		if(y1 < 32)
			y1 = 32;
		if(y1 > 284)
			y1 = 284;
		drawLine(x0, y0, x1, y1, ch->color);
	}
	// draw marker 0V
	if((CANVA_MIDDLE_V - ch->x_offset - 2 > 32) && (CANVA_MIDDLE_V - ch->x_offset - 2 < 284)){
		for(int j = 0; j < 5; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 2, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->x_offset - 1 > 32) && (CANVA_MIDDLE_V - ch->x_offset - 1 < 284)){
		for(int j = 0; j < 6; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 1, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->x_offset > 32) && (CANVA_MIDDLE_V - ch->x_offset < 284)){
		for(int j = 0; j < 7; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->x_offset + 1 > 32) && (CANVA_MIDDLE_V - ch->x_offset + 1 < 284)){
	for(int j = 0; j < 6; ++j)
		drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 1, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->x_offset + 2 > 32) && (CANVA_MIDDLE_V - ch->x_offset + 2 < 284)){
	for(int j = 0; j < 5; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 2, ch->color);
	}
}

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
	for(int i = 0; i < 441; i += 42)
		drawFastVLine(i, 32, 253, GREY);
	// draw horizontal lines
	for(int i = 32; i < 291; i += 42)
		drawFastHLine(0, i, 420, GREY);

	for(int i = 21; i < 441; i += 42)
			drawFastVLine(i, 156, 5, GREY);
	for(int i = 53; i < 291; i += 42)
			drawFastHLine(208, i, 5, GREY);
}

void drawChanellVperDev(uint16_t x, Oscilloscope_channel* ch){
	char buf[20];
	uint8_t color = ch->isOn ? ch->color : GREY;

	drawLine(x + 10, 295, x + 110, 295, color);	 		/// _______
	for(uint8_t i = 0; i < 30; ++i){
		drawLine(x + i+10, 295, x + i, 319, color);   ///  		 /
	}
	drawLine(x + 110, 295, x +  100, 319, color); 	///		   /
	drawLine(x + 0, 319,x +  100, 319, color);	   /// ________

	sprintf(buf,"%d", ch->number);
	LCD_Font(x + 16, 312, buf, _Open_Sans_Bold_14, 1, BLACK);
	sprintf(buf,"%dmV", ch->y_scale_mV);
	LCD_Font(x + 42, 312, buf, _Open_Sans_Bold_14, 1, color);
}

void serveTouchScreen(Oscilloscope* osc){
	BSP_TS_GetState(& osc->touchScreen);
	  if(osc->touchScreen.TouchDetected){
		fillRect(osc->touchScreen.X, osc->touchScreen.Y, 5, 5, RED);
	  }
}
/*
void serveTouchScreen(Oscilloscope* osc){
	BSP_TS_GetState(&(osc->touchScreen));
	if(osc->touchScreen.TouchDetected){
	  fillRect(osc->touchScreen.X, osc->touchScreen.Y, 5, 5, RED);
	  if(ts.X < 110 && ts.X > 0 && ts.Y < 320 && ts.Y > 290){	// CH1
		  if(HAL_GetTick() - prevTime > 1000){
			  prevTime = HAL_GetTick();
			  if(changed_var == 0){
				  if(!(osc->ch1.isOn)){
					  osc->ch1.isOn = 1;
				  }
				  else
					  osc->ch1.isOn = 0;
			  }
			  osc->selection = SelectionCH1;
		  }
	  }
	  else if(ts.X < 220 && ts.X > 110 && ts.Y < 320 && ts.Y > 290){	//CH2
		if(HAL_GetTick() - prevTime > 1000){
			prevTime = HAL_GetTick();
			if(changed_var == 1){
			  if(color2 == GREY){
				color2 = BLUE;
			  }
			  else
				color2 = GREY;
			}
			osc->selection = SelectionCH2;
		}
	  }
	  else if(ts.X < 70  && ts.Y < 25){			// Time per division
			changed_var = 2;
	  }
	  else if(ts.X > 440 && ts.Y < 25){			// trigger
			changed_var = 3;
	  }else if(ts.X > 425 && ts.Y < 90 && ts.Y > 50){
		  if(changed_var == 10){
			  changed_var = 11;

		  }

	  }else if(ts.X > 425 && ts.Y < 174 && ts.Y > 134){
		  if(changed_var == 11){
							  changed_var = 12;

						  }
	  }

	  else
		  changed_var = 10;
  }
}*/


void drawMenu(uint8_t color){
	drawRectangleRoundedFrame(423, 32, 56, 253, color);

	if(color == YELLOW)
		LCD_Font(437, 45, "CH 1", _Open_Sans_Bold_12, 1, color);
	else
		LCD_Font(437, 45, "CH 2", _Open_Sans_Bold_12, 1, color);

	LCD_Font(433, 74, "Scale", _Open_Sans_Bold_12, 1, color);
	drawRectangleRoundedFrame(425, 50, 52, 40, color);

	drawImageTransparent(arrowUpDown, 447, 105, 8, 15);
	drawRectangleRoundedFrame(425, 92, 52, 40, color);

	drawImageTransparent(arrowLeftRight, 443, 108 + 42, 15, 7);
	drawRectangleRoundedFrame(425, 92 + 42, 52, 40, color);
}

void drawMainMenu(uint8_t color){
	drawRectangleRoundedFrame(423, 32, 56, 253, color);

	LCD_Font(433, 45, "Menu", _Open_Sans_Bold_12, 1, color);

	LCD_Font(428, 74, "Cursor", _Open_Sans_Bold_12, 1, color);
	drawRectangleRoundedFrame(425, 50, 52, 40, color);

	LCD_Font(438, 74+42, "FFT", _Open_Sans_Bold_12, 1, color);
	drawRectangleRoundedFrame(425, 92, 52, 40, color);

	LCD_Font(426, 74+84, "Measur", _Open_Sans_Bold_12, 1, color);
	drawRectangleRoundedFrame(425, 92 + 42, 52, 40, color);
}

void drawCursorsMenu(uint8_t color){
	drawRectangleRoundedFrame(423, 32, 56, 253, color);

	LCD_Font(425, 45, "Cursors", _Open_Sans_Bold_12, 1, color);

	LCD_Font(432, 74, "Ch 1", _Open_Sans_Bold_12, 1, color);
	drawRectangleRoundedFrame(425, 50, 52, 40, color);

	drawImageTransparent(arrowLeftRight, 443, 112, 15, 7);
	drawRectangleRoundedFrame(425, 92, 52, 40, color);

	drawImageTransparent(arrowUpDown, 447, 108 + 40, 8, 15);
	drawRectangleRoundedFrame(425, 92 + 42, 52, 40, color);




}
