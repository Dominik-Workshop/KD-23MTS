/*
 * oscilloscope.h
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */


#ifndef INC_OSCILLOSCOPE_H_
#define INC_OSCILLOSCOPE_H_

#include "main.h"
#include "tim.h"
#include "inttypes.h"

#include "ili9488.h"
#include "ts.h"
#include "stm32_adafruit_ts.h"

extern  TS_DrvTypeDef         *ts_drv;
#define ts_calib()

#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define LCD_BRIGHTNESS 1000 // 0-1000

#define MEMORY_DEPTH  480//512

#define CANVA_MIDDLE_V 158


#define MEMORY_DEPTH  480//512

enum Selection{
	SelectionCH1,
	SelectionCH2,
	SelectionTIME_BASE,
	SelectionTRIGGER,
	SelectionMAIN_MENU,
	SelectionCURSORS,
	SelectionFFT
};

typedef struct osc_ch{
	uint16_t waveform[MEMORY_DEPTH];				// all samples

	int16_t x_offset;								// horizontal offset
	int16_t y_offset;								// vertical offset (offset = 0 => 0V = middle of the display)

	uint16_t y_scale_mV;							// vertical scale [mV/div]

	uint16_t waveform_display[LCD_WIDTH];			// section of waveform currently displayed
	uint8_t color;

	uint8_t isOn;
	uint8_t number;									// number of channel for identification

}Oscilloscope_channel;

typedef struct osc{
	Oscilloscope_channel ch1;
	Oscilloscope_channel ch2;

	uint64_t timeBase_us;

	TS_StateTypeDef touchScreen;
	enum Selection selection;
}Oscilloscope;

void oscilloscopeInit(Oscilloscope* osc);

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color);
int calculate_peak_to_peak(int16_t waveform[MEMORY_DEPTH]);
int calculate_RMS(int16_t waveform[MEMORY_DEPTH]);

void draw_waveform(Oscilloscope_channel* ch);

void drawGrid();
void drawChanellVperDev(uint16_t x, Oscilloscope_channel* ch);

void serveTouchScreen(Oscilloscope* osc);

void drawMenu(uint8_t color);
void drawMainMenu(uint8_t color);
void drawCursorsMenu(uint8_t color);

#endif
