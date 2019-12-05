

#ifndef __DRIVE_H
#define __DRIVE_H
#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"
#include "system.h"
#define MOSFET_BRD
//#define RELE_BRD

//#define DRIVE_DEBUG

typedef struct{
	int16_t pos;
	//int16_t start_pos;
	int16_t max_pos;
	int16_t dest_pos;
	int16_t steps;
	volatile _Bool direction;
	volatile _Bool run_f;	
	volatile _Bool fail_f;
	volatile _Bool photosensor_state;
	volatile _Bool min_sens_f;
	volatile _Bool max_sens_f;
	
}drive_t;

extern drive_t drv_m1;

#define DRV_POS 	drv_m1.pos//(TIM3->CNT)
#define DRV_DIR		drv_m1.direction//TIM3->CR1 & TIM_CR1_DIR



#define DRV_OPEN()			DRV_DIR=1;  HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_SET); drv_m1.run_f=1;SysCnt.drv_move=0;
#define DRV_CLOSE()			DRV_DIR=0;  HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_SET); drv_m1.run_f=1;	SysCnt.drv_move=0;
#define DRV_STOP()				HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); drv_m1.run_f = 0; 



_Bool Drive_CheckRotation(_Bool direction);
void Drive_HomeInit(void);
	
#endif
