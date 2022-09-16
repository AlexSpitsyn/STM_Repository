
#ifndef __io_expander_H
#define __io_expander_H

#include "stm32f1xx_hal.h"
#include "i2c.h"
#include <stdio.h>
#include "sys_config.h"

//#define PCF8574A_ADDR 0x40
//#define PCF8574A_I2C_PORT &hi2c2
//#define PCF8574A_DBG_PRINT_F 1

extern uint8_t PCF8574_reg;

uint32_t PCF8574_ReadReg(void);
uint32_t PCF8574_WriteReg(uint8_t chanels);
uint32_t PCF8574_Init(uint8_t chanels);
uint32_t PCF8574_IsDeviceReady(void);
uint32_t PCF8574_PortSet(uint8_t PortNum);
uint32_t PCF8574_PortReset(uint8_t PortNum);
uint32_t PCF8574_PortToggle(uint8_t PortNum);

#endif


