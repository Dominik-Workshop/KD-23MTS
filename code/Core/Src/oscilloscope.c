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


void draw_waveform(oscilloscope_channel* ch){
	ch->x_offset = -htim1.Instance->CNT;

	for(int i = 0; i < 440; ++i)
				ch->waveform_display[i] = ch->waveform[i];
	for(int i = 0; i < 440-1; ++i){
		//ch->waveform_display[i] = ch->waveform[i];
		int x0 = i;
		int x1 = i+1;
		int y0 = CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i]/40;
		int y1 = CANVA_MIDDLE_V - ch->x_offset - ch->waveform_display[i+1]/40;
		if(y0 < 26)
			y0 = 26;
		if(y0 > 290)
			y0 = 290;
		if(y1 < 26)
			y1 = 26;
		if(y1 > 290)
			y1 = 290;
		drawLine(x0, y0, x1, y1, GREEN);
	}
	// draw marker 0
	if((CANVA_MIDDLE_V - ch->x_offset - 2 > 26) && (CANVA_MIDDLE_V - ch->x_offset - 2 < 290)){
		for(int j = 0; j < 5; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 2, GREEN);
	}

	if((CANVA_MIDDLE_V - ch->x_offset - 1 > 26) && (CANVA_MIDDLE_V - ch->x_offset - 1 < 290)){
		for(int j = 0; j < 6; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset - 1, GREEN);
	}

	if((CANVA_MIDDLE_V - ch->x_offset > 26) && (CANVA_MIDDLE_V - ch->x_offset < 290)){
		for(int j = 0; j < 7; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset, GREEN);
	}

	if((CANVA_MIDDLE_V - ch->x_offset + 1 > 26) && (CANVA_MIDDLE_V - ch->x_offset + 1 < 290)){
	for(int j = 0; j < 6; ++j)
		drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 1, GREEN);
	}

	if((CANVA_MIDDLE_V - ch->x_offset + 2 > 26) && (CANVA_MIDDLE_V - ch->x_offset + 2 < 290)){
	for(int j = 0; j < 5; ++j)
			drawPixel(j, CANVA_MIDDLE_V - ch->x_offset + 2, GREEN);
	}
}
