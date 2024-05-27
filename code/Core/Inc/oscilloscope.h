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

enum ChangedParameter{
	HorizontalOffset,
	VerticalOffset,
	VerticalScale
};


enum Selection{
	SelectionCH1,
	SelectionCH2,
	SelectionTIME_BASE,
	SelectionTRIGGER,
	SelectionMAIN_MENU,
	SelectionCURSORS,
	SelectionCURSORS_CHANGE_CHANNEL,
	SelectionCURSORS_VERTICAL,
	SelectionCURSORS_HORIZONTAL,
	SelectionFFT,
	SelectionMEASUREMENTS,
	Idle
};

enum ClickedItem{
	ClickedCH1,
	ClickedCH2,
	ClickedTIME_BASE,
	ClickedTRIGGER,
	ClickedMAIN_MENU,
	ClickedCURSORS,
	ClickedFFT,
	Nothing
};

enum ActiveCursorChannel{
	CursorChannel_1,
	CursorChannel_2
};

enum ActiveCursorType{
	CursorType_VERTICAL,
	CursorType_HORIZONTAL,
	CursorType_DISABLE
};

typedef struct cursors{

	int32_t vertical_cursor_1;
	int32_t vertical_cursor_2;
	int32_t horizontal_cursor_1;
	int32_t horizontal_cursor_2;

}Channel_cursors;

typedef struct osc_ch{
	uint16_t waveform[MEMORY_DEPTH];				// all samples

	int16_t x_offset;								// horizontal offset
	int16_t y_offset;								// vertical offset (offset = 0 => 0V = middle of the display)

	uint16_t y_scale_mV;							// vertical scale [mV/div]

	uint16_t waveform_display[LCD_WIDTH];			// section of waveform currently displayed
	uint8_t color;

	uint8_t isOn;
	uint8_t number;									// number of channel for identification

	enum ChangedParameter changedParameter;

	Channel_cursors cursors;


}Oscilloscope_channel;




typedef struct osc{
	Oscilloscope_channel ch1;
	Oscilloscope_channel ch2;

	uint64_t timeBase_us;

	TS_StateTypeDef touchScreen;
	enum Selection selection;
	enum ClickedItem clickedItem;

	uint32_t timeBaseArray[22];
	int8_t timeBaseIndex;

	enum ActiveCursorChannel active_cursor_channel;
	enum ActiveCursorType active_cursor_type;

}Oscilloscope;

void oscilloscopeInit(Oscilloscope* osc);

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color);
void oscilloscope_channel_toggle_on_off(Oscilloscope_channel* ch);
int calculate_peak_to_peak(int16_t waveform[MEMORY_DEPTH]);
int calculate_RMS(int16_t waveform[MEMORY_DEPTH]);

void draw_waveform(Oscilloscope_channel* ch);

void drawGrid();
void drawChanellVperDev(uint16_t x, Oscilloscope_channel* ch);

void displayTimeBase(Oscilloscope* osc);

void serveTouchScreen(Oscilloscope* osc);
void change_parameter(Oscilloscope_channel* ch);
void serveEncoder(Oscilloscope* osc);

void drawMenu(Oscilloscope_channel* ch);
void drawMainMenu(uint8_t color);


void drawCursorsMenu(enum ActiveCursorChannel active_cursor_channel, enum ActiveCursorType active_cursor_type);
void changeActiveCursorChannel(enum ActiveCursorChannel * active_cursor_channel);
//void disableActiveCursorChannel(enum ActiveCursorChannel * active_cursor_channel);

void setActiveCursorType(enum ActiveCursorType  cursor_type_to_set, enum ActiveCursorType * osc_active_cursor_type);



void drawFFTMenu(Oscilloscope* osc);
void drawMeasurements(Oscilloscope* osc);

#endif
