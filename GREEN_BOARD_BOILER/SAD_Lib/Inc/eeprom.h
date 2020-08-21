
#ifndef __eeprom_H
#define __eeprom_H

#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "sys_config.h"
#include <stdio.h>

//#define EEPROM_ADDR 0xA0
//#define EEPROM_I2C_PORT &hi2c2
//#define EEPROM_DBG_PRINT_F
//extern uint8_t EEPROM_buf[32];

uint8_t  EEPROM_Write(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData);
uint8_t  EEPROM_Read(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData);
uint8_t EEPROM_IsDeviceReady(void);



#endif

