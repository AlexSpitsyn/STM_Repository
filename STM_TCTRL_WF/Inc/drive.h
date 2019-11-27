

#ifndef __DRIVE_H
#define __DRIVE_H
#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"
#include "system.h"
#define MOSFET_BRD
//#define RELE_BRD

//#define DRIVE_DEBUG
#define DRV_POS 	(TIM3->CNT)
#define DRV_DIR		TIM3->CR1 & TIM_CR1_DIR
typedef struct{
	int16_t start_pos;
	int16_t max_pos;
	int16_t dest_pos;
	int16_t steps;
	//int16_t direction;
	volatile _Bool run_f;	
	volatile _Bool fail_f;
	volatile _Bool min_sens_f;
	volatile _Bool max_sens_f;
	
}drive_t;


extern drive_t drv_m1;

#ifdef MOSFET_BRD
#define DRV_OPEN()			CLEAR_BIT(TIM3->CR1,TIM_CR1_DIR); drv_m1.start_pos=DRV_POS; HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_SET); drv_m1.run_f=1;
#define DRV_CLOSE()			SET_BIT(TIM3->CR1,TIM_CR1_DIR); drv_m1.start_pos=DRV_POS; HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_SET); drv_m1.run_f=1;	

//#define DRV_OPEN					GPIO_PIN_SET
//#define DRV_CLOSE					GPIO_PIN_RESET
//#define DRV_RUN(x)				HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,x); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,(!x)); drv_m1.run_f=1;

#define DRV_STOP()				HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); drv_m1.run_f = 0; SysCnt.drv_move=0;
#endif

#ifdef RELE_BRD
#define DRV_OPEN			1
#define DRV_CLOSE			0
#define DRV_RUN(x)				HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_SET); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,x);	
#define DRV_STOP()				HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET);
#endif

//_Bool Drive_CheckRotation(void);
void Drive_HomeInit(void);
	
#endif
