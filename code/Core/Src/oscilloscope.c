/*
 * oscilloscope.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#include "stdio.h"
#include "oscilloscope.h"
#include "sprites.h"
#include "memory.h"
#include "012_Open_Sans_Bold.h"
#include "014_Open_Sans_Bold.h"

#define _Open_Sans_Bold_12      &Open_Sans_Bold_12
#define _Open_Sans_Bold_14      &Open_Sans_Bold_14

void touchScreenCalibration(void);

void oscilloscopeInit(Oscilloscope* osc){
	oscilloscope_channel_init(&(osc->ch1), YELLOW);
	osc->ch1.isOn = 1;
	osc->ch1.number = 1;
	oscilloscope_channel_init(&(osc->ch2), BLUE);
	osc->ch2.number = 2;
	osc->timeBase_us = 1000;
	osc->selection = SelectionCH1;
	osc->clickedItem = Nothing;
	osc->ch2.y_scale_mV = 500;
	osc->timeBaseIndex = 9;
	osc->ch1.y_scale_mVIndex = 6;
	osc->ch2.y_scale_mVIndex = 6;
	osc->ch2.y_scale_mV = 1000;
	osc->stop = 0;
	uint32_t y_scale_mVArray[] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
	memcpy(osc->ch1.y_scale_mVArray, y_scale_mVArray, sizeof(y_scale_mVArray));
	memcpy(osc->ch2.y_scale_mVArray, y_scale_mVArray, sizeof(y_scale_mVArray));
	uint32_t timeBaseArray[] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000, 10000000};
	memcpy(osc->timeBaseArray, timeBaseArray, sizeof(timeBaseArray));

	osc->triggerLevel_px_formZero = 0;
	BSP_TS_Init(ILI9488_TFTHEIGHT, ILI9488_TFTWIDTH);
	ts_calib();
	//touchScreenCalibration();
}

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color){
	ch->x_offset = 0;
	ch->y_offset = 0;
	ch->y_scale_mV= 1000;
	ch->color = color;
	ch->isOn = 0;
	ch->changedParameter = VerticalScale;
}

void oscilloscope_channel_toggle_on_off(Oscilloscope_channel* ch){
	if(ch->isOn)
		ch->isOn = 0;
	else
		ch->isOn = 1;
}
/*
int calculate_peak_to_peak(uint32_t waveform[LCD_WIDTH]){
	uint32_t max=0, min=4294967295;
	for(int i = 0; i < LCD_WIDTH; ++i){
		if(waveform[i]<min)
			min=waveform[i];
		if(waveform[i]>max)
			max=waveform[i];
	}
	return (int)(max-min);
}

int calculate_RMS(uint32_t waveform[LCD_WIDTH]) {
    int64_t sum_of_squares = 0;
    for (int i = 0; i < LCD_WIDTH; ++i) {
        sum_of_squares += (int32_t)waveform[i] * waveform[i];
    }
    double mean_of_squares = (double)sum_of_squares / LCD_WIDTH;
    double rms = sqrt(mean_of_squares);
    return (int)rms;
}*/

int calculate_peak_to_peak(uint16_t waveform[LCD_WIDTH]){
	uint16_t max=0, min;
	min = 50000;
	for(int i = 0; i < LCD_WIDTH; ++i){
		if(waveform[i]<min)
			min=waveform[i];
		if(waveform[i]>max)
			max=waveform[i];
	}
	return max-min;
}

int calculate_RMS(uint16_t waveform[LCD_WIDTH]) {
    int64_t sum_of_squares = 0;
    for (int i = 0; i < LCD_WIDTH; ++i) {
        sum_of_squares += (int32_t)waveform[i] * waveform[i];
    }
    double mean_of_squares = (double)sum_of_squares / LCD_WIDTH;
    double rms = sqrt(mean_of_squares);
    return (int)rms;
}


