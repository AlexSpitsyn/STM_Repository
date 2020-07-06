#include "eeprom.h"

HAL_StatusTypeDef  EEPROM_Write(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData)
{
	HAL_StatusTypeDef res;
	res = HAL_I2C_Mem_Write (&hi2c2, (uint16_t) EEPROM_ADDR, MEM_ADDR, I2C_MEMADD_SIZE_8BIT, pData, Size, (uint32_t)1000);
		if(res != HAL_OK){
			printf("EEPROM WRITE ERROR CODE: %d\r\n",HAL_I2C_GetError(&hi2c2));
	
		}
		return res;
}


HAL_StatusTypeDef  EEPROM_Read(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData)
{
	HAL_StatusTypeDef res;
	res = HAL_I2C_Mem_Read (&hi2c2, (uint16_t) EEPROM_ADDR, MEM_ADDR, I2C_MEMADD_SIZE_8BIT, pData, Size, (uint32_t)1000);
		if(res != HAL_OK){
			printf("EEPROM READ ERROR CODE: %d\r\n",HAL_I2C_GetError(&hi2c2));
	
		}
		return res;
}

//#define HAL_I2C_ERROR_NONE       0x00000000U    /*!< No error           */
//#define HAL_I2C_ERROR_BERR       0x00000001U    /*!< BERR error         */
//#define HAL_I2C_ERROR_ARLO       0x00000002U    /*!< ARLO error         */
//#define HAL_I2C_ERROR_AF         0x00000004U    /*!< AF error           */
//#define HAL_I2C_ERROR_OVR        0x00000008U    /*!< OVR error          */
//#define HAL_I2C_ERROR_DMA        0x00000010U    /*!< DMA transfer error */
//#define HAL_I2C_ERROR_TIMEOUT    0x00000020U    /*!< Timeout Error      */
