 
#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "usart.h"


#include "usbd_cdc_if.h"
#include "tim.h"


//#include "io_expander.h"
#include "eeprom.h"
//#include "7seg.h"
#include "ds18b20.h"
#include "uart_cmd_parser.h"
#include "hw_crc32.h"
#include "wireless_ctrl.h"
#include "1-wire.h"
#include "SX1278.h"
//#include "buttons_io_exp.h"



#define DEV_NAME 								"Wireless Temp Sensor v.2\r\n> "
#define WTS_NUM 								9
#define EEPROM_DUMP_SIZE 								72
static uint8_t eeprom_dump[EEPROM_DUMP_SIZE] ={
	0x00, 0x55, 0xD0, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	WTS_NUM, 0x57, 0x54, 0x53, 0x00, 0xFF, 0xFF, 0xFF
};


//EEPROM 
#define EEPROM_WL_ADDR 								0x40




//SEVEN SEGMENT CONFIG

//#define SS_SPI_PORT 									&hspi2
//#define SS_LE_PIN	            				GPIO_PIN_8
//#define SS_LE_PORT           					GPIOA
//#define SS_OE_PIN             				GPIO_PIN_12
//#define SS_OE_PORT            				GPIOB



//SX1278 CONFIG
#define SX1278_SPI_PORT 							&hspi1
#define SX1278_NSS_PORT								GPIOA
#define SX1278_NSS_PIN								GPIO_PIN_4
#define SX1278_RESET_PORT							GPIOA
#define SX1278_RESET_PIN							GPIO_PIN_8
#define SX1278_DIO0_PORT							GPIOB
#define SX1278_DIO0_PIN								GPIO_PIN_0



//// PCF8574A CONFIG
//#define PCF8574A_ADDR 								0x40
//#define PCF8574A_I2C_PORT 						&hi2c1



// EEPROM CONFIG
#define EEPROM_ADDR 									0xA0
#define EEPROM_I2C_PORT 							&hi2c2


// 1-WIRE CONFIG
#define OW_PORT 											GPIOB
#define OW_PIN												GPIO_PIN_1

// LEDS

//#define LED_RED												3 //PCF8574 port
//#define LED_BLUE											4 //PCF8574 port

// UART CONFIG
//#define print_to(x)										printf(x)
//#define print_to(x)									HAL_UART_Transmit(&huart1, (uint8_t*)x, strlen(x), 0xFFFF);
#define print_to(x)										while(CDC_Transmit_FS( (uint8_t*)x, strlen(x))!=USBD_OK)

//	DEBUG
//#define dbg_print(s) 									printf(s)
#define dbg_print(s) 									print_to(s)

extern char dbg_str[64];

#define EEPROM_DBG_PRINT_F 						0
//#define PCF8574A_DBG_PRINT_F 					1
#define DS18B20_DBG_PRINT_F 					0
//#define BUTTON_DBG_PRINT_F						0
#define SYS_DBG_PRINT_F								0

#endif //__SYS_CONFIG_H__
