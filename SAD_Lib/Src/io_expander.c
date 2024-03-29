#include "io_expander.h"

uint8_t PCF8574_reg;
uint8_t PCF8574_config;
//=======================I2C STATUS CODE=====================
// HAL_StatusTypeDef;
//  HAL_OK       = 0x00U,
//  HAL_ERROR    = 0x01U,
//  HAL_BUSY     = 0x02U,
//  HAL_TIMEOUT  = 0x03U

//=======================I2C ERROR CODE=====================
//#define HAL_I2C_ERROR_NONE       0x00000000U    /*!< No error           */
//#define HAL_I2C_ERROR_BERR       0x00000001U    /*!< BERR error         */
//#define HAL_I2C_ERROR_ARLO       0x00000002U    /*!< ARLO error         */
//#define HAL_I2C_ERROR_AF         0x00000004U    /*!< AF error           */
//#define HAL_I2C_ERROR_OVR        0x00000008U    /*!< OVR error          */
//#define HAL_I2C_ERROR_DMA        0x00000010U    /*!< DMA transfer error */
//#define HAL_I2C_ERROR_TIMEOUT    0x00000020U    /*!< Timeout Error      */

uint32_t PCF8574_ReadReg(void){
	HAL_StatusTypeDef res;
	
	res = HAL_I2C_Master_Receive(PCF8574A_I2C_PORT, PCF8574A_ADDR, &PCF8574_reg, 1, HAL_MAX_DELAY);		
	if (res != HAL_OK) {			
		if(PCF8574A_DBG_PRINT_F){			 
			dbg_print("PCF8574A: READ ERROR!\r\n");
			sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
			dbg_print(dbg_str);
			sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
			dbg_print(dbg_str);
		}
		return 1;	
	}	
	return 0;
			
}

uint32_t PCF8574_WriteReg(uint8_t chanels){
	HAL_StatusTypeDef res;
	
	res = HAL_I2C_Master_Transmit(PCF8574A_I2C_PORT, PCF8574A_ADDR, &chanels, 1, HAL_MAX_DELAY);		
	if (res != HAL_OK) {			
		if(PCF8574A_DBG_PRINT_F){			 
			dbg_print("PCF8574A: WRITE ERROR!\r\n");
			sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
			dbg_print(dbg_str);
			sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
			dbg_print(dbg_str);
		}
		return 1;	
	}	
	return 0;
	
	

}

uint32_t PCF8574_Init(uint8_t chanels){
	HAL_StatusTypeDef res;
	PCF8574_config=chanels;
	res = HAL_I2C_Master_Transmit(PCF8574A_I2C_PORT, PCF8574A_ADDR, &chanels, 1, HAL_MAX_DELAY);		
	if (res != HAL_OK) {			
		if(PCF8574A_DBG_PRINT_F){			 
			dbg_print("PCF8574A: WRITE ERROR!\r\n");
			sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
			dbg_print(dbg_str);
			sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
			dbg_print(dbg_str);
		}
		return 1;	
	}	
	return 0;
	

		

}

uint32_t PCF8574_IsDeviceReady(void){	
	HAL_StatusTypeDef res;
	
	res = HAL_I2C_IsDeviceReady(PCF8574A_I2C_PORT, PCF8574A_ADDR, 1, HAL_MAX_DELAY);		
	if (res != HAL_OK) {			
		if(PCF8574A_DBG_PRINT_F){			 
			dbg_print("PCF8574A: ERROR!\r\n");
			sprintf(dbg_str, "I2C STATUS CODE: %d\r\n", res);
			dbg_print(dbg_str);
			sprintf(dbg_str, "I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( EEPROM_I2C_PORT));
			dbg_print(dbg_str);
		}
		return 1;	
	}	
	return 0;	
	 
}

uint32_t PCF8574_PortReset(uint8_t PortNum){
	
	if(PCF8574_ReadReg())
		return 1;
	if(PCF8574_WriteReg(PCF8574_reg|(1<<PortNum)|PCF8574_config))
		return 1;
	return 0;
}
uint32_t PCF8574_PortSet(uint8_t PortNum){
	if(PCF8574_ReadReg())
		return 1;
	if(PCF8574_WriteReg((PCF8574_reg&~(1<<PortNum))|PCF8574_config))
		return 1;
	return 0;
}
uint32_t PCF8574_PortToggle(uint8_t PortNum){
	if(PCF8574_ReadReg())
		return 1;		
	if(PCF8574_WriteReg((PCF8574_reg^(1<<PortNum))|PCF8574_config))
		return 1;
	return 0;
}
