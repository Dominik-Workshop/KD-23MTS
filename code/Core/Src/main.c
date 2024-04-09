/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ts.h"
#include "stm32_adafruit_ts.h"
#include "ili9488.h"
#include "stdio.h"
#include "oscilloscope.h"
#include "008_Open_Sans_Bold.h"
#include "009_Open_Sans_Bold.h"
#include "010_Open_Sans_Bold.h"
#include "012_Open_Sans_Bold.h"
#include "014_Open_Sans_Bold.h"
#include "016_Open_Sans_Bold.h"
#include "018_Open_Sans_Bold.h"
#include "020_Open_Sans_Bold.h"
#include "022_Open_Sans_Bold.h"
#include "024_Open_Sans_Bold.h"
#include "026_Open_Sans_Bold.h"
#include "028_Open_Sans_Bold.h"
#include "036_Open_Sans_Bold.h"
#include "048_Open_Sans_Bold.h"
#include "072_Open_Sans_Bold.h"
#include "096_Open_Sans_Bold.h"
#include "112_Open_Sans_Bold.h"
#include "128_Open_Sans_Bold.h"

#define _Open_Sans_Bold_8      &Open_Sans_Bold_8
#define _Open_Sans_Bold_9      &Open_Sans_Bold_9
#define _Open_Sans_Bold_10     &Open_Sans_Bold_10
#define _Open_Sans_Bold_11     &Open_Sans_Bold_11
#define _Open_Sans_Bold_12      &Open_Sans_Bold_12
#define _Open_Sans_Bold_14      &Open_Sans_Bold_14
#define _Open_Sans_Bold_16      &Open_Sans_Bold_16
#define _Open_Sans_Bold_18      &Open_Sans_Bold_18
#define _Open_Sans_Bold_20      &Open_Sans_Bold_20
#define _Open_Sans_Bold_22      &Open_Sans_Bold_22
#define _Open_Sans_Bold_24      &Open_Sans_Bold_24
#define _Open_Sans_Bold_26      &Open_Sans_Bold_26
#define _Open_Sans_Bold_28      &Open_Sans_Bold_28
#define _Open_Sans_Bold_36      &Open_Sans_Bold_36
#define _Open_Sans_Bold_48      &Open_Sans_Bold_48
#define _Open_Sans_Bold_72      &Open_Sans_Bold_72
#define _Open_Sans_Bold_96      &Open_Sans_Bold_96
#define _Open_Sans_Bold_112      &Open_Sans_Bold_112
#define _Open_Sans_Bold_128      &Open_Sans_Bold_128

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t SPI1_TX_completed_flag = 1;


static int conv_done = 0;

uint8_t trigRisingIcon[15][9] = {
    {0     , 0     , 0     , 0     , WHITE , WHITE , WHITE , WHITE , WHITE},
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0     },
    {0     , 0     , WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0     },
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0     , 0     , 0     }
};


uint8_t arrowUpDown[15][8] = {
    {0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0},
    {WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0},
	{0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0},
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0}
};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI1_TX_completed_flag = 1;
}
extern  TS_DrvTypeDef         *ts_drv;
#define ts_calib()

