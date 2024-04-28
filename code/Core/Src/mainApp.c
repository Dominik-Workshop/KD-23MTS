#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/* BSP LCD driver */
#include "ILI9488.h"

/* BSP TS driver */
#include "stm32_adafruit_ts.h"

#include "lcd.h"


//=============================================================================
/* Setting section (please set the necessary things in this section) */

/* Touchscreen calibrate at starting
   - 0: off (for the touchscreen, use the TS_CINDEX values in stm32_adafruit_ts.h)
   - 1: on  (the touchscreen must be calibrated at startup)
   - 2: on and printf (the touchscreen must be calibrated at startup and printf the cindex values)
   - 3: on and displays the TS_CINDEX values on the screen */
#define TS_CALBIBRATE         0

/* If TS_CALBIBRATE == 3 -> Text line size */
#define TS_CALIBTEXTSIZE      12

//=============================================================================
#ifdef  osCMSIS
#define Delay(t)              osDelay(t)
#define GetTime()             osKernelSysTick()
#else
#define Delay(t)              HAL_Delay(t)
#define GetTime()             HAL_GetTick()
#endif

#if TS_CALBIBRATE == 0
#define ts_calib()
#elif TS_CALBIBRATE > 0

#include "ts.h"

#define CALIBDELAY            500
#define CALIBBOXSIZE          6
#define CALIBBOXPOS           15
#define TOUCHDELAY            50

extern  TS_DrvTypeDef         *ts_drv;

//-----------------------------------------------------------------------------
void touchcalib_drawBox(int32_t x, int32_t y, uint16_t cl)
{
  BSP_LCD_SetTextColor(cl);
  BSP_LCD_DrawRect(x - CALIBBOXSIZE / 2, y - CALIBBOXSIZE / 2, CALIBBOXSIZE, CALIBBOXSIZE);
}

//-----------------------------------------------------------------------------
/* Touchscreen calibration function */
void ts_calib(void)
{
  uint16_t tx, ty;
  ts_three_points tc, dc; /* touchscreen and display corrdinates */
  #if TS_CALBIBRATE == 2
  ts_cindex ci;
  #elif TS_CALBIBRATE == 3
  ts_cindex ci;
  static char s[16];
  #endif

  dc.x0 = 20;
  dc.y0 = 20;
  dc.x1 = BSP_LCD_GetXSize() >> 1;
  dc.x2 = BSP_LCD_GetXSize() - 1 - 20;
  dc.y1 = BSP_LCD_GetYSize() - 1 - 20;
  dc.y2 = BSP_LCD_GetYSize() >> 1;

  //touchcalib_drawBox(dc.x0, dc.y0, LCD_COLOR_YELLOW);
  Delay(CALIBDELAY);
  while(!ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);
  ts_drv->GetXY(0, &tx, &ty);
  tc.x0 = tx; tc.y0 = ty;

  while(ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);

  //touchcalib_drawBox(dc.x0, dc.y0, LCD_COLOR_GRAY);
  //touchcalib_drawBox(dc.x1, dc.y1, LCD_COLOR_YELLOW);
  Delay(CALIBDELAY);
  while(!ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);
  ts_drv->GetXY(0, &tx, &ty);
  tc.x1 = tx; tc.y1 = ty;
  while(ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);

  //touchcalib_drawBox(dc.x1, dc.y1, LCD_COLOR_GRAY);
  //touchcalib_drawBox(dc.x2, dc.y2, LCD_COLOR_YELLOW);
  Delay(CALIBDELAY);
  while(!ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);
  ts_drv->GetXY(0, &tx, &ty);
  tc.x2 = tx; tc.y2 = ty;
  while(ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);

  #if TS_CALBIBRATE == 1
  BSP_TS_CalibCalc(&tc, &dc, NULL);
  #elif TS_CALBIBRATE == 2
  BSP_TS_CalibCalc(&tc, &dc, &ci);
  BSP_TS_SetCindex(&ci);
  printf("\r\n#define  TS_CINDEX            {%d, %d, %d, %d, %d, %d, %d}\r\n", (int)ci[0], (int)ci[1], (int)ci[2], (int)ci[3], (int)ci[4], (int)ci[5], (int)ci[6]);
  #elif TS_CALBIBRATE == 3
  BSP_TS_CalibCalc(&tc, &dc, &ci);
  BSP_TS_SetCindex(&ci);
 // BSP_LCD_DisplayStringAt(10, 0, (uint8_t *)"#define TS_CINDEX", LEFT_MODE);
  for(uint32_t i=0; i<7; i++)
  {
    sprintf(s, "%d", (int)ci[i]);
   // BSP_LCD_DisplayStringAt(10, (i+1)*TS_CALIBTEXTSIZE, (uint8_t *)s, LEFT_MODE);
  }
  Delay(CALIBDELAY);
  while(!ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);
  while(ts_drv->DetectTouch(0))
    Delay(TOUCHDELAY);
  #endif
  Delay(CALIBDELAY);
  //BSP_LCD_Clear(LCD_COLOR_BLACK);
}

