/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define RED_Pin GPIO_PIN_3
#define RED_GPIO_Port GPIOA
#define AMBER_Pin GPIO_PIN_4
#define AMBER_GPIO_Port GPIOA
#define GREEN_Pin GPIO_PIN_5
#define GREEN_GPIO_Port GPIOA
#define RED1_Pin GPIO_PIN_6
#define RED1_GPIO_Port GPIOA
#define AMBER1_Pin GPIO_PIN_7
#define AMBER1_GPIO_Port GPIOA
#define BTN_DEC_Pin GPIO_PIN_12
#define BTN_DEC_GPIO_Port GPIOB
#define BTN_INC_Pin GPIO_PIN_13
#define BTN_INC_GPIO_Port GPIOB
#define BTN_SELECT_Pin GPIO_PIN_14
#define BTN_SELECT_GPIO_Port GPIOB
#define BTN_MODE_Pin GPIO_PIN_15
#define BTN_MODE_GPIO_Port GPIOB
#define GREEN1_Pin GPIO_PIN_8
#define GREEN1_GPIO_Port GPIOA
#define LED_DEBUG_Pin GPIO_PIN_9
#define LED_DEBUG_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
