/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h> 

#include "usbd_cdc_if.h"
#include "uart_cmd_parser.h"
#include "nrf24.h"
#include "usbd_cdc_if.h"
#include "wireless_ctrl.h"

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
/* Private variables ---------------------------------------------------------*/
uint8_t nrf_init_state=0;
uint32_t pump_on_timer=0;
uint32_t reset_timer=0;
uint16_t beep_on_timer=0;
_Bool pump_fail_f=0;
_Bool beep_f=0;
_Bool reset_f=0;

#define MAX_PUMP_ON_TIME			6000//60 sec
#define BEEP_ON_TIME					200//2 sec
#define PUMP_SHUTDOWN_TIME		30000//5 min
#define PUMP_RESET_TIME				90000//15 min

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_CRC_Init();
  MX_USB_DEVICE_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	printf("SYS INIT\r\n");
	
HAL_GPIO_WritePin(RELEY_CTRL_GPIO_Port,RELEY_CTRL_Pin,GPIO_PIN_SET);//RX
HAL_GPIO_WritePin(USB_PULLUP_GPIO_Port, USB_PULLUP_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
HAL_Delay(200);
uartInit();

nrf_init_state = NRF24_Init();
HAL_Delay(200);
//putsUSART("3\r\n>");
HAL_TIM_Base_Start_IT( &htim4);
//HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);
//putsUSART("4\r\n>");

if(nrf_init_state){
	putsUSART("NRF init FAIL\r\n>");
	printf("NRF init FAIL\r\n");
}

uint8_t cdc_init=0;
pump_on_timer=0;
if(nrf_init_state==0){
	printf("NRF CHECK ADDR\r\n");
	nrf_init_state=WL_Check_Addr(0xB0);	
	
}
HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

 while (1){
//Blinking 

//NRF24 init error	 -
//NRF24 addr busy		 - -
//NRF24 timeout			 - - -
	 
			

		if (vcp_rx_flag) {
			if(cdc_init){
			CDC_Transmit_FS((uint8_t*)"\r\n>", 3);
			//CDC_Transmit_FS((uint8_t*)"OK\n\r", 4);
			//putsUSART("\r\nOK\n\r");
			parseCommand(vcp_rx_buf);			
			}else{
				uartInit();
				cdc_init=1;					
			}
			vcp_rx_flag = 0;
		}
		
		
		
		if(nrf_init_state){
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
			HAL_Delay(1000);
			uint8_t i=nrf_init_state;
			while(i--){
				HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
				HAL_Delay(1000);
				HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
				HAL_Delay(1000);				
      }
			HAL_Delay(2000);	
			
			if(packet_handler_timer>500){// every 5 sec
				packet_handler_timer=0;
				if (nrf_init_state == 1){
					nrf_init_state = NRF24_Init();
					HAL_Delay(200);
				
				}
				if(nrf_init_state!=1){
						HAL_Delay(200);
						nrf_init_state = WL_Check_Addr(0xB0);
					}
				}
		}else{
			WL_Handler();	
		}
		
		

		//PUMP MONITOR
		if(pump_on_timer>MAX_PUMP_ON_TIME){
			pump_fail_f=1;							
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);			
		}else{
			pump_fail_f=0;			
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
		}
		
		//BEEP CTRL
		if(beep_on_timer>BEEP_ON_TIME){
			beep_f^=1;
			beep_on_timer=0;
			printf("BEEP\r\n");
		}
		if(beep_f){
			HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);
		}else{
			HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_2);
		}
		//END BEEP CTRL
		
		
		//PUMP SHUTDOWN
		if(pump_on_timer>PUMP_SHUTDOWN_TIME){
			printf("PUMP SHUTDOWN\r\n");
			HAL_GPIO_WritePin(RELEY_CTRL_GPIO_Port,RELEY_CTRL_Pin,GPIO_PIN_RESET);
			reset_f=1;
		}
		//PUMP RESET
		if(reset_timer>PUMP_RESET_TIME){
			printf("PUMP RESET\r\n");
			HAL_GPIO_WritePin(RELEY_CTRL_GPIO_Port,RELEY_CTRL_Pin,GPIO_PIN_SET);
			reset_timer=0;
			reset_f=0;
			beep_f=0;
		}
		


	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
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
