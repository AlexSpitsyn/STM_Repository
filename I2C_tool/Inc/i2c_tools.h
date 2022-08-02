
#ifndef I2C_TOOLS_H
#define	I2C_TOOLS_H

#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c2;
#define I2C_PORT 				&hi2c2
extern HAL_I2C_StateTypeDef i2c_tool_state;		

HAL_StatusTypeDef I2C_set_addr(uint8_t dev_addr);
void I2C_set_memadd_size(uint32_t memadd_size);
uint8_t I2C_get_memadd_size(void);
uint8_t I2C_Scan(uint8_t* dev_addr);
//HAL_StatusTypeDef I2C_write_byte (uint8_t dbyte);
//HAL_StatusTypeDef I2C_read_byte (uint8_t* dbyte);
HAL_StatusTypeDef I2C_write_nbyte (uint8_t size, uint8_t *dbyte);
HAL_StatusTypeDef I2C_read_nbyte (uint8_t size, uint8_t* dbyte);
HAL_StatusTypeDef I2C_write_eeprom (uint16_t mem_addr, uint16_t size, uint8_t* pData);
HAL_StatusTypeDef I2C_read_eeprom(uint16_t mem_addr, uint16_t size, uint8_t* pData);

#endif	/* I2C_TOOLS_H */