void draw_waveform(Oscilloscope_channel* ch){
	for(int i = 0; i < 420; ++i){
		if(ch->number == 1)
			ch->waveform_display[i] = convertAdcToVoltage(ch->waveform_raw_adc[i+37])*1000;
		else
			ch->waveform_display[i] = convertAdcToVoltage(ch->waveform_raw_adc[i])*1000;
	}
	for(int i = 0; i < 420-1; ++i){
		//ch->waveform_display[i] = ch->waveform[i];
		int x0 = i;
		int x1 = i+1;
		int y0 = CANVA_MIDDLE_V - ch->y_offset - (ch->waveform_display[i]*42)/ch->y_scale_mV;
		int y1 = CANVA_MIDDLE_V - ch->y_offset - (ch->waveform_display[i+1]*42)/ch->y_scale_mV;
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
	if((CANVA_MIDDLE_V - ch->y_offset - 2 > 32) && (CANVA_MIDDLE_V - ch->y_offset - 2 < 284)){
		for(int j = 0; j < 5; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->y_offset - 2, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->y_offset - 1 > 32) && (CANVA_MIDDLE_V - ch->y_offset - 1 < 284)){
		for(int j = 0; j < 6; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->y_offset - 1, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->y_offset > 32) && (CANVA_MIDDLE_V - ch->y_offset < 284)){
		for(int j = 0; j < 7; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->y_offset, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->y_offset + 1 > 32) && (CANVA_MIDDLE_V - ch->y_offset + 1 < 284)){
	for(int j = 0; j < 6; ++j)
		drawPixel(j, CANVA_MIDDLE_V - ch->y_offset + 1, ch->color);
	}

	if((CANVA_MIDDLE_V - ch->y_offset + 2 > 32) && (CANVA_MIDDLE_V - ch->y_offset + 2 < 284)){
	for(int j = 0; j < 5; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->y_offset + 2, ch->color);
	}
}

void drawGrid(){
	// vertical lines
	for(int i = 0; i < 441; i += 42)
		drawFastVLine(i, 32, 253, GREY);
	// horizontal lines
	for(int i = 32; i < 291; i += 42)
		drawFastHLine(0, i, 420, GREY);

	// short lines along the center lines
	for(int i = 21; i < 441; i += 42)
			drawFastVLine(i, 156, 5, GREY);
	for(int i = 53; i < 291; i += 42)
			drawFastHLine(208, i, 5, GREY);
}

void drawChanellVperDev(uint16_t x, Oscilloscope_channel* ch){
	char buf[20];
	uint8_t colorFrame = ch->isOn ? ch->color : GREY;
	uint8_t colorVoltage = ch->isOn ? WHITE : GREY;

	drawLine(x + 10, 295, x + 110, 295, colorFrame);	 		/// _______
	for(uint8_t i = 0; i < 30; ++i){
		drawLine(x + i+10, 295, x + i, 319, colorFrame);   ///  		 /
	}
	drawLine(x + 110, 295, x +  100, 319, colorFrame); 	///		   /
	drawLine(x + 0, 319,x +  100, 319, colorFrame);	   /// ________

	sprintf(buf,"%d", ch->number);
	LCD_Font(x + 16, 312, buf, _Open_Sans_Bold_14, 1, BLACK);


	if(ch->y_scale_mV < 1000){
	  sprintf(buf,"%dmV", (int) ch->y_scale_mV);
	}
	else
		sprintf(buf,"%dV", (int) ch->y_scale_mV/1000);
	LCD_Font(x + 42, 312, buf, _Open_Sans_Bold_14, 1, colorVoltage);
}

void displayTimeBase(Oscilloscope* osc){
	char buf[20];
	if(osc->timeBase_us < 1000){
	  sprintf(buf,"H %dus", (int) osc->timeBase_us);
	}
	else if(osc->timeBase_us < 1000000)
		sprintf(buf,"H %dms", (int) osc->timeBase_us/1000);
	else
		sprintf(buf,"H %ds", (int) osc->timeBase_us/1000000);

	LCD_Font(8, 19, buf, _Open_Sans_Bold_12  , 1, WHITE);
	if(osc->selection == SelectionTIME_BASE)
		drawRectangleRoundedFrame(3, 3, 70, 22, WHITE);
	else
		drawRectangleRoundedFrame(3, 3, 70, 22, GREY);
}

void serveTouchScreen(Oscilloscope* osc){
	BSP_TS_GetState(& osc->touchScreen);
	if(osc->touchScreen.TouchDetected){
		fillRect(osc->touchScreen.X, osc->touchScreen.Y, 5, 5, RED);

		if(osc->touchScreen.X < 110 && osc->touchScreen.X > 0 && osc->touchScreen.Y < 320 && osc->touchScreen.Y > 290){	// CH1
			if(osc->clickedItem != ClickedCH1){
				if(osc->selection == SelectionCH1)
					oscilloscope_channel_toggle_on_off(& osc->ch1);
				else
					osc->ch1.isOn = 1;
				osc->clickedItem = ClickedCH1;
			}
			osc->selection = SelectionCH1;
		}
		else if(osc->touchScreen.X < 220 && osc->touchScreen.X > 110 && osc->touchScreen.Y < 320 && osc->touchScreen.Y > 290){	//CH2
			if(osc->clickedItem != ClickedCH2){
				if(osc->selection == SelectionCH2)
					oscilloscope_channel_toggle_on_off(& osc->ch2);
				else
					osc->ch2.isOn = 1;
				osc->clickedItem = ClickedCH2;
			}
			osc->selection = SelectionCH2;
		}

		else if(osc->touchScreen.X < 70  && osc->touchScreen.Y < 25){			// Time per division
			osc->selection = SelectionTIME_BASE;
		}

		else if(osc->touchScreen.X > 400  && osc->touchScreen.Y < 25){			// Trigger
			osc->selection = SelectionTRIGGER;
		}

		else if(osc->touchScreen.X < 420 && osc->touchScreen.Y > 32 && osc->touchScreen.Y < 284){
			osc->selection = SelectionMAIN_MENU;
		}

		else if(osc->selection == SelectionCH1){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (68+40) && osc->touchScreen.Y > 68){	// CH1 scale vertically
				osc->ch1.changedParameter = VerticalScale;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (129+40) && osc->touchScreen.Y > 129){	// CH1 move vertically
				osc->ch1.changedParameter = VerticalOffset;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (171+40) && osc->touchScreen.Y > 171){	// CH1 move horizontally
				osc->ch1.changedParameter = HorizontalOffset;
			}
		}

		else if(osc->selection == SelectionCH2){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (68+40) && osc->touchScreen.Y > 68){	// CH2 scale vertically
				osc->ch2.changedParameter = VerticalScale;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (129+40) && osc->touchScreen.Y > 129){	// CH2 move vertically
				osc->ch2.changedParameter = VerticalOffset;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (171+40) && osc->touchScreen.Y > 171){	// CH2 move horizontally
				osc->ch2.changedParameter = HorizontalOffset;
			}
		}

		else if(osc->selection == SelectionMAIN_MENU){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (50+40) && osc->touchScreen.Y > 50){
				osc->selection = SelectionCURSORS;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (92+40) && osc->touchScreen.Y > 92){
				osc->selection = SelectionFFT;
			}
		}

	}else
		osc->clickedItem = Nothing;

	switch(osc->selection){
	case SelectionCH1:
		drawMenu(& osc->ch1);
		break;
	case SelectionCH2:
		drawMenu(& osc->ch2);
		break;
	case SelectionMAIN_MENU:
		drawMainMenu(LIGHT_GREEN);
		break;
	case SelectionCURSORS:
		drawCursorsMenu(GREY);
		break;
	case SelectionFFT:
		drawFFTMenu(osc);
		break;
	case Idle:
		break;
	}
}

