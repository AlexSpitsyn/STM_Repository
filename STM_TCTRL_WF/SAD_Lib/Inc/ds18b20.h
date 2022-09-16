/*
 * ds18b20.h
 *
 *  Created on: 30 но€б. 2018 г.
 *      Author: a.spitsyn
 */

#ifndef DS18B20_H_
#define DS18B20_H_

//--------------------------------------------------
#include "stm32f1xx_hal.h"
#include "1-wire.h"
#include <stdio.h>
#include <string.h>
#include "sys_config.h"


//--------------------------------------------------

#define MAX_DEV_CNT	8

//extern uint8_t DS18B20_DBG_PRINT_F;

extern uint8_t SCRATCHPAD[9];
extern int8_t DS18B20_TEMP;

//--------------------------------------------------
#define SKIP_ROM 0
#define NO_SKIP_ROM 1
//--------------------------------------------------
#define RESOLUTION_9BIT 0x1F		//0.5
#define RESOLUTION_10BIT 0x3F		//0.25
#define RESOLUTION_11BIT 0x5F		//0.125
#define RESOLUTION_12BIT 0x7F		//0.0625
//--------------------------------------------------
uint8_t ds18b20_Reset(void) ;
uint8_t ds18b20_Init(uint8_t device_cnt, uint8_t resolution);
uint8_t ds18b20_GetTemp(uint8_t DevNum) ;
//void ds18b20_MeasureTemperCmd(void);
//void ds18b20_ReadStratcpad(void);
//void ds18b20_Convert_int8_t(void);
//--------------------------------------------------

#endif /* DS18B20_H_ */

