
#ifndef __io_expander_H
#define __io_expander_H

#include "stm32f1xx_hal.h"
#include "i2c.h"
#include <stdio.h>

#define PCF8574A_ADDR 0x40
extern uint8_t PCF8574A_reg;

HAL_StatusTypeDef PCF8574_ReadReg(void);
HAL_StatusTypeDef PCF8574_Init(uint8_t chanels);
#endif


