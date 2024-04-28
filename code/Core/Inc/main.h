/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define ENC_BTN_Pin GPIO_PIN_0
#define ENC_BTN_GPIO_Port GPIOC
#define CH1_Pin GPIO_PIN_1
#define CH1_GPIO_Port GPIOC
#define TS_MISO_Pin GPIO_PIN_2
#define TS_MISO_GPIO_Port GPIOC
#define TS_MOSI_Pin GPIO_PIN_3
#define TS_MOSI_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TFT_DC_Pin GPIO_PIN_4
#define TFT_DC_GPIO_Port GPIOC
#define GEN_OUT_Pin GPIO_PIN_0
#define GEN_OUT_GPIO_Port GPIOB
#define TS_SCK_Pin GPIO_PIN_10
#define TS_SCK_GPIO_Port GPIOB
#define TS_CS_Pin GPIO_PIN_12
#define TS_CS_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_8
#define TFT_CS_GPIO_Port GPIOC
#define TFT_LED_Pin GPIO_PIN_9
#define TFT_LED_GPIO_Port GPIOC
#define TFT_RST_Pin GPIO_PIN_10
#define TFT_RST_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
