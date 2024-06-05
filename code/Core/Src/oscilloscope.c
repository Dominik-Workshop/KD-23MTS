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
	osc->timeBase_us = 160;
	osc->selection = SelectionCH1;
	osc->clickedItem = Nothing;
	osc->ch2.y_scale_mV = 500;
	osc->timeBaseIndex = 4;
	osc->ch1.y_scale_mVIndex = 6;
	osc->ch2.y_scale_mVIndex = 6;
	osc->ch2.y_scale_mV = 1000;
	osc->stop = 0;
	osc->x_offset = 0;
	uint32_t y_scale_mVArray[] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
	memcpy(osc->ch1.y_scale_mVArray, y_scale_mVArray, sizeof(y_scale_mVArray));
	memcpy(osc->ch2.y_scale_mVArray, y_scale_mVArray, sizeof(y_scale_mVArray));
	uint32_t timeBaseArray[] = {10, 20, 40, 80, 160, 320, 640, 1280, 2560};
	memcpy(osc->timeBaseArray, timeBaseArray, sizeof(timeBaseArray));

	osc->triggerLevel_mV = 1500;
	BSP_TS_Init(ILI9488_TFTHEIGHT, ILI9488_TFTWIDTH);
	ts_calib();
	//touchScreenCalibration();

	osc -> active_cursor_channel = CursorChannel_1;
	osc -> changedCursor = CH1_TimeCursor_1;

	osc -> fft_active_channel = FFT_Channel_1;
	osc->is_fft_on = 0;
}

void oscilloscope_channel_init(Oscilloscope_channel* ch, uint8_t color){
	ch->y_offset = 0;
	ch->y_scale_mV= 1000;
	ch->color = color;
	ch->isOn = 0;
	ch->changedParameter = VerticalScale;
	ch->cursors.time_cursor_1 = 100;
	ch->cursors.time_cursor_2 = 200;
	ch->cursors.voltage_cursor_1 = 100;
	ch->cursors.voltage_cursor_2 = 200;
	ch->cursors.cursor_type = CursorType_DISABLE;
	ch->cursors.num_cursor_flag = 1;

}

void oscilloscope_channel_toggle_on_off(Oscilloscope_channel* ch){
	if(ch->isOn)
		ch->isOn = 0;
	else
		ch->isOn = 1;
}

uint32_t calculate_peak_to_peak(uint32_t *waveform){
	uint32_t max=0, min;
	min = 50000;
	for(int i = 0; i < CANVA_WIDTH; ++i){
		if(waveform[i]<min){
			min=waveform[i];
		}
		if(waveform[i]>max){
			max=waveform[i];
		}
	}
	return max-min;
}

uint32_t calculate_RMS(uint32_t *waveform) {
    uint64_t sum_of_squares = 0;
    for (int i = 0; i < CANVA_WIDTH; ++i) {
        sum_of_squares += waveform[i] * waveform[i];
    }
    float mean_of_squares = (float)sum_of_squares / CANVA_WIDTH;
    float rms = sqrtf(mean_of_squares);
    return rms;
}