void change_parameter(Oscilloscope_channel* ch){
    switch(ch->changedParameter){
        case VerticalScale:
        	ch->y_scale_mVIndex += htim1.Instance->CNT;
    		if(ch->y_scale_mVIndex < 0)
    			ch->y_scale_mVIndex = 0;
    		if(ch->y_scale_mVIndex > 9)
    			ch->y_scale_mVIndex = 9;
            ch->y_scale_mV = ch->y_scale_mVArray[ch->y_scale_mVIndex];
            break;
        case VerticalOffset:
            ch->y_offset -= htim1.Instance->CNT;
    		if(HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == 0){
    			ch->y_offset = 0;
    		}
            break;
        case HorizontalOffset:
            ch->x_offset -= htim1.Instance->CNT;
            break;
    }
}

void serveEncoder(Oscilloscope* osc){
	switch(osc->selection){
	case SelectionCH1:
		if(! osc->ch1.isOn)
			break;
		change_parameter(&osc->ch1);
		break;
	case SelectionCH2:
		if(! osc->ch2.isOn)
			break;
		change_parameter(&osc->ch2);
		break;
	case SelectionCURSORS:
		break;
	case SelectionTIME_BASE:
		osc->timeBaseIndex -= htim1.Instance->CNT;
		if(osc->timeBaseIndex < 0)
			osc->timeBaseIndex = 0;
		if(osc->timeBaseIndex > 21)
			osc->timeBaseIndex = 21;
		osc->timeBase_us = osc->timeBaseArray[osc->timeBaseIndex];
		break;
	case SelectionTRIGGER:
		osc->triggerLevel_px_formZero -= htim1.Instance->CNT;
		break;
	case Idle:
		break;
	}
	htim1.Instance->CNT=0;
}


void drawMenu(Oscilloscope_channel* ch){
	char buf[10];
	uint8_t color1 = GREY;
	uint8_t color2 = GREY;
	uint8_t color3 = GREY;
	switch(ch->changedParameter){
	case VerticalScale:
		color1 = WHITE;
		break;
	case VerticalOffset:
		color2 = WHITE;
		break;
	case HorizontalOffset:
		color3 = WHITE;
		break;
	}
	drawRectangleRoundedFrame(423, 32, 56, 253, ch->color);

	sprintf(buf,"Ch %d", ch->number);
	LCD_Font(437, 45, buf, _Open_Sans_Bold_12, 1, WHITE);

	drawFastHLine(428, 48, 46, ch->color);

	LCD_Font(433, 64, "Scale", _Open_Sans_Bold_12, 1, WHITE);
	drawImageTransparentColored(447, 81,  8, 15, arrowUpDown, color1);
	drawRectangleRoundedFrame(  425, 68, 52, 40, ch->color);

	LCD_Font(433, 125, "Move", _Open_Sans_Bold_12, 1, WHITE);
	drawImageTransparentColored(447, 142,  8, 15, arrowUpDown, color2);
	drawRectangleRoundedFrame(  425, 129, 52, 40, ch->color);
	drawImageTransparentColored(443, 187, 15, 7, arrowLeftRight, color3);
	drawRectangleRoundedFrame(  425, 171, 52, 40, ch->color);
}

