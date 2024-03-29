/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC

#define M_DRV1_Pin GPIO_PIN_0
#define M_DRV1_GPIO_Port GPIOA
#define M_DRV2_Pin GPIO_PIN_1
#define M_DRV2_GPIO_Port GPIOA

#define CS1_Pin GPIO_PIN_4
#define CS1_GPIO_Port GPIOA
#define CS2_Pin GPIO_PIN_12
#define CS2_GPIO_Port GPIOB

#define E_SENS_Pin GPIO_PIN_0
#define E_SENS_GPIO_Port GPIOB
#define E_SENS_EXTI_IRQn EXTI0_IRQn

#define SW_INT_Pin GPIO_PIN_1
#define SW_INT_GPIO_Port GPIOB
#define SW_INT_EXTI_IRQn EXTI1_IRQn



#define CE1_Pin GPIO_PIN_6
#define CE1_GPIO_Port GPIOB

#define CE2_Pin GPIO_PIN_7
#define CE2_GPIO_Port GPIOB

#define IRQ2_EXTI_IRQn EXTI9_5_IRQn
#define IRQ2_Pin GPIO_PIN_8
#define IRQ2_GPIO_Port GPIOA

#define IRQ1_Pin GPIO_PIN_8
#define IRQ1_GPIO_Port GPIOB
#define IRQ1_EXTI_IRQn EXTI9_5_IRQn

#define ds18b20_Pin GPIO_PIN_9
#define ds18b20_GPIO_Port GPIOB

#define GPIO2_Pin GPIO_PIN_15
#define GPIO2_GPIO_Port GPIOA

#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB


/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
//#define LED_Pin GPIO_PIN_13
//#define LED_GPIO_Port GPIOC

//#define M_DRV1_Pin GPIO_PIN_0
//#define M_DRV1_GPIO_Port GPIOA
//#define M_DRV2_Pin GPIO_PIN_1
//#define M_DRV2_GPIO_Port GPIOA

//#define CS1_Pin GPIO_PIN_4
//#define CS1_GPIO_Port GPIOA
//#define CS2_Pin GPIO_PIN_12
//#define CS2_GPIO_Port GPIOB

//#define E_SENS_Pin GPIO_PIN_0
//#define E_SENS_GPIO_Port GPIOB
//#define E_SENS_EXTI_IRQn EXTI0_IRQn

//#define SW_INT_Pin GPIO_PIN_1
//#define SW_INT_GPIO_Port GPIOB
//#define SW_INT_EXTI_IRQn EXTI1_IRQn



//#define CE1_Pin GPIO_PIN_6
//#define CE1_GPIO_Port GPIOB

//#define CE2_Pin GPIO_PIN_7
//#define CE2_GPIO_Port GPIOB

//#define IRQ2_EXTI_IRQn EXTI9_5_IRQn
//#define IRQ2_Pin GPIO_PIN_8
//#define IRQ2_GPIO_Port GPIOA

//#define IRQ1_Pin GPIO_PIN_8
//#define IRQ1_GPIO_Port GPIOB
//#define IRQ1_EXTI_IRQn EXTI9_5_IRQn

//#define ds18b20_Pin GPIO_PIN_9
//#define ds18b20_GPIO_Port GPIOB

//#define GPIO2_Pin GPIO_PIN_15
//#define GPIO2_GPIO_Port GPIOA

//#define BOOT1_Pin GPIO_PIN_2
//#define BOOT1_GPIO_Port GPIOB

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
