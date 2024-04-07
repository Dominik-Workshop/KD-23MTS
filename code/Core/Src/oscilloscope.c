/*
 * oscilloscope.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#include "oscilloscope.h"

void oscilloscope_channel_init(oscilloscope_channel* ch){
	ch->x_offset = 0;
	ch->y_offset = 0;
	ch->y_scale = 0;
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

void erase_waveform(uint16_t waveform[MEMORY_DEPTH], uint32_t x){
	for(int i = 0; i < 480; ++i){
			drawPixel(i, CANVA_MIDDLE_V - x - waveform[i]/40, BLACK);
		if((i%2==0) || (i ==479)){
			if(((CANVA_MIDDLE_V - x - waveform[i]/40) == 20) ||
			   ((CANVA_MIDDLE_V - x - waveform[i]/40) == 80) ||
			   ((CANVA_MIDDLE_V - x - waveform[i]/40) == 140) ||
			   ((CANVA_MIDDLE_V - x - waveform[i]/40) == 200) ||
			   ((CANVA_MIDDLE_V - x - waveform[i]/40) == 260) ||
			   ((CANVA_MIDDLE_V - x - waveform[i]/40) == 319)){
				drawPixel(i, CANVA_MIDDLE_V - x - waveform[i]/40, ILI9488_DARKGREY);
			}
		}
		if((i%60==0) || (i ==479)){
			if(((LCD_HEIGHT/2 - x - waveform[i]/40) % 2) == 0){
				drawPixel(i, CANVA_MIDDLE_V - x - waveform[i]/40, ILI9488_DARKGREY);
			}
		}
	  }
}

void draw_waveform(oscilloscope_channel* ch){

/*
	// erase marker 0
	for(int j = 0; j < 5; ++j){
		drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 2, BLACK);
	}
	for(int j = 0; j < 6; ++j){
		drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 1, BLACK);
	}
	for(int j = 0; j < 7; ++j){
		drawPixel(j, CANVA_MIDDLE_V - ch->x_offset, BLACK);
	}
	for(int j = 0; j < 6; ++j){
		drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 1, BLACK);
	}
	for(int j = 0; j < 5; ++j){
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 2, BLACK);
	}
	erase_waveform(ch->waveform_display_previous, ch->x_offset);
*/
	ch->x_offset = -htim1.Instance->CNT;

	for(int i = 0; i < 480; ++i)
				ch->waveform_display[i] = ch->waveform[i];
		for(int i = 0; i < 480-1; ++i){
			//ch->waveform_display[i] = ch->waveform[i];
			int x0 = i;
			int x1 = i+1;
			int y0 = CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i]/40;
			int y1 = CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i+1]/40;
			drawLine(x0, y0, x1, y1, 10);
			//drawLine(i, CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i]/40, i+1,  CANVA_MIDDLE_V - ch->x_offset -ch->waveform_display[i+1]/40, GREEN);
			//drawPixel(i, CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i]/40, GREEN);
			ch->waveform_display_previous[i] = ch->waveform_display[i];
		}
	// draw marker 0
	for(int j = 0; j < 5; ++j){
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 2, 10);
		}
		for(int j = 0; j < 6; ++j){
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 1, 10);
		}
		for(int j = 0; j < 7; ++j){
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset, 10);
		}
		for(int j = 0; j < 6; ++j){
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 1, 10);
		}
		for(int j = 0; j < 5; ++j){
				drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 2, 10);
		}
}
