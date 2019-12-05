
#ifndef __7seg_H
#define __7seg_H

#include "spi.h"
#include "main.h"
#include "stm32f1xx_hal.h"

#define SS_SPI_PORT1
//#define BLUE_BOARD
#define GREEN_BOARD

//#define SS_OE_Pin GPIO_PIN_6
//#define SS_OE_GPIO_Port GPIOB
//#define SS_LE_Pin GPIO_PIN_8
//#define SS_LE_GPIO_Port GPIOB
#ifdef SS_SPI_PORT1

#define SS_LE_PIN	            CE1_Pin
#define SS_LE_PORT            CE1_GPIO_Port

#define SS_OE_PIN             IRQ1_Pin
#define SS_OE_PORT            IRQ1_GPIO_Port
#define hSPI_PORT 						&hspi1

#endif

#ifdef SS_SPI_PORT2
#define SS_LE_PIN             CE2_Pin//B6
#define SS_LE_PORT            CE2_GPIO_Port

#define SS_OE_PIN             IRQ2_Pin//B7
#define SS_OE_PORT            IRQ2_GPIO_Port
#define hSPI_PORT 						&hspi2
#endif


extern uint8_t SS_CH;

#define SS_LATCH()          HAL_GPIO_WritePin(SS_LE_PORT, SS_LE_PIN, GPIO_PIN_SET); HAL_Delay(1); HAL_GPIO_WritePin(SS_LE_PORT, SS_LE_PIN, GPIO_PIN_RESET); 
#define SS_OE(x)            HAL_GPIO_WritePin(SS_OE_PORT, SS_OE_PIN, x);
//#define SS_OD()             HAL_GPIO_WritePin(SS_OE_GPIO_Port, SS_OE_Pin, GPIO_PIN_SET);
#define SS_OE_Toggle()      HAL_GPIO_TogglePin(SS_OE_PORT, SS_OE_PIN);
#define putcSS(c)			 			SS_CH=c ; HAL_SPI_Transmit(hSPI_PORT,&SS_CH, 1, 1000); 

#ifdef BLUE_BOARD
//#define SEV_SEG_0          0xEE
//#define SEV_SEG_1          0x82
//#define SEV_SEG_2          0xDC
//#define SEV_SEG_3          0xD6
//#define SEV_SEG_4          0xB2
//#define SEV_SEG_5          0x76
//#define SEV_SEG_6          0x7E
//#define SEV_SEG_7          0xC2
//#define SEV_SEG_8          0xFE
//#define SEV_SEG_9          0xF6
//#define SEV_SEG_DOT        0x01
//#define SEV_SEG_t          0x3C
//#define SEV_SEG_P          0xF8
//#define SEV_SEG_NONE       0x10
//#define SEV_SEG_E          0x7C
//#define SEV_SEG_F          0x78
//#define SEV_SEG_A          0xFA
//#define SEV_SEG_L          0x2C
//#define SEV_SEG_I          0x28
//			 - a6 -
//			|				|
//			f5  		b7
//			|				|
//			 - g4 -
//			|				|
//			e3  		c1
//			|				|
//			 - d2 -		.dp0
#endif

#ifdef GREEN_BOARD
//GREEN SCREEN
#define SEV_SEG_0          0x7D
#define SEV_SEG_1          0x60
#define SEV_SEG_2          0x57
#define SEV_SEG_3          0x76
#define SEV_SEG_4          0x6A
#define SEV_SEG_5          0x3E
#define SEV_SEG_6          0x3F
#define SEV_SEG_7          0x70
#define SEV_SEG_8          0x7F
#define SEV_SEG_9          0x7E
#define SEV_SEG_DOT        0x80
#define SEV_SEG_t          0x0F
#define SEV_SEG_P          0x5B
#define SEV_SEG_NONE       0x02
#define SEV_SEG_E          0x1F
#define SEV_SEG_F          0x1B
#define SEV_SEG_A          0xFB
#define SEV_SEG_L          0x0D
#define SEV_SEG_I          0x60


//			 - 0x10-
//			|				|
//		0x08  		0x40
//			|				|
//			 - 0x02-
//			|				|
//		0x01  		0x20
//			|				|
//			 - 0x04-		.0x80
			 
#endif			 
void SevSegInit(void);
void PrintDecSevSeg(int16_t dec);
void trulala(void);

#endif