void draw_waveform(Oscilloscope_channel* ch, uint64_t timeBase_us, int offset, int stop){
	const uint offsetBetweenChannels = 360;
	for(int i = 0; i < CANVA_WIDTH; ++i){
		if(ch->number == 1)
			ch->waveform_display[i] = convertAdcToVoltage(ch->waveform_raw_adc[i+offset+(offsetBetweenChannels/timeBase_us)])*1000;
		else
			ch->waveform_display[i] = convertAdcToVoltage(ch->waveform_raw_adc[i+offset])*1000;
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
}

void drawChannels0Vmarkers(Oscilloscope_channel* ch){
	if(!ch->isOn){
		return;
	}

	uint16_t markerPosition_px = CANVA_MIDDLE_V - ch->y_offset;
	if((markerPosition_px - 2 > 32) && (markerPosition_px - 2 < 284)){
		for(int j = 0; j < 5; ++j)
			drawPixel(j, markerPosition_px - 2, ch->color);
	}

	if((markerPosition_px - 1 > 32) && (markerPosition_px - 1 < 284)){
		for(int j = 0; j < 6; ++j)
			drawPixel(j, markerPosition_px - 1, ch->color);
	}

	if((markerPosition_px > 32) && (markerPosition_px < 284)){
		for(int j = 0; j < 7; ++j)
			drawPixel(j, markerPosition_px, ch->color);
	}

	if((markerPosition_px + 1 > 32) && (markerPosition_px + 1 < 284)){
	for(int j = 0; j < 6; ++j)
		drawPixel(j, markerPosition_px + 1, ch->color);
	}

	if((markerPosition_px + 2 > 32) && (markerPosition_px + 2 < 284)){
	for(int j = 0; j < 5; ++j)
			drawPixel(j, markerPosition_px + 2, ch->color);
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

void drawMainMenuButton(){
	drawRectangleRoundedFrame(310, 288, 80, 28, GREY);
	LCD_Font(330, 307, "Menu", _Open_Sans_Bold_14, 1, WHITE);
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
	sprintf(buf,"H %dus", (int) osc->timeBase_us);

	LCD_Font(8, 19, buf, _Open_Sans_Bold_12  , 1, WHITE);
	if(osc->selection == SelectionTIME_BASE)
		drawRectangleRoundedFrame(3, 3, 70, 22, WHITE);
	else
		drawRectangleRoundedFrame(3, 3, 70, 22, GREY);
}

void displayHorizontallOffset(Oscilloscope* osc){
	uint8_t color = GREY;
	if(osc->selection == SelectionMOVE_WAVEFORMS_HORIZONTALLY){
		color = WHITE;
	}
	drawImageTransparentColored(358, 11, 15, 7, arrowLeftRight, WHITE);
	drawRectangleRoundedFrame(  340, 3, 50, 22, color);
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

		else if(osc->touchScreen.X > 399 && osc->touchScreen.Y > 290){
			if(osc->clickedItem != ClickedRUN_STOP){
				osc->stop = !osc->stop;
				osc->clickedItem = ClickedRUN_STOP;
			}
		}

		else if(osc->touchScreen.X < 70  && osc->touchScreen.Y < 25){			// Time per division
			osc->selection = SelectionTIME_BASE;
		}
		else if(osc->touchScreen.X > 340  && osc->touchScreen.X < 390  && osc->touchScreen.Y < 25){			// Time per division
			osc->selection = SelectionMOVE_WAVEFORMS_HORIZONTALLY;
		}

		else if(osc->touchScreen.X > 400  && osc->touchScreen.Y < 25){			// Trigger
			osc->selection = SelectionTRIGGER;
		}

		else if(osc->touchScreen.X > 310 && osc->touchScreen.X < 390 && osc->touchScreen.Y > 288){
			osc->selection = SelectionMAIN_MENU;
		}

		else if(osc->selection == SelectionCH1){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (68+40) && osc->touchScreen.Y > 68){	// CH1 scale vertically
				osc->ch1.changedParameter = VerticalScale;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (129+40) && osc->touchScreen.Y > 129){	// CH1 move vertically
				osc->ch1.changedParameter = VerticalOffset;
			}
		}

		else if(osc->selection == SelectionCH2){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (68+40) && osc->touchScreen.Y > 68){	// CH2 scale vertically
				osc->ch2.changedParameter = VerticalScale;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (129+40) && osc->touchScreen.Y > 129){	// CH2 move vertically
				osc->ch2.changedParameter = VerticalOffset;
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
		else if(osc ->selection == SelectionCURSORS || osc->selection == SelectionCURSORS_TIME || osc->selection == SelectionCURSORS_VOLTAGE){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (68+40) && osc->touchScreen.Y > 68){
				osc -> selection = SelectionCURSORS_CHANGE_CHANNEL;
			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (110+40) && osc->touchScreen.Y > 110){
				osc -> selection = SelectionCURSORS_TIME;

			}
			else if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (152+40) && osc->touchScreen.Y > 152){
				osc -> selection = SelectionCURSORS_VOLTAGE;
			}
		}
		else if(osc->selection == SelectionFFT){
			if(osc->touchScreen.X > 425 && osc->touchScreen.Y < (68+40) && osc->touchScreen.Y > 68){
					osc -> selection = SelectionFFT_CHANGE_CHANNEL;
			}
		}
		if((osc->selection != SelectionFFT) && (osc->selection != SelectionFFT_CHANGE_CHANNEL)){
			osc->is_fft_on = 0;
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
		drawMainMenu(WHITE);
		break;
	case SelectionCURSORS:
		drawCursorsMenu(osc);
		break;
	case SelectionCURSORS_CHANGE_CHANNEL:
		if( osc->active_cursor_channel == CursorChannel_1){
			osc->active_cursor_channel = CursorChannel_2;
		}else if(osc->active_cursor_channel == CursorChannel_2){
			osc->active_cursor_channel = CursorChannel_1;
		}
		//changeActiveCursorChannel(osc->active_cursor_channel);

		osc -> selection = SelectionCURSORS;
		drawCursorsMenu(osc);
		break;
	case SelectionCURSORS_TIME:

		if(osc->active_cursor_channel == CursorChannel_1 ){
			osc->ch1.cursors.cursor_type = CursorType_TIME;
		}else if(osc->active_cursor_channel == CursorChannel_2){
			osc->ch2.cursors.cursor_type = CursorType_TIME;
		}
		drawCursorsMenu(osc);
		break;
	case SelectionCURSORS_VOLTAGE:
		if(osc->active_cursor_channel == CursorChannel_1 ){
			osc->ch1.cursors.cursor_type = CursorType_VOLTAGE;
		}else if(osc->active_cursor_channel == CursorChannel_2){
			osc->ch2.cursors.cursor_type = CursorType_VOLTAGE;
		}
		drawCursorsMenu(osc);
		break;
	case SelectionFFT:
		osc->is_fft_on = 1;
		drawFFTMenu(osc);
		if(osc->fft_active_channel== FFT_Channel_1){
			drawFFTgraph(&osc->ch1);
		}else if(osc->fft_active_channel = FFT_Channel_2){
			drawFFTgraph(&osc->ch2);
		}
		break;
	case SelectionFFT_CHANGE_CHANNEL:
		if( osc->fft_active_channel== FFT_Channel_1){
			osc->fft_active_channel = FFT_Channel_2;
		}else if(osc->fft_active_channel == FFT_Channel_2){
			osc->fft_active_channel = FFT_Channel_1;
		}
		//changeActiveCursorChannel(osc->active_cursor_channel);

		osc -> selection = SelectionFFT;
		osc->is_fft_on = 1;
		drawFFTMenu(osc);
		break;
	case SelectionTRIGGER:
		drawTriggerMenu(osc);
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
    }
}

void change_cursors(Oscilloscope * osc){
    switch(osc -> changedCursor){
        case CH1_TimeCursor_1:
            osc->ch1.cursors.time_cursor_1 -=  htim1.Instance->CNT;
            if(osc->ch1.cursors.time_cursor_1  > SCOPE_X){osc->ch1.cursors.time_cursor_1 = SCOPE_X;}
            if(osc->ch1.cursors.time_cursor_1  < 0){osc->ch1.cursors.time_cursor_1  = 0;}
            break;

        case CH1_TimeCursor_2:
        	osc->ch1.cursors.time_cursor_2 -=  htim1.Instance->CNT;
        	if(osc->ch1.cursors.time_cursor_2  > SCOPE_X){osc->ch1.cursors.time_cursor_2 = SCOPE_X;}
        	if(osc->ch1.cursors.time_cursor_2  < 0){osc->ch1.cursors.time_cursor_2  = 0;}
        	break;

        case CH1_VoltageCursor_1:
            osc->ch1.cursors.voltage_cursor_1 +=  htim1.Instance->CNT;
			if(osc->ch1.cursors.voltage_cursor_1 > SCOPE_Y){osc->ch1.cursors.voltage_cursor_1 = SCOPE_Y;}
			if(osc->ch1.cursors.voltage_cursor_1 < 0){osc->ch1.cursors.voltage_cursor_1 = 0;}
            break;

        case CH1_VoltageCursor_2:
            osc->ch1.cursors.voltage_cursor_2 +=  htim1.Instance->CNT;
            if(osc->ch1.cursors.voltage_cursor_2 > SCOPE_Y){osc->ch1.cursors.voltage_cursor_2 = SCOPE_Y;}
            if(osc->ch1.cursors.voltage_cursor_2 < 0){osc->ch1.cursors.voltage_cursor_2 = 0;}
            break;

        case CH2_TimeCursor_1:
        	osc->ch2.cursors.time_cursor_1 -=  htim1.Instance->CNT;
        	if(osc->ch2.cursors.time_cursor_1  > SCOPE_X){osc->ch2.cursors.time_cursor_1 = SCOPE_X;}
        	if(osc->ch2.cursors.time_cursor_1  < 0){osc->ch2.cursors.time_cursor_1  = 0;}
        	break;

        case CH2_TimeCursor_2:
        	osc->ch2.cursors.time_cursor_2 -=  htim1.Instance->CNT;
        	if(osc->ch2.cursors.time_cursor_1  > SCOPE_X){osc->ch2.cursors.time_cursor_1 = SCOPE_X;}
        	if(osc->ch2.cursors.time_cursor_1  < 0){osc->ch2.cursors.time_cursor_1  = 0;}
        	break;

        case CH2_VoltageCursor_1:
        	osc->ch2.cursors.voltage_cursor_1 +=  htim1.Instance->CNT;
        	if(osc->ch2.cursors.voltage_cursor_1 > SCOPE_Y){osc->ch2.cursors.voltage_cursor_1 = SCOPE_Y;}
        	if(osc->ch2.cursors.voltage_cursor_1 < 0){osc->ch2.cursors.voltage_cursor_1 = 0;}
        	break;

        case CH2_VoltageCursor_2:
        	osc->ch2.cursors.voltage_cursor_2 +=  htim1.Instance->CNT;
        	if(osc->ch2.cursors.voltage_cursor_2 > SCOPE_Y){osc->ch2.cursors.voltage_cursor_2 = SCOPE_Y;}
        	if(osc->ch2.cursors.voltage_cursor_2 < 0){osc->ch2.cursors.voltage_cursor_2 = 0;}
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
	case SelectionCURSORS_TIME:
	case SelectionCURSORS_VOLTAGE:
		change_cursors(osc);
		if(HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == 0){
			if(osc->active_cursor_channel == CursorChannel_1){
				if(osc->ch1.cursors.num_cursor_flag == 1){
					osc->ch1.cursors.num_cursor_flag = 2;
				}else if(osc->ch1.cursors.num_cursor_flag == 2){
					osc->ch1.cursors.num_cursor_flag = 1;
				}
			}
			if(osc->active_cursor_channel == CursorChannel_2){
				if(osc->ch2.cursors.num_cursor_flag == 1){
					osc->ch2.cursors.num_cursor_flag = 2;
				}else if(osc->ch2.cursors.num_cursor_flag == 2){
					osc->ch2.cursors.num_cursor_flag = 1;
				}
			}
		}
		break;
	case SelectionTIME_BASE:
		if(osc->stop){
			break;
		}
		osc->timeBaseIndex += htim1.Instance->CNT;
		if(osc->timeBaseIndex < 0)
			osc->timeBaseIndex = 0;
		if(osc->timeBaseIndex > 8)
			osc->timeBaseIndex = 8;
		osc->timeBase_us = osc->timeBaseArray[osc->timeBaseIndex];
		break;
	case SelectionMOVE_WAVEFORMS_HORIZONTALLY:
		osc->x_offset -= htim1.Instance->CNT;
		if(osc->x_offset < 0){
			osc->x_offset = 0;
		}
		else if(osc->x_offset > MEMORY_DEPTH - CANVA_WIDTH){
			osc->x_offset = MEMORY_DEPTH - CANVA_WIDTH;
		}
		if(HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == 0){
			osc->x_offset = 0;
		}
		break;
	case SelectionTRIGGER:
		osc->triggerLevel_mV -= 10*htim1.Instance->CNT;
		if(osc->triggerLevel_mV < 0){
			osc->triggerLevel_mV = 0;
		}
		else if(osc->triggerLevel_mV >3300){
			osc->triggerLevel_mV = 3300;
		}
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
	switch(ch->changedParameter){
	case VerticalScale:
		color1 = WHITE;
		break;
	case VerticalOffset:
		color2 = WHITE;
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

}

void drawMainMenu(uint8_t color){
	drawRectangleRoundedFrame(423, 32, 56, 253, color);

	LCD_Font(433, 45, "Menu", _Open_Sans_Bold_12, 1, WHITE);

	LCD_Font(428, 74, "Cursor", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(425, 50, 52, 40, color);

	LCD_Font(438, 74+42, "FFT", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(425, 92, 52, 40, color);
}

void drawCursorsMenu(Oscilloscope * osc){

	char buf[20];
	drawRectangleRoundedFrame(423, 32, 56, 253, WHITE);

	LCD_Font(425, 45, "Cursors", _Open_Sans_Bold_12, 1, WHITE);

	drawFastHLine(428, 48, 46, WHITE);


	LCD_Font(429, 64, "Source", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(  425, 68, 52, 40, WHITE);

	drawRectangleRoundedFrame(425, 110, 52, 40, WHITE);
	drawRectangleRoundedFrame(425, 152, 52, 40, WHITE);

	if(osc->active_cursor_channel == CursorChannel_1){
		if(osc->ch1.cursors.cursor_type == CursorType_TIME){
			drawCursorsTime(&osc->ch1.cursors, osc->timeBase_us, WHITE);
		}else if(osc->ch1.cursors.cursor_type == CursorType_VOLTAGE){
			drawCursorsVoltage(&osc->ch1.cursors, osc->ch1.y_scale_mV, WHITE);
		}
	}else if(osc->active_cursor_channel == CursorChannel_2){
		if(osc->ch2.cursors.cursor_type == CursorType_TIME){
			drawCursorsTime(&osc->ch2.cursors, osc->timeBase_us, WHITE);
		}else if(osc->ch2.cursors.cursor_type == CursorType_VOLTAGE){
			drawCursorsVoltage(&osc->ch2.cursors, osc->ch2.y_scale_mV, WHITE);
		}
	}

	if(osc->active_cursor_channel == CursorChannel_1){
		if(osc->ch1.cursors.cursor_type == CursorType_TIME){
			if(osc->ch1.cursors.num_cursor_flag == 1){
				osc->changedCursor = CH1_TimeCursor_1;
			}else if(osc->ch1.cursors.num_cursor_flag == 2){
				osc->changedCursor = CH1_TimeCursor_2;
			}

		}else if(osc->ch1.cursors.cursor_type == CursorType_VOLTAGE){
			if(osc->ch1.cursors.num_cursor_flag == 1){
				osc->changedCursor = CH1_VoltageCursor_1;
			}else if(osc->ch1.cursors.num_cursor_flag == 2){
				osc->changedCursor = CH1_VoltageCursor_2;
			}
		}


		LCD_Font(437, 93, "Ch 1", _Open_Sans_Bold_12, 1, WHITE);
		if(osc->ch1.cursors.cursor_type == CursorType_DISABLE){
			drawImageTransparentColored(443, 127, 15, 7, arrowLeftRight, GREY);
			drawImageTransparentColored(447, 166, 8, 15, arrowUpDown, GREY);
			//LCD_Font(425, 235, "disable", _Open_Sans_Bold_12, 1, GREY);
		}else if(osc->ch1.cursors.cursor_type  == CursorType_TIME){
			drawImageTransparentColored(443, 127, 15, 7, arrowLeftRight, WHITE);
			drawImageTransparentColored(447, 166, 8, 15, arrowUpDown, GREY);

			fillRect(3+1, 35+1, 125-1, 35-1, BLACK);
			drawRectangleRoundedFrame(3, 35, 125, 35, GREY);

			LCD_Font(7, 50, "AX-BX = ", _Open_Sans_Bold_12, 1, WHITE);
			sprintf(buf,"%dus", osc->ch1.cursors.calculated_time);
			LCD_Font(60, 50, buf, _Open_Sans_Bold_12, 1, WHITE);

			LCD_Font(6, 50+15, "1/|dX|= ", _Open_Sans_Bold_12, 1, WHITE);
			sprintf(buf,"%dHz", 1000000/abs(osc->ch1.cursors.calculated_time));
			LCD_Font(61, 50+15, buf, _Open_Sans_Bold_12, 1, WHITE);
		}else if(osc->ch1.cursors.cursor_type  == CursorType_VOLTAGE){
			drawImageTransparentColored(443, 127, 15, 7, arrowLeftRight, GREY);
			drawImageTransparentColored(447, 166, 8, 15, arrowUpDown, WHITE);

			fillRect(3+1, 35+1, 125-1, 20-1, BLACK);
			drawRectangleRoundedFrame(3, 35, 125, 20, GREY);

			LCD_Font(7, 50, "AY-BY = ", _Open_Sans_Bold_12, 1, WHITE);
			sprintf(buf,"%dmV", osc->ch1.cursors.calculated_voltage);
			LCD_Font(60, 50, buf, _Open_Sans_Bold_12, 1, WHITE);
		}

	}else if(osc->active_cursor_channel == CursorChannel_2){
		if(osc->ch2.cursors.cursor_type == CursorType_TIME){
			if(osc->ch2.cursors.num_cursor_flag == 1){
				osc->changedCursor = CH2_TimeCursor_1;
			}else if(osc->ch2.cursors.num_cursor_flag == 2){
				osc->changedCursor = CH2_TimeCursor_2;
			}

		}else if(osc->ch2.cursors.cursor_type == CursorType_VOLTAGE){
			if(osc->ch2.cursors.num_cursor_flag == 1){
				osc->changedCursor = CH2_VoltageCursor_1;
			}else if(osc->ch2.cursors.num_cursor_flag == 2){
				osc->changedCursor = CH2_VoltageCursor_2;
			}
		}

		LCD_Font(437, 93, "Ch 2", _Open_Sans_Bold_12, 1, WHITE);
		if(osc->ch2.cursors.cursor_type == CursorType_DISABLE){
			drawImageTransparentColored(443, 127, 15, 7, arrowLeftRight, GREY);
			drawImageTransparentColored(447, 166, 8, 15, arrowUpDown, GREY);
		}else if(osc->ch2.cursors.cursor_type  == CursorType_TIME){
			drawImageTransparentColored(443, 127, 15, 7, arrowLeftRight, WHITE);
			drawImageTransparentColored(447, 166, 8, 15, arrowUpDown, GREY);

			fillRect(3+1, 35+1, 125-1, 35-1, BLACK);
			drawRectangleRoundedFrame(3, 35, 125, 35, GREY);

			LCD_Font(7, 50, "AX-BX = ", _Open_Sans_Bold_12, 1, WHITE);
			sprintf(buf,"%dus", osc->ch2.cursors.calculated_time);
			LCD_Font(60, 50, buf, _Open_Sans_Bold_12, 1, WHITE);

			LCD_Font(6, 50+15, "1/|dX|= ", _Open_Sans_Bold_12, 1, WHITE);
			sprintf(buf,"%dHz", 1000000/abs(osc->ch2.cursors.calculated_time));
			LCD_Font(61, 50+15, buf, _Open_Sans_Bold_12, 1, WHITE);
		}else if(osc->ch2.cursors.cursor_type  == CursorType_VOLTAGE){
			drawImageTransparentColored(443, 127, 15, 7, arrowLeftRight, GREY);
			drawImageTransparentColored(447, 166, 8, 15, arrowUpDown, WHITE);

			fillRect(3+1, 35+1, 125-1, 20-1, BLACK);
			drawRectangleRoundedFrame(3, 35, 125, 20, GREY);

			LCD_Font(7, 50, "AY-BY = ", _Open_Sans_Bold_12, 1, WHITE);
			sprintf(buf,"%dmV", osc->ch2.cursors.calculated_voltage);
			LCD_Font(60, 50, buf, _Open_Sans_Bold_12, 1, WHITE);
		}
	}



}

void changeActiveCursorChannel(enum ActiveCursorChannel active_cursor_channel){
	if(active_cursor_channel == CursorChannel_1){
		 active_cursor_channel = CursorChannel_2;
	}else if(active_cursor_channel == CursorChannel_2){
		 active_cursor_channel = CursorChannel_1;
	}
}

void drawFFTMenu(Oscilloscope* osc){
	drawRectangleRoundedFrame(423, 32, 56, 253, DARK_PINK);
	LCD_Font(440, 45, "FFT", _Open_Sans_Bold_12, 1, WHITE);

	drawFastHLine(428, 48, 46, DARK_PINK);

	LCD_Font(429, 64, "Source", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(  425, 68, 52, 40, DARK_PINK);

	if(osc->fft_active_channel == FFT_Channel_1){
		LCD_Font(437, 93, "Ch 1", _Open_Sans_Bold_12, 1, WHITE);
	}else if(osc->fft_active_channel == FFT_Channel_2){
		LCD_Font(437, 93, "Ch 2", _Open_Sans_Bold_12, 1, WHITE);
	}

}

void drawFFTgraph(Oscilloscope_channel * ch){
	uint32_t num_of_stripes;
	uint32_t stripe_width;
	uint32_t stripe_max_height;
	uint32_t max_amp = 0;
	uint32_t max_freq = 0;
	uint32_t amp_stripes_h[FFT_SIZE/2];
	uint32_t amp_stripes_y[FFT_SIZE/2];
	uint32_t stripes_counter = 0;




	num_of_stripes = FFT_SIZE/2;

	stripe_width = (2 * SCOPE_X)/FFT_SIZE;
	stripe_max_height = SCOPE_Y;

	for(int i = 0; i < FFT_SIZE/2;++i){
		if(ch->fft_amplitude[i] > max_amp){
			max_amp = ch->fft_amplitude[i];
		}
	}

	for(int i = 0; i < FFT_SIZE/2;++i){
		if(ch->fft_frequency[i] > max_freq){
			max_freq = ch->fft_frequency[i];
		}
	}

	for(int i = 0; i < FFT_SIZE/2; ++i){
		amp_stripes_h[i] = (ch->fft_amplitude[i]*stripe_max_height)/max_amp;
		amp_stripes_y[i] = SCOPE_Y_OFFSET + (stripe_max_height  - amp_stripes_h[i]);
	}



	for(int i = 0; i < num_of_stripes; ++i){

		for(int j = 0; j < stripe_width; ++j){
			drawFastVLine(stripes_counter, amp_stripes_y[i], amp_stripes_h[i], ch->color);
			stripes_counter++;
		}
	}

	ch->frequency_per_stripe = max_freq/num_of_stripes;

}



void calculateFFT(Oscilloscope_channel * ch, uint32_t sampling_frequency){

	char buff[20];
	uint32_t signal[FFT_SIZE];
	for(int i = 0; i < FFT_SIZE; ++i){
		signal[i] = ch->waveform_raw_adc[i];
	}

	Complex X[FFT_SIZE];
	for (int i = 0; i < FFT_SIZE; i++) {
		X[i].real = signal[i];
		X[i].imag = 0;
	}

	// Obliczanie FFT
	fft(X, FFT_SIZE);

	for (int i = 0; i < FFT_SIZE / 2; i++) {
		ch->fft_amplitude[i] = sqrt(X[i].real * X[i].real + X[i].imag * X[i].imag);
		ch->fft_frequency[i] = (i *sampling_frequency) / FFT_SIZE;
		printf("AMPLITUDA: %d\t:\t %d :freq\n\r", ch->fft_amplitude[i], ch->fft_frequency[i]);
	}

	uint32_t amp_max = 0;
	uint32_t freq = 0;

	for(int i = 0; i < FFT_SIZE / 2; i++){
		if(ch->fft_amplitude[i] > amp_max){
			amp_max = ch->fft_amplitude[i];
			freq = ch->fft_frequency[i];
		}
	}

	ch->channel_frequency = (uint32_t)freq;
}



void fft(Complex *X, int N) {
    if (N <= 1) return;


    Complex *X_even = (Complex *)malloc(N / 2 * sizeof(Complex));
    Complex *X_odd = (Complex *)malloc(N / 2 * sizeof(Complex));
    for (int i = 0; i < N / 2; i++) {
        X_even[i] = X[i * 2];
        X_odd[i] = X[i * 2 + 1];
    }


    fft(X_even, N / 2);
    fft(X_odd, N / 2);


    for (int k = 0; k < N / 2; k++) {
        double t = -2 * M_PI * k / N;
        Complex e = {cos(t), sin(t)};
        Complex temp = {e.real * X_odd[k].real - e.imag * X_odd[k].imag, e.real * X_odd[k].imag + e.imag * X_odd[k].real};
        X[k].real = X_even[k].real + temp.real;
        X[k].imag = X_even[k].imag + temp.imag;
        X[k + N / 2].real = X_even[k].real - temp.real;
        X[k + N / 2].imag = X_even[k].imag - temp.imag;
    }

    free(X_even);
    free(X_odd);
}


void drawTriggerMenu(Oscilloscope* osc){
	drawRectangleRoundedFrame(423, 32, 56, 253, ORANGE);
	LCD_Font(426, 45, "Trigger", _Open_Sans_Bold_12, 1, WHITE);

	drawFastHLine(428, 48, 46, ORANGE);

	LCD_Font(429, 64, "Source", _Open_Sans_Bold_12, 1, WHITE);
	drawRectangleRoundedFrame(  425, 68, 52, 40, ORANGE);
	LCD_Font(437, 93, "Ch 1", _Open_Sans_Bold_12, 1, WHITE);
}



void drawMeasurements(Oscilloscope* osc){
	char buf[20];
	if(osc->ch1.isOn){
		sprintf(buf,"Vpp=%dmV", calculate_peak_to_peak(osc->ch1.waveform_display));
		LCD_Font(90, 13, buf, _Open_Sans_Bold_12, 1, osc->ch1.color);
		sprintf(buf,"Vrms=%dmV", calculate_RMS(osc->ch1.waveform_display));
		LCD_Font(190, 13, buf, _Open_Sans_Bold_12, 1, osc->ch1.color);
		sprintf(buf,"freq = %d Hz", osc->ch1.channel_frequency);
		LCD_Font(290, 13, buf, _Open_Sans_Bold_12, 1, osc->ch1.color);
	}
	if(osc->ch2.isOn){
		sprintf(buf,"Vpp=%dmV", calculate_peak_to_peak(osc->ch2.waveform_display));
		LCD_Font(90, 27, buf, _Open_Sans_Bold_12, 1, osc->ch2.color);
		sprintf(buf,"Vrms=%dmV", calculate_RMS(osc->ch2.waveform_display));
		LCD_Font(190, 27, buf, _Open_Sans_Bold_12, 1, osc->ch2.color);
		sprintf(buf,"freq = %d Hz", osc->ch2.channel_frequency);
		LCD_Font(290, 13, buf, _Open_Sans_Bold_12, 1, osc->ch2.color);
	}
}

void drawRunStop(Oscilloscope* osc){
	drawRectangleRoundedFrame(399, 288, 80, 28, GREY);
	if(osc->stop)
		LCD_Font(420, 307, "STOP", _Open_Sans_Bold_14, 1, RED);
	else
		LCD_Font(423, 307, "RUN", _Open_Sans_Bold_14, 1, LIGHT_GREEN);

}

void drawTriggerIcon(Oscilloscope* osc){
	char buf[20];
	drawRectangleRoundedFrame(399, 3, 80, 22, GREY);
	drawImageTransparent(405, 7, 9, 15, trigRisingIcon);
	fillRect(417, 7, 15, 16, YELLOW);
	LCD_Font(420, 20, "1", _Open_Sans_Bold_14, 1, BLACK);
	sprintf(buf, "%.2fV", osc->triggerLevel_mV/1000.0);
	LCD_Font(433, 20, buf, _Open_Sans_Bold_14, 1, ORANGE);

	// draw marker
	int trigMarkerPos = CANVA_MIDDLE_V - osc->ch1.y_offset - ((osc->triggerLevel_mV*42)/osc->ch1.y_scale_mV);
	if((trigMarkerPos - 2 > 32) && (trigMarkerPos - 2 < 284)){
		for(int j = 420; j > 415; --j)
			drawPixel(j, trigMarkerPos - 2, ORANGE);
	}

	if((trigMarkerPos - 1 > 32) && (trigMarkerPos - 1 < 284)){
		for(int j = 420; j > 414; --j)
			drawPixel(j, trigMarkerPos - 1, ORANGE);
	}

	if((trigMarkerPos > 32) && (trigMarkerPos < 284)){
		for(int j = 420; j > 413; --j)
			drawPixel(j, trigMarkerPos, ORANGE);
	}

	if((trigMarkerPos + 1 > 32) && (trigMarkerPos + 1 < 284)){
		for(int j = 420; j > 414; --j)
			drawPixel(j, trigMarkerPos + 1, ORANGE);
	}

	if((trigMarkerPos + 2 > 32) && (trigMarkerPos + 2 < 284)){
		for(int j = 420; j > 415; --j)
			drawPixel(j, trigMarkerPos + 2, ORANGE);
	}
}

float convertAdcToVoltage(uint16_t adcValue){
	return adcValue*ADC_VREF/ADC_RESOLUTION;
}
