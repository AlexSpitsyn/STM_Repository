#include "io_expander.h"

uint8_t PCF8574A_reg;


HAL_StatusTypeDef PCF8574_ReadReg(void)
{
	HAL_StatusTypeDef res;
	res = HAL_I2C_Master_Receive(&hi2c2, PCF8574A_ADDR, &PCF8574A_reg, 1, HAL_MAX_DELAY);
		if(res != HAL_OK){
			printf("I2C STATUS CODE: %d\r\n",res);
			printf("I2C ERROR CODE: %d\r\n",HAL_I2C_GetError(&hi2c2));	
		}
		return res;
}


HAL_StatusTypeDef PCF8574_Init(uint8_t chanels)
{
	HAL_StatusTypeDef res;
	res = HAL_I2C_Master_Transmit(&hi2c2, PCF8574A_ADDR, &chanels, 1, HAL_MAX_DELAY);
		if(res != HAL_OK){
			printf("I2C STATUS CODE: %d\r\n",res);
			printf("I2C ERROR CODE: %d\r\n",HAL_I2C_GetError(&hi2c2));
	
		}
		return res;
}
