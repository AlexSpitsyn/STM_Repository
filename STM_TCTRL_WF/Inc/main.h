/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define M_DRV1_Pin GPIO_PIN_0
#define M_DRV1_GPIO_Port GPIOA
#define M_DRV2_Pin GPIO_PIN_1
#define M_DRV2_GPIO_Port GPIOA
#define MAX_SENS_Pin GPIO_PIN_2
#define MAX_SENS_GPIO_Port GPIOA
#define MAX_SENS_EXTI_IRQn EXTI2_IRQn
#define MIN_SENS_Pin GPIO_PIN_3
#define MIN_SENS_GPIO_Port GPIOA
#define MIN_SENS_EXTI_IRQn EXTI3_IRQn
#define CS1_Pin GPIO_PIN_4
#define CS1_GPIO_Port GPIOA
#define RELEY_CTRL_Pin GPIO_PIN_0
#define RELEY_CTRL_GPIO_Port GPIOB
#define SW_INT_Pin GPIO_PIN_1
#define SW_INT_GPIO_Port GPIOB
#define SW_INT_EXTI_IRQn EXTI1_IRQn
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define CS2_Pin GPIO_PIN_12
#define CS2_GPIO_Port GPIOB
#define CE2_Pin GPIO_PIN_8
#define CE2_GPIO_Port GPIOA
#define GPIO2_Pin GPIO_PIN_15
#define GPIO2_GPIO_Port GPIOA
#define PHOTOSENSOR_Pin GPIO_PIN_4
#define PHOTOSENSOR_GPIO_Port GPIOB
#define CE1_Pin GPIO_PIN_6
#define CE1_GPIO_Port GPIOB
#define IRQ2_Pin GPIO_PIN_7
#define IRQ2_GPIO_Port GPIOB
#define IRQ2_EXTI_IRQn EXTI9_5_IRQn
#define IRQ1_Pin GPIO_PIN_8
#define IRQ1_GPIO_Port GPIOB
#define IRQ1_EXTI_IRQn EXTI9_5_IRQn
#define ds18b20_Pin GPIO_PIN_9
#define ds18b20_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
