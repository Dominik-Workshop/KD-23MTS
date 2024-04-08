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
#include "lcd.h"



#define MEMORY_DEPTH  480//512

typedef struct oscilloscope_ch{
	uint16_t waveform[MEMORY_DEPTH];				// all samples

	int16_t x_offset;								// horizontal offset
	int16_t y_offset;								// vertical offset (offset = 0 => 0V = middle of the display)

	int16_t y_scale;								// vertical scale [V/div]

	uint16_t waveform_display[LCD_WIDTH];			// section of waveform currently displayed
	uint8_t color;

}oscilloscope_channel;

void oscilloscope_channel_init(oscilloscope_channel* ch, uint8_t color);
int calculate_peak_to_peak(int16_t waveform[MEMORY_DEPTH]);
int calculate_RMS(int16_t waveform[MEMORY_DEPTH]);

void draw_waveform(oscilloscope_channel* ch);


#endif