void drawMainMenu(uint8_t color){
	drawRectangleRoundedFrame(423, 32, 56, 253, color);

	LCD_Font(433, 45, "Menu", _Open_Sans_Bold_12, 1, WHITE);

	LCD_Font(428, 74, "Cursor", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(425, 50, 52, 40, color);

	LCD_Font(438, 74+42, "FFT", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(425, 92, 52, 40, color);
}

void drawCursorsMenu(uint8_t color){
	drawRectangleRoundedFrame(423, 32, 56, 253, color);

	LCD_Font(426, 46, "Cursors", _Open_Sans_Bold_12, 1, WHITE);

	LCD_Font(436, 74, "Ch 1", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(425, 50, 52, 40, color);

	drawImageTransparent(443, 110, 15, 7, arrowLeftRight);
	drawRectangleRoundedFrame(425, 92, 52, 40, color);

	drawImageTransparent(447, 108 + 40, 8, 15, arrowUpDown);
	drawRectangleRoundedFrame(425, 92 + 42, 52, 40, color);

}

void drawFFTMenu(Oscilloscope* osc){
	drawRectangleRoundedFrame(423, 32, 56, 253, DARK_PINK);
	LCD_Font(440, 45, "FFT", _Open_Sans_Bold_12, 1, WHITE);

	drawFastHLine(428, 48, 46, DARK_PINK);

	LCD_Font(429, 64, "Source", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(  425, 68, 52, 40, DARK_PINK);
	LCD_Font(437, 93, "Ch 1", _Open_Sans_Bold_12, 1, WHITE);
}

void drawMeasurements(Oscilloscope* osc){
	char buf[20];
	if(osc->ch1.isOn){
		sprintf(buf,"Vpp=%dmV", calculate_peak_to_peak((uint16_t *)osc->ch1.waveform_display));
		LCD_Font(90, 13, buf, _Open_Sans_Bold_12, 1, osc->ch1.color);
		sprintf(buf,"Vrms=%dmV", calculate_RMS((uint16_t *)osc->ch1.waveform_display));
		LCD_Font(190, 13, buf, _Open_Sans_Bold_12, 1, osc->ch1.color);
	}
	if(osc->ch2.isOn){
		sprintf(buf,"Vpp=%dmV", calculate_peak_to_peak((uint16_t *)osc->ch2.waveform_display));
		LCD_Font(90, 27, buf, _Open_Sans_Bold_12, 1, osc->ch2.color);
		sprintf(buf,"Vrms=%dmV", calculate_RMS((uint16_t *)osc->ch2.waveform_display));
		LCD_Font(190, 27, buf, _Open_Sans_Bold_12, 1, osc->ch2.color);
	}
}

void drawRunStop(Oscilloscope* osc){
	drawRectangleRoundedFrame(399, 288, 80, 28, GREY);
	if(osc->stop)
		LCD_Font(480 - 50, 307, "STOP", _Open_Sans_Bold_14, 1, RED);
	else
		LCD_Font(402, 307, "RUNNING", _Open_Sans_Bold_14, 1, LIGHT_GREEN);

}

void drawTriggerIcon(Oscilloscope* osc){
	char buf[20];
	drawRectangleRoundedFrame(399, 3, 80, 22, GREY);
	drawImageTransparent(405, 7, 9, 15, trigRisingIcon);
	fillRect(417, 7, 15, 16, YELLOW);
	LCD_Font(420, 20, "1", _Open_Sans_Bold_14, 1, BLACK);
	sprintf(buf, "%.2fV", osc->triggerLevel_px_formZero * osc->ch1.y_scale_mV / 42.0 / 1000.0);
	LCD_Font(433, 20, buf, _Open_Sans_Bold_14, 1, YELLOW);

	drawFastHLine(0, CANVA_MIDDLE_V - osc->ch1.y_offset - osc->triggerLevel_px_formZero, 420, RED);
	//drawPixel(2, CANVA_MIDDLE_V - osc->ch1.y_offset - osc->triggerLevel_px_formZero, RED);
}

float convertAdcToVoltage(uint16_t adcValue){
	return adcValue*ADC_VREF/ADC_RESOLUTION;
}
