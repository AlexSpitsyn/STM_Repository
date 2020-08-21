#include "eeprom.h"



uint8_t  EEPROM_Write(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData)
{
	HAL_StatusTypeDef res;
	res = HAL_I2C_Mem_Write (EEPROM_I2C_PORT, (uint16_t) EEPROM_ADDR, MEM_ADDR, I2C_MEMADD_SIZE_8BIT, pData, Size, (uint32_t)1000);
	if (res != HAL_OK) {			
		 if(EEPROM_DBG_PRINT_F){			 
			  dbg_print("EEPROM: READ ERROR!\r\n");
				sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
				dbg_print(dbg_str);
				sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
				dbg_print(dbg_str);
		 }
		 return 1;	
	 }	
	 return 0;	
}


uint8_t  EEPROM_Read(uint16_t MEM_ADDR, uint16_t Size, uint8_t * pData)
{
	HAL_StatusTypeDef res;
	
	res = HAL_I2C_Mem_Read (EEPROM_I2C_PORT, (uint16_t) EEPROM_ADDR, MEM_ADDR, I2C_MEMADD_SIZE_8BIT, pData, Size, (uint32_t)1000);	 
	if (res != HAL_OK) {			
		 if(EEPROM_DBG_PRINT_F){
				dbg_print("EEPROM: READ ERROR!\r\n");
				sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
				dbg_print(dbg_str);
				sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
				dbg_print(dbg_str);
		 }
		 return 1;	
	 }	
	 return 0;	
}

 uint8_t EEPROM_IsDeviceReady(){
	 HAL_StatusTypeDef res = HAL_I2C_IsDeviceReady( EEPROM_I2C_PORT, EEPROM_ADDR, 3, HAL_MAX_DELAY);		
	 if (res != HAL_OK) {			
		 if(EEPROM_DBG_PRINT_F){
				dbg_print("EEPROM: READ ERROR!\r\n");
				sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
				dbg_print(dbg_str);
				sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
				dbg_print(dbg_str);
		 }
		 return 1;	
	 }	
	 return 0;	
}

//#define HAL_I2C_ERROR_NONE       0x00000000U    /*!< No error           */
//#define HAL_I2C_ERROR_BERR       0x00000001U    /*!< BERR error         */
//#define HAL_I2C_ERROR_ARLO       0x00000002U    /*!< ARLO error         */
//#define HAL_I2C_ERROR_AF         0x00000004U    /*!< AF error           */
//#define HAL_I2C_ERROR_OVR        0x00000008U    /*!< OVR error          */
//#define HAL_I2C_ERROR_DMA        0x00000010U    /*!< DMA transfer error */
//#define HAL_I2C_ERROR_TIMEOUT    0x00000020U    /*!< Timeout Error      */
