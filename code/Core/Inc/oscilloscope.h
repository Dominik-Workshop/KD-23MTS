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
#include "cursors.h"

extern  TS_DrvTypeDef         *ts_drv;
#define ts_calib()

#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define LCD_BRIGHTNESS 1000 // 0-1000

#define MEMORY_DEPTH  600//512
#define FFT_SIZE 64
#define CANVA_MIDDLE_V 158
#define CANVA_WIDTH 420

#define CHANNEL_OFFSET

#define ADC_RESOLUTION 4096
#define ADC_VREF 3.3

enum ChangedParameter{
	VerticalOffset,
	VerticalScale
};

enum ChangedCursor{
	CH1_TimeCursor_1,
	CH1_TimeCursor_2,
	CH1_VoltageCursor_1,
	CH1_VoltageCursor_2,
	CH2_TimeCursor_1,
	CH2_TimeCursor_2,
	CH2_VoltageCursor_1,
	CH2_VoltageCursor_2
};

enum Selection{
	SelectionCH1,
	SelectionCH2,
	SelectionTIME_BASE,
	SelectionMOVE_WAVEFORMS_HORIZONTALLY,
	SelectionTRIGGER,
	SelectionMAIN_MENU,
	SelectionCURSORS,
	SelectionCURSORS_CHANGE_CHANNEL,
	SelectionCURSORS_TIME,
	SelectionCURSORS_VOLTAGE,
	SelectionFFT,
	SelectionFFT_CHANGE_CHANNEL,
	SelectedRUN_STOP,
	Idle
};

enum ActiveCursorChannel{
	CursorChannel_1,
	CursorChannel_2
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
	ClickedRUN_STOP,
	Nothing
};

enum FFT_ActiveChannel{
	FFT_Channel_1,
	FFT_Channel_2
};



typedef struct {
    double real;
    double imag;
} Complex;



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
	Channel_cursors cursors;
	uint32_t channel_frequency;
	double fft_amplitude[FFT_SIZE/2];
	double fft_frequency[FFT_SIZE/2];
	uint32_t frequency_per_stripe;

}Oscilloscope_channel;

typedef struct osc{
	Oscilloscope_channel ch1;
	Oscilloscope_channel ch2;

	int16_t x_offset;								// horizontal offset
	uint64_t timeBase_us;

	TS_StateTypeDef touchScreen;
	enum Selection selection;
	enum ClickedItem clickedItem;

	uint32_t timeBaseArray[9];
	int8_t timeBaseIndex;

	int16_t triggerLevel_mV;

	uint8_t stop;

	enum ActiveCursorChannel active_cursor_channel;
	enum ChangedCursor changedCursor;


	enum FFT_ActiveChannel fft_active_channel;

	uint8_t is_fft_on;


}Oscilloscope;

void oscilloscopeInit(Oscilloscope* osc);

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color);
void oscilloscope_channel_toggle_on_off(Oscilloscope_channel* ch);

uint32_t calculate_peak_to_peak(uint32_t *waveform);
uint32_t calculate_RMS(uint32_t *waveform);

void draw_waveform(Oscilloscope_channel* ch, uint64_t timeBase_us, int offset, int stop);

void drawGrid();
void drawChanellVperDev(uint16_t x, Oscilloscope_channel* ch);

void displayTimeBase(Oscilloscope* osc);
void displayHorizontallOffset(Oscilloscope* osc);

void serveTouchScreen(Oscilloscope* osc);
void change_parameter(Oscilloscope_channel* ch);
void serveEncoder(Oscilloscope* osc);

void change_cursors(Oscilloscope * osc);
void drawCursorsMenu(Oscilloscope* osc);
void changeActiveCursorChannel(enum ActiveCursorChannel active_cursor_channel);
void setActiveCursorType(enum ActiveCursorType  cursor_type_to_set, enum ActiveCursorType * osc_active_cursor_type);


void drawMenu(Oscilloscope_channel* ch);
void drawMainMenuButton();
void drawMainMenu(uint8_t color);

void drawFFTMenu(Oscilloscope* osc);
void calculateFFT(Oscilloscope_channel * ch, uint32_t sampling_frequency);
void fft(Complex *X, int N);
void drawFFTgraph(Oscilloscope_channel * ch);

void drawTriggerMenu(Oscilloscope* osc);
void drawMeasurements(Oscilloscope* osc);
void drawRunStop(Oscilloscope* osc);
void drawTriggerIcon(Oscilloscope* osc);
void drawChannels0Vmarkers(Oscilloscope_channel* ch);
float convertAdcToVoltage(uint16_t adcValue);

#endif
