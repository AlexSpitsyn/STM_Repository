#include "i2c_tools.h"

#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include "stdarg.h"


#include "usart.h"
	


//======================= HAL_StatusTypeDef =================
//  HAL_OK       = 0x00U,
//  HAL_ERROR    = 0x01U,
//  HAL_BUSY     = 0x02U,
//  HAL_TIMEOUT  = 0x03U 
//======================= HAL_StateTypeDef =================
//		HAL_I2C_STATE_RESET ,//           	= 0x00U,   /*!< Peripheral is not yet Initialized         */
//		HAL_I2C_STATE_READY,//            	= 0x20U,   /*!< Peripheral Initialized and ready for use  */
//		HAL_I2C_STATE_BUSY,//             	= 0x24U,   /*!< An internal process is ongoing            */
//		HAL_I2C_STATE_BUSY_TX,//           	= 0x21U,   /*!< Data Transmission process is ongoing      */
//		HAL_I2C_STATE_BUSY_RX,//           	= 0x22U,   /*!< Data Reception process is ongoing         */
//		HAL_I2C_STATE_LISTEN,//            	= 0x28U,   /*!< Address Listen Mode is ongoing            */
//		HAL_I2C_STATE_BUSY_TX_LISTEN,//    	= 0x29U,   /*!< Address Listen Mode and Data Transmission
//				 //																											 process is ongoing                         */
//		HAL_I2C_STATE_BUSY_RX_LISTEN,//    	= 0x2AU,   /*!< Address Listen Mode and Data Reception
//				 //																											 process is ongoing                         */
//		HAL_I2C_STATE_ABORT,//             	= 0x60U,   /*!< Abort user request ongoing                */
//		HAL_I2C_STATE_TIMEOUT,//           	= 0xA0U,   /*!< Timeout state                             */
//		HAL_I2C_STATE_ERROR//             	= 0xE0U    /*!< Error    
//=======================  I2C Error Code definition =================
//		HAL_I2C_ERROR_NONE,//              0x00000000U    /*!< No error              */
//		HAL_I2C_ERROR_BERR,//              0x00000001U    /*!< BERR error            */
//		HAL_I2C_ERROR_ARLO,//              0x00000002U    /*!< ARLO error            */
//		HAL_I2C_ERROR_AF,//                0x00000004U    /*!< AF error              */
//		HAL_I2C_ERROR_OVR,//               0x00000008U    /*!< OVR error             */
//		HAL_I2C_ERROR_DMA,//               0x00000010U    /*!< DMA transfer error    */
//		HAL_I2C_ERROR_TIMEOUT,//           0x00000020U    /*!< Timeout Error         */
//		HAL_I2C_ERROR_SIZE,//              0x00000040U    /*!< Size Management error */
//		HAL_I2C_ERROR_DMA_PARAM,//         0x00000080U    /*!< DMA Parameter Error   */
//		HAL_I2C_WRONG_START//              0x00000200U    /*!< Wrong start Error     */
		

HAL_I2C_StateTypeDef i2c_tool_state=0;		
uint8_t I2C_ADDRESS;
uint32_t I2C_MEMADD_SIZE=I2C_MEMADD_SIZE_16BIT;



HAL_StatusTypeDef I2C_set_addr(uint8_t dev_addr){ 
	I2C_ADDRESS = dev_addr;
	return HAL_I2C_IsDeviceReady(I2C_PORT, I2C_ADDRESS<<1, 1, 1000);
}	

void I2C_set_memadd_size(uint32_t memadd_size){ 
	I2C_MEMADD_SIZE = memadd_size;
}	

uint8_t I2C_get_memadd_size(void){
	return (uint8_t)I2C_MEMADD_SIZE;
}	

uint8_t I2C_Scan(uint8_t* dev_addr){
uint8_t cnt=0;	
		
    for(uint8_t i = 0; i < 128; i++){					
				HAL_I2C_StateTypeDef state =  HAL_I2C_GetState(I2C_PORT);
				if (state == HAL_I2C_STATE_READY){					
					HAL_StatusTypeDef status =  HAL_I2C_IsDeviceReady(I2C_PORT, i << 1, 1, 1000);
					//HAL_Delay(200);
					if(status == HAL_OK){		
						*dev_addr = i;
						dev_addr++;		
						*dev_addr = 0;
						cnt++;						
					}
				}
		}    
		return cnt;
}




//HAL_StatusTypeDef I2C_read_byte (uint8_t* dbyte){	
//	return HAL_I2C_Master_Receive(I2C_PORT,I2C_ADDRESS<<1, dbyte, 1, 1000);	
//}
//HAL_StatusTypeDef I2C_write_byte (uint8_t dbyte){	
//	return HAL_I2C_Master_Transmit(I2C_PORT,I2C_ADDRESS<<1, &dbyte, 1, 1000);
//}



HAL_StatusTypeDef I2C_read_nbyte (uint8_t size, uint8_t* dbyte){	
	HAL_I2C_Master_Receive(I2C_PORT,I2C_ADDRESS<<1, dbyte, size, 1000);	
}
HAL_StatusTypeDef I2C_write_nbyte (uint8_t size, uint8_t* dbyte){		
	return HAL_I2C_Master_Transmit(I2C_PORT, I2C_ADDRESS<<1, dbyte, size, 1000);	
}


HAL_StatusTypeDef I2C_write_eeprom (uint16_t mem_addr, uint16_t size, uint8_t* pData){
	return HAL_I2C_Mem_Write (I2C_PORT, I2C_ADDRESS<<1, mem_addr, I2C_MEMADD_SIZE, pData, size, HAL_MAX_DELAY);	
}

HAL_StatusTypeDef I2C_read_eeprom(uint16_t mem_addr, uint16_t size, uint8_t* pData){	
	return HAL_I2C_Mem_Read (I2C_PORT, I2C_ADDRESS<<1, mem_addr, I2C_MEMADD_SIZE, pData, size, HAL_MAX_DELAY);	 	
}
