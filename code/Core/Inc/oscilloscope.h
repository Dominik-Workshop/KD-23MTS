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

#define MEMORY_DEPTH  600//512

#define CANVA_MIDDLE_V 158
#define CANVA_WIDTH 420

#define CHANNEL_OFFSET

#define ADC_RESOLUTION 4096
#define ADC_VREF 3.3

enum ChangedParameter{
	VerticalOffset,
	VerticalScale
};


enum Selection{
	SelectionCH1,
	SelectionCH2,
	SelectionMOVE_WAVEFORMS_HORIZONTALLY,
	SelectionTIME_BASE,
	SelectionTRIGGER,
	SelectionMAIN_MENU,
	SelectionCURSORS,
	SelectionFFT,
	Idle
};

enum ClickedItem{
	ClickedCH1,
	ClickedCH2,
	ClickedMOVE_WAVEFORMS_HORIZONTALLY,
	ClickedTIME_BASE,
	ClickedTRIGGER,
	ClickedMAIN_MENU,
	ClickedCURSORS,
	ClickedFFT,
	Nothing
};

typedef struct osc_ch{
	uint16_t waveform_raw_adc[MEMORY_DEPTH];		// all samples

	int16_t y_offset;								// vertical offset (offset = 0 => 0V = middle of the display)

	uint16_t y_scale_mV;							// vertical scale [mV/div]
	uint32_t y_scale_mVArray[10];
	int8_t y_scale_mVIndex;

	uint32_t waveform_display[LCD_WIDTH];			// section of waveform currently displayed [mV]
	uint8_t color;

	uint8_t isOn;
	uint8_t number;									// number of channel for identification

	enum ChangedParameter changedParameter;

}Oscilloscope_channel;

typedef struct osc{
	Oscilloscope_channel ch1;
	Oscilloscope_channel ch2;

	int16_t x_offset;								// horizontal offset
	uint64_t timeBase_us;

	TS_StateTypeDef touchScreen;
	enum Selection selection;
	enum ClickedItem clickedItem;

	uint32_t timeBaseArray[22];
	int8_t timeBaseIndex;

	int16_t triggerLevel_mV;

	uint8_t stop;
}Oscilloscope;

void oscilloscopeInit(Oscilloscope* osc);

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color);
void oscilloscope_channel_toggle_on_off(Oscilloscope_channel* ch);

uint32_t calculate_peak_to_peak(uint32_t *waveform);
uint32_t calculate_RMS(uint32_t *waveform);

void draw_waveform(Oscilloscope_channel* ch, uint64_t timeBase_us, int offset);

void drawGrid();
void drawChanellVperDev(uint16_t x, Oscilloscope_channel* ch);

void displayTimeBase(Oscilloscope* osc);
void displayHorizontallOffset(Oscilloscope* osc);

void serveTouchScreen(Oscilloscope* osc);
void change_parameter(Oscilloscope_channel* ch);
void serveEncoder(Oscilloscope* osc);

void drawMenu(Oscilloscope_channel* ch);
void drawMainMenu(uint8_t color);
void drawCursorsMenu(uint8_t color);
void drawFFTMenu(Oscilloscope* osc);
void drawTriggerMenu(Oscilloscope* osc);
void drawMeasurements(Oscilloscope* osc);
void drawRunStop(Oscilloscope* osc);
void drawTriggerIcon(Oscilloscope* osc);
void drawChannels0Vmarkers(Oscilloscope_channel* ch);
float convertAdcToVoltage(uint16_t adcValue);

#endif