void mainApp(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  oscilloscope_channel CH1;
  oscilloscope_channel_init(&CH1, YELLOW);

  oscilloscope_channel CH2;
  oscilloscope_channel_init(&CH2, BLUE);

  char buf[20];						// buffer for measurements to be displayed
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*) CH1.waveform , MEMORY_DEPTH);
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, LCD_BRIGHTNESS); // 0-1000
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 200); // 0-1000
  ILI9488_Init();
  setRotation(1);
  ILI9341_Fill_Screen(ILI9488_BLACK);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int faza = 0;

  TS_StateTypeDef ts;
  BSP_TS_Init(ILI9488_TFTHEIGHT,ILI9488_TFTWIDTH);
  ts_calib();

  for(int i = 0; i < 480; ++i){
  		CH2.waveform[i] = 0;
  }
  for(int i = 200; i < 350; ++i){
    		CH2.waveform[i] = 3000;
  }

  uint8_t color = YELLOW;
  uint8_t color2 = GREY;

  uint8_t changed_var = 0;
  uint16_t timeDiv = 100;
  uint16_t trigger = 140;
  uint64_t prevTime = HAL_GetTick();
  while (1){

	  clearScreen();
	  drawGrid();

	  drawImageTransparent(trigRisingIcon, 440, 5, 9, 15);
	  drawImageTransparent(arrowUpDown, 460, 60, 8, 15);

	  drawRectangleRoundedFrame(450, 50, 25, 40, RED, 2);
	  sprintf(buf,"%d", trigger);
	  LCD_Font(453, 15, buf, _Open_Sans_Bold_12, 1, WHITE);

	  BSP_TS_GetState(&ts);
	  	  if(ts.TouchDetected){
	  	  	  fillRect(ts.X, ts.Y, 5, 5, RED);
	  	  	  if(ts.X < 110 && ts.X > 0 && ts.Y < 320 && ts.Y > 290){	// CH1
	  	  		  if(HAL_GetTick() - prevTime > 1000){
	  	  			  prevTime = HAL_GetTick();
	  	  			  if(changed_var == 0){
						  if(color == GREY){
							color = YELLOW;
						  }
						  else
							color = GREY;
	  	  			  }
	  	  			  changed_var = 0;
	  	  		  }
	  	  	  }
	  	  	  if(ts.X < 220 && ts.X > 110 && ts.Y < 320 && ts.Y > 290){	//CH2
	  	  		if(HAL_GetTick() - prevTime > 1000){
	  	  			prevTime = HAL_GetTick();
	  	  			if(changed_var == 1){
					  if(color2 == GREY){
						color2 = BLUE;
					  }
					  else
						color2 = GREY;
	  	  			}
	  	  			changed_var = 1;
	  	  		}
			  }
	  	  	  if(ts.X < 70  && ts.Y < 25){			// Time per division
	  	  			changed_var = 2;
	  	  	  }
	  	  	if(ts.X > 440 && ts.Y < 25){			// trigger
					changed_var = 3;
			  }
	  	  }


	  switch(changed_var){
	  case 0:
		  CH1.x_offset += -htim1.Instance->CNT;
		  fillRect(460, 30, 10, 10, CH1.color);
		  break;
	  case 1:
		  CH2.x_offset += -htim1.Instance->CNT;
		  fillRect(460, 30, 10, 10, CH2.color);
		  break;
	  case 2:
		  timeDiv += -htim1.Instance->CNT;
		  if((HAL_GetTick()%1000) > 500)
			  fillRect(0, 0, 70, 23, GREY);
		  break;
	  case 3:
		  trigger += -htim1.Instance->CNT;
		  drawFastHLine(0, 291 - trigger, 440, RED);
		  break;
	  }
	  htim1.Instance->CNT=0;

	  for(int i = 0; i < 480; ++i){
		CH1.waveform[i] = 2000*sinf(0.05f*i + faza*0.1f) + 2000;
	  }
	  faza++;

	  if(HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == 0){
		  switch(changed_var){
		  	  case 0:
		  		  CH1.x_offset = 0;
		  		  break;
		  	  case 1:
		  		  CH2.x_offset = 0;
		  		  break;
		  }
	  }

	  if(color == YELLOW)
		  draw_waveform(& CH1);

	  if(color2 == BLUE)
		  draw_waveform(& CH2);

	  sprintf(buf,"H %dms", timeDiv);
	  LCD_Font(5, 15, buf, _Open_Sans_Bold_12  , 1, WHITE);

	  sprintf(buf,"Vpp=%d", calculate_peak_to_peak(CH1.waveform));
	  LCD_Font(250+5, 312, buf, _Open_Sans_Bold_12  , 1, YELLOW);
	  sprintf(buf,"Vrms=%d", calculate_RMS(CH1.waveform));
	  LCD_Font(250+80, 312, buf, _Open_Sans_Bold_12  , 1, YELLOW);

	  drawChanellVperDev(0, color);
	  LCD_Font(16, 312, "1", _Open_Sans_Bold_14, 1, BLACK);
	  LCD_Font(48, 312, "1.00V", _Open_Sans_Bold_14, 1, color);

	  drawChanellVperDev(110, color2);
	  LCD_Font(16+110, 312, "2", _Open_Sans_Bold_14, 1, BLACK);
	  LCD_Font(48+110, 312, "1.00V", _Open_Sans_Bold_14, 1, color2);

	  imageRender();

/*
	  //ILI9341_Fill_Screen(ILI9488_BLACK);
	  //drawImage(image, 10, 10, 200, 1);
	  if(conv_done){
		  conv_done = 0;
		  draw_waveform(& CH1);
		  //HAL_Delay(500);
		  sprintf(buf1,"Vpp=%d", calculate_peak_to_peak(CH1.waveform_display));
		  fillRect(39, 1, 35, 18, RED);
		  LCD_Font(5, 15, buf1, _Open_Sans_Bold_12  , 1, WHITE);
		  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) CH1.waveform, MEMORY_DEPTH);
		  //HAL_Delay(1);
	  }
	  */

	  //    CODE FOR TOUCH
	  /*
	  //char buf[20];
	  //LCD_Font(25, 25, buf1, _Open_Sans_Bold_24  , 1, WHITE);
	  touchX = getX();
	  touchY = getY();
	  //sprintf(buf,"x=%3d y=%3d",touchX,touchY);
	  //sprintf(buf1,"x=%3d y=%3d",touchX,touchY);
	  //LCD_Font(25, 25, buf, _Open_Sans_Bold_24  , 1, BLACK);
	  fillRect(touchX, touchY,7,7, RED);
	  */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	conv_done = 1;
	HAL_ADC_Stop_DMA(hadc);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
