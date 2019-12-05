
#ifndef __eeprom_H
#define __eeprom_H

#include "stm32f1xx_hal.h"
#include "i2c.h"


#define EEPROM_ADDR 0xA0
extern uint8_t EEPROM_buf[32];

HAL_StatusTypeDef  EEPROM_Write(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData);
HAL_StatusTypeDef  EEPROM_Read(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData);

typedef struct {
  int16_t val;
  int16_t addr;
  int16_t error_check;
  int16_t temp_ctrl;
  int16_t blink;
  int16_t view_mode;
}eeprom_vars_t;

#endif

