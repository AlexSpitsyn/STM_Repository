/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define DQ_Pin GPIO_PIN_0
#define DQ_GPIO_Port GPIOA
#define USB_PULLUP_Pin GPIO_PIN_1
#define USB_PULLUP_GPIO_Port GPIOA
#define USB_INT_Pin GPIO_PIN_2
#define USB_INT_GPIO_Port GPIOA
#define XP9_5o_Pin GPIO_PIN_3
#define XP9_5o_GPIO_Port GPIOA
#define SPI1_NSS1_Pin GPIO_PIN_4
#define SPI1_NSS1_GPIO_Port GPIOA
#define LORA_IRQ_Pin GPIO_PIN_0
#define LORA_IRQ_GPIO_Port GPIOB
#define LORA_IRQ_EXTI_IRQn EXTI0_IRQn
#define MCP_IRQ_Pin GPIO_PIN_1
#define MCP_IRQ_GPIO_Port GPIOB
#define XP9_6o_Pin GPIO_PIN_2
#define XP9_6o_GPIO_Port GPIOB
#define LORA_RESET_Pin GPIO_PIN_10
#define LORA_RESET_GPIO_Port GPIOB
#define SW_INT_Pin GPIO_PIN_11
#define SW_INT_GPIO_Port GPIOB
#define SW_INT_EXTI_IRQn EXTI15_10_IRQn
#define SS_OE_Pin GPIO_PIN_12
#define SS_OE_GPIO_Port GPIOB
#define SS_LE_Pin GPIO_PIN_8
#define SS_LE_GPIO_Port GPIOA
#define PHOTO_SENS_Pin GPIO_PIN_4
#define PHOTO_SENS_GPIO_Port GPIOB
#define PHOTO_SENS_EXTI_IRQn EXTI4_IRQn
#define MOTO_DRV1_Pin GPIO_PIN_8
#define MOTO_DRV1_GPIO_Port GPIOB
#define MOTO_DRV2_Pin GPIO_PIN_9
#define MOTO_DRV2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
