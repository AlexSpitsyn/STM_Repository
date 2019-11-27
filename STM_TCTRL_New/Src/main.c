/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "crc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
//#include <stdout.h>
#include <stdio.h> 
#include "ds18b20.h"
#include "7seg.h"
#include "eeprom.h"
#include "drive.h"
#include "io_expander.h"
#include "system.h"
#include "uart_cmd_parser.h"
#include "UART_DBG.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	
	//ERRATA HAL I2C Debug 
  __HAL_RCC_I2C2_CLK_ENABLE();
   HAL_Delay(100);
   __HAL_RCC_I2C2_FORCE_RESET();
   HAL_Delay(100);
   __HAL_RCC_I2C2_RELEASE_RESET();
   HAL_Delay(100);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */


//Start timer1 IT
HAL_TIM_Base_Start_IT(&htim1);

//Start timer3 IT
HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
HAL_TIM_Base_Start_IT(&htim3);

//Arm UART1
HAL_UART_Receive_IT(&huart1, &receive_val, 1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

int16_t temp_pos	=0;
SysInit();
//putsUSART("=====================SysInit======================\r\n");
printf("=====================SysInit======================\r\n");

if(!NRF24_Init()){
	if(WL_Check_Addr(0xC0)){
		SysState.nrf24_en_f=0;
		putcSS(SEV_SEG_A);
		PrintDecSevSeg(0);
		
	}else{
		SysState.nrf24_en_f=1;
		putcSS(SEV_SEG_A);
		PrintDecSevSeg(NRF_RX_ADDR[0]);
	}
	SS_LATCH();
	uint8_t t=5;
	while(t--){

	HAL_Delay(300);
		SS_OE(1);
	HAL_Delay(300);
	SS_OE(0);	
	}

	
}



// if (ds18b20_Init(RESOLUTION_9BIT)) {
//    
//    putsUSART("TEMP SENSOR ERROR\r\n");
//  }
//while(1){
////ds18b20_GetTemp();
//	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//	HAL_Delay(2000);
//}

	while (1) {
	  SystemTask();
	  if (!SysState.error_f) {
	    if (!drv_m1.run_f) {
	      if (btn_pressed_f) {
	        if (SysState.view_mode_f) {
	          //=================POSITION MODE====================
	          printf("POS MODE\r\n");
						switch (Buttons_GetCode()) {

	          case BTN_SEL:
	            printf("SEL\r\n");
	            SysState.view_mode_f ^= 1;
	            SysState.ss_update_f = 1;
	            SysCnt.view_mode = 0;
	            break;

	          case LONG_PRESS(BTN_SEL):
	            printf("LONG SEL\r\n");
	            SetMaxPosition();
	            SS_SHOW(POSITION_VIEW);
	            SysCnt.temp_update = 0;
	            SysCnt.temp_ctrl = 0;
	            break;

	          case BTN_UP:
	            printf("UP\r\n");
	            if (drv_m1.dest_pos + temp_pos <= drv_m1.max_pos - 10) {
	              temp_pos += 10;
	            } else {
	              temp_pos = drv_m1.max_pos - drv_m1.dest_pos;
	            }
	            SS_PRINT_POS(drv_m1.dest_pos + temp_pos);
	            SysCnt.view_mode = 0;

	            break;

	          case LONG_PRESS(BTN_UP):
	            printf("LONG UP\r\n");
	            break;

	          case BTN_DOWN:
	            printf("DOWN\r\n");
	            if (drv_m1.dest_pos + temp_pos >= 10) {
	              temp_pos -= 10;
	            } else {
	              temp_pos = -drv_m1.dest_pos;
	            }
	            SS_PRINT_POS(drv_m1.dest_pos + temp_pos);
	            SysCnt.view_mode = 0;
	            break;

	          case LONG_PRESS(BTN_DOWN):
	            printf("LONG DOWN\r\n");
							drv_m1.fail_f=0;
	            Drive_HomeInit();
	            SysState.temp_ctrl_f = 0;
	            //drv_m1.dest_pos = 0;
	            //DRV_CLOSE();

	          default:
	            printf("CODE %d\r\n", Buttons_GetCode());
	            break;
	          } // END POSITION MODE
	        }else {
	          //==============TEMP MODE (Default)=========================
	          printf("TEMP MODE\r\n");
						switch (Buttons_GetCode()) {

	          case BTN_SEL:
	            printf("SEL\r\n");
	            SysState.view_mode_f ^= 1;
	            SysState.ss_update_f = 1;
	            SysCnt.view_mode = 0;

	            break;

	          case LONG_PRESS(BTN_SEL):
	            printf("LONG SEL\r\n");
	            SetTemp();
	            SS_SHOW(TEMP_VIEW);
	            SysCnt.temp_update = 0;
	            SysCnt.temp_ctrl = 0;
	            break;

	          case BTN_UP:
	            printf("UP\r\n");
	            SysCnt.temp_update = SysState.t_updt_time;
	            SysCnt.temp_ctrl = SysState.t_ctrl_time;
	            break;

	          case LONG_PRESS(BTN_UP):	          
	          SysState.temp_ctrl_f = 1;
	          SysVarRW(WR, &SV[vn_T_CTRL_F]);
	          SysCnt.temp_update = 0;
	          SysCnt.temp_ctrl = 0;
						ds18b20_GetTemp();
	          SS_SHOW(TEMP_VIEW);
						SysState.view_mode_f = TEMP_VIEW; 
						SysState.ss_update_f = 1; 
						SysCnt.view_mode = 0;
	            break;

	          case BTN_DOWN:
	            printf("DOWN\r\n");

	            break;

	          case LONG_PRESS(BTN_DOWN):
	            printf("LONG DOWN\r\n");
	            SysState.temp_ctrl_f = 0;
	            SysVarRW(WR, &SV[vn_T_CTRL_F]);
	            SS_PRINT_T_NONE();
	            SS_SHOW(TEMP_VIEW);
	            break;
						
						case LONG_PRESS(BTN_DOWN|BTN_UP):
	            printf("LONG DOWN & UP\r\n");
	            uartInit();
							SysState.uart_en_f=1;
	            break;

	          default:
	            printf("CODE %d\r\n", Buttons_GetCode());
	            break;
	          }
	        } //END TEMP MODE

	      } //END btn_pressed_f

	      // Show position when manual set pos
	      if (!SysState.view_mode_f && (temp_pos != 0)) {
	        drv_m1.dest_pos += temp_pos;
	        temp_pos = 0;
	        SS_SHOW(POSITION_VIEW);
	      }
				
	    } else {//DRV RUN if (!drv_m1.run_f)
	      if (btn_pressed_f) {
					printf("DRV RUN\r\n");
	        switch (Buttons_GetCode()) {

	        case BTN_SEL:
	          printf("SEL\r\n");
	          drv_m1.stop_req_f = 1;
	          break;

	        default:
	          printf("CODE %d\r\n", Buttons_GetCode());
	          break;

	        }
	      }
	    }			
	  }else{//if (!SysState.error_f)
			if(HAL_GPIO_ReadPin(BOOT1_GPIO_Port, BOOT1_Pin) && SysState.uart_en_f==0){
				uartInit();								
				SysState.uart_en_f=1;	
			}
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
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
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)

{

  if(GPIO_Pin== SW_INT_Pin) {    

  } 
	if (GPIO_Pin == IRQ2_Pin) {

		//NRF24_IRQ_Callback();
	}

}
//void TIM4_IRQHandler(void){
//  
// 

//  HAL_TIM_IRQHandler(&htim4);
//  
// 
//  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
// 
//  
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