#endif



void mainApp(void){

  TS_StateTypeDef ts;


  BSP_TS_Init(480, 320);

  //ts_calib();



  while(1){
	  //BSP_LCD_DrawBitmap(10, 200, (uint8_t *)&rombitmap);
	  //BSP_LCD_Clear(LCD_COLOR_BLACK);
	  //drawGrid();
	  //for(int i = 10; i<400; ++i){
	  	  //BSP_LCD_DrawPixel(i, 10, LCD_COLOR_RED);
	  	  //BSP_LCD_DrawPixel(i, 12, LCD_COLOR_RED);
	  //}

	  BSP_TS_GetState(&ts);
	  if(ts.TouchDetected)
		  fillRect(ts.X, ts.Y, 5, 5, RED);
	  imageRender();
  }

  /*
  while(1)
  {
    BSP_TS_GetState(&ts);
    if(ts.TouchDetected)
    {
      if(ts.Y < boxsize)
      {
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        if (ts.X < boxsize)
        {
          currentcolor = LCD_COLOR_RED;
          BSP_LCD_DrawRect(0, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 2)
        {
          currentcolor = LCD_COLOR_YELLOW;
          BSP_LCD_DrawRect(boxsize, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 3)
        {
          currentcolor = LCD_COLOR_GREEN;
          BSP_LCD_DrawRect(boxsize*2, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 4)
        {
          currentcolor = LCD_COLOR_CYAN;
          BSP_LCD_DrawRect(boxsize*3, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 5)
        {
          currentcolor = LCD_COLOR_BLUE;
          BSP_LCD_DrawRect(boxsize*4, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 6)
        {
          currentcolor = LCD_COLOR_MAGENTA;
          BSP_LCD_DrawRect(boxsize*5, 0, boxsize, boxsize);
        }

        if (oldcolor != currentcolor)
        {
          BSP_LCD_SetTextColor(oldcolor);
          if (oldcolor == LCD_COLOR_RED)
            BSP_LCD_FillRect(0, 0, boxsize, boxsize);
          else if (oldcolor == LCD_COLOR_YELLOW)
            BSP_LCD_FillRect(boxsize, 0, boxsize, boxsize);
          else if (oldcolor == LCD_COLOR_GREEN)
            BSP_LCD_FillRect(boxsize * 2, 0, boxsize, boxsize);
          else if (oldcolor == LCD_COLOR_CYAN)
            BSP_LCD_FillRect(boxsize * 3, 0, boxsize, boxsize);
          else if (oldcolor == LCD_COLOR_BLUE)
            BSP_LCD_FillRect(boxsize * 4, 0, boxsize, boxsize);
          else if (oldcolor == LCD_COLOR_MAGENTA)
            BSP_LCD_FillRect(boxsize * 5, 0, boxsize, boxsize);
        }

        oldcolor = currentcolor;
      }
      else
      {
        BSP_LCD_DrawPixel(ts.X, ts.Y, currentcolor);
      }
    }
    Delay(1);
  }*/
}
