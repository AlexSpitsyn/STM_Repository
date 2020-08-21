
#ifndef __7seg_H
#define __7seg_H

#include "spi.h"
#include "sys_config.h"
#include "stm32f1xx_hal.h"

// TO DO
//#define SS_LE_PIN	            
//#define SS_LE_PORT            
//#define SS_OE_PIN             
//#define SS_OE_PORT            
//#define SS_SPI_PORT 					



#define SS_LATCH()          HAL_GPIO_WritePin(SS_LE_PORT, SS_LE_PIN, GPIO_PIN_SET); HAL_Delay(1); HAL_GPIO_WritePin(SS_LE_PORT, SS_LE_PIN, GPIO_PIN_RESET); 
#define SS_OE(x)            HAL_GPIO_WritePin(SS_OE_PORT, SS_OE_PIN, x);
#define SS_OE_Toggle()      HAL_GPIO_TogglePin(SS_OE_PORT, SS_OE_PIN);

#define SEV_SEG_0          0xEE
#define SEV_SEG_1          0x82
#define SEV_SEG_2          0xDC
#define SEV_SEG_3          0xD6
#define SEV_SEG_4          0xB2
#define SEV_SEG_5          0x76
#define SEV_SEG_6          0x7E
#define SEV_SEG_7          0xC2
#define SEV_SEG_8          0xFE
#define SEV_SEG_9          0xF6
#define SEV_SEG_DOT        0x01
#define SEV_SEG_t          0x3C
#define SEV_SEG_P          0xF8
#define SEV_SEG_NONE       0x10
#define SEV_SEG_E          0x7C
#define SEV_SEG_F          0x78
#define SEV_SEG_A          0xFA
#define SEV_SEG_L          0x2C
#define SEV_SEG_I          0x28

//			 - a6 -
//			|				|
//			f5  		b7
//			|				|
//			 - g4 -
//			|				|
//			e3  		c1
//			|				|
//			 - d2 -		.dp0


		 
void SevSeg_Init(void);
void SevSeg_PrintDec(int16_t dec);
void trulala(void);
void putcSS(uint8_t c);	
void SevSeg_Idle(void);
#endif
