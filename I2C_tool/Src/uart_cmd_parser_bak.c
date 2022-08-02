#include "uart_cmd_parser.h"

#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include "stdarg.h"


#include "usart.h"
	
#define I2C_PORT 	&hi2c2
#define state_code_cnt 11
#define error_code_cnt 10

#define UVARS_CNT 10
#define DEC_STR 10
#define HEX_STR 16

typedef struct {		
	char cmd[10];
	uint8_t argc;
	const char man[64];	
} cmd_avalbl_t;





typedef struct {	
	uint8_t argc;
	char* argv[20];
} cmd_t;




const static	uint8_t error_code_desc [error_code_cnt][10] = {
		"NONE",
		"BERR",
		"ARLO",
		"AF",
		"OVR",
		"DMA",
		"TIMEOUT",
		"SIZE",
		"DMA_PARAM",
		"START"		
	};
const static	uint8_t state_code_desc [state_code_cnt][16] = {
		"RESET",
		"READY",
		"BUSY",
		"BUSY_TX",
		"BUSY_RX",
		"LISTEN",
		"BUSY_TX_LISTEN",
		"BUSY_RX_LISTEN",
		"ABORT",
		"TIMEOUT",
		"ERROR"
	};



void uartInit(void) {  

  putsUSART("\r\n");
  putsUSART("\r\n");
  putsUSART("             |||||||||||||||\r\n");
  putsUSART("         ||||||||||||||||||||||\r\n");
  putsUSART("       ||||||||||||||||||||||||||\r\n");
  putsUSART("      ||||     |||||||||      |||||\r\n");
  putsUSART("    ||||     ||||||||||||||    |||||\r\n");
  putsUSART("   |||||  ||||||||||||||||||||  |||||\r\n");
  putsUSART("   ||||| |||||||||     |||||||| ||||||\r\n");
  putsUSART("  ||||||||||||              |||||||||||\r\n");
  putsUSART("  ||||||||||                  |||||||||\r\n");
  putsUSART(" |||||||||                      |||||||\r\n");
  putsUSART(" ||||||||                        ||||||\r\n");
  putsUSART(" |||||||                          |||||\r\n");
  putsUSART("  ||||||                          |||||\r\n");
  putsUSART("  |||||                           |||||\r\n");
  putsUSART("  |||||                           ||||\r\n");
  putsUSART("   ||||                           ||||\r\n");
  putsUSART("    ||||          |||||||||       |||\r\n");
  putsUSART("     ||||    ||  |||||||||||||   |||\r\n");
  putsUSART("       ||||||||  |||||||||||||||||\r\n");
  putsUSART("        |||||||  |||||||||||||||\r\n");
  putsUSART("           |||||||||||||||||||\r\n");
  putsUSART("               |||||||||||\r\n");
  putsUSART("                                    ||||\r\n");
  putsUSART("       ||||||    ||||||        |||||||||\r\n");
  putsUSART("      |||||||  ||||||||||    |||||||||||\r\n");
  putsUSART("     |||||    ||||||||||||  ||||||||||||\r\n");
  putsUSART("     ||||     ||||    ||||| ||||    ||||\r\n");
  putsUSART("    |||||     ||||    ||||| ||||    ||||\r\n");
  putsUSART("||||||||       ||||||||||||  |||||||||||\r\n");
  putsUSART("|||||||         |||||||||||   ||||||||||\r\n");
  putsUSART("I2C Scanner v.1\r\n> ");

}
cmd_t cmd;
char msg[64];
uint8_t I2C_ADDRESS;

#define CMD_CNT 11
	
const cmd_avalbl_t cmd_list[CMD_CNT] = {							
		{"help", 	1, NULL},                              
		{"scan",	1, NULL},
		{"set",   2, "i2c_addr[0x00-0x7F]"},
		{"wr",  	2, "data[0x00-0xFF]"},
		{"rd",		1, NULL},
		{"wreg",  3, "reg[0x00-0xFF] data[0x00-0xFF]"},
		{"rreg",	2, "reg[0x00-0xFF]"},
		{"wmem",  3, "mem_addr[0x00-0xFF] data1[0x00-0xFF]..data15"},
		{"rmem",	3, "mem_addr[0x00-0xFF] size[0x00-0xFF]"},
		{"debug",	1, NULL},
		{"scm",		1, NULL},
		
											
	};


void I2C_Scan();
void I2C_write_byte (uint8_t dbyte);
void I2C_read_byte (uint8_t* dbyte);
void I2C_write_reg (uint8_t reg_addr, uint8_t dbyte);
void I2C_read_reg (uint8_t reg_addr, uint8_t* dbyte);
uint8_t I2C_write_eeprom (uint16_t mem_addr, uint16_t size, uint8_t* pData);
uint8_t I2C_read_eeprom(uint16_t mem_addr, uint16_t size, uint8_t* pData);
	
uint8_t get_i2c_error_str();
uint8_t get_i2c_state_str();
uint8_t str_to_int32(char* str, char** end, uint8_t str_format, uint32_t* val, uint32_t vmin, uint32_t vmax);
void remove_spaces(char *str);
void get_cmd(char *str);
	

void parseCommand(char *buf) {
	
  uint8_t i, f, c;
	uint8_t reg;
	uint8_t data;
	uint16_t mem_addr;
	uint16_t size;
	uint8_t mem_data[64];
	
//  char *pArg1 = NULL, *pArg2 = NULL, *pEnd = NULL;
//  uint8_t l = 0;
//  char answer[64] = "";
  
	get_cmd(buf);
  
	putsUSART("\r\n");				
  for (c = 0; c < CMD_CNT; c++) {		
    if (strncmp(cmd.argv[0], cmd_list[c].cmd, strlen(cmd_list[c].cmd)) == 0) {
      if(cmd.argc < cmd_list[c].argc){
				putsUSART("Wrong argument count\r\n");				
				return;
			}
      break;
    }
  }

		//printf("argv[i] - %s\r\n", cmd.argv[0]);
		
//		printf("cmd = %s\r\n", cmd_list[c].cmd);
//		printf("argc = %d\r\n", cmd.argc);
		

  
	switch (c) {


    case 0: //help 
      break;

    case 1: //scan 
		I2C_Scan();     
		break;

    case 2:  // set			
		if(str_to_int32(cmd.argv[1], NULL, HEX_STR, (uint32_t*)&I2C_ADDRESS, 0, 127)){
			putsUSART("Wrong ADDR value [0x00-0x7F]");
		}
		snprintf(msg, sizeof(msg), "I2C ADDRESS set 0x%02X", I2C_ADDRESS);						
		putsUSART(msg);
		break;
		
		case 3:  // wr 				
		if(str_to_int32(cmd.argv[1], NULL, HEX_STR, (uint32_t*)&data, 0, 255)){
			snprintf(msg, sizeof(msg),"Wrong command args: %s %s", cmd_list[i].cmd, cmd_list[i].man);
			putsUSART(msg);					
		}else{
			I2C_write_byte(data);
		}
		break;
		
		case 4:  // rd   
		I2C_read_byte(&data);
		break;

		case 5:  // wreg   
		if(str_to_int32(cmd.argv[1], NULL, HEX_STR, (uint32_t*)&reg, 0, 255) | 
			 str_to_int32(cmd.argv[2], NULL, HEX_STR, (uint32_t*)&data, 0, 255)){
			snprintf(msg, sizeof(msg),"Wrong command args: %s %s", cmd_list[i].cmd, cmd_list[i].man);
			putsUSART(msg);					
		}else{
			I2C_write_reg(reg, data);
		}
		break;
		
		case 6:  // rreg   
		if(str_to_int32(cmd.argv[1], NULL, HEX_STR, (uint32_t*)&reg, 0, 255)){
			snprintf(msg, sizeof(msg),"Wrong command args: %s %s", cmd_list[i].cmd, cmd_list[i].man);
			putsUSART(msg);					
		}else{
			I2C_read_reg(reg, &data);
		}
		break;
		
		case 7:  // wmem  
		f = str_to_int32(cmd.argv[1], NULL, HEX_STR, (uint32_t*)&mem_addr, 0, 0xFFFF);
		size = cmd.argc-2;
		for(i=0;i<size;i++){
			f|= str_to_int32(cmd.argv[i+2], NULL, HEX_STR, (uint32_t*)&mem_data[i], 0, 0xFF);
		}
		if(f){
			snprintf(msg, sizeof(msg),"Wrong command args: %s %s", cmd_list[i].cmd, cmd_list[i].man);
			putsUSART(msg);					
		}else{			
			I2C_write_eeprom(mem_addr, size, mem_data);			
		}
			
		break;
		
		case 8:  // rmem  		
		
		if(str_to_int32(cmd.argv[1], NULL, HEX_STR, (uint32_t*)&mem_addr, 0, 0xFFFF)|
			 str_to_int32(cmd.argv[2], NULL, HEX_STR, (uint32_t*)&size, 0, 0xFFFF)){
			snprintf(msg, sizeof(msg),"Wrong command args: %s %s", cmd_list[i].cmd, cmd_list[i].man);
			putsUSART(msg);					
		}else{
			I2C_read_eeprom(mem_addr, size, mem_data);
		}
		break;
		
		
		case 9:  // dbg									
		
		snprintf(msg, sizeof(msg), "argc = %d", cmd.argc);	
		putsUSART(msg);
		for(i=0; i< cmd.argc; i++){
			putsUSART(cmd.argv[i]);
		}
		break;
		
		case 10:  // scm software control mode									
		putsUSART("scm!");   	
		soft_ctrl_mode =1;

		break;

    default:     
		putsUSART("command unknown");      
		break;
  }
	
	
	putsUSART("\r\n>");
  
}

void print_i2c_status(HAL_StatusTypeDef status){
	if (status == HAL_ERROR){
		if (get_i2c_error_str()){
			putsUSART("I2C ERROR: " );	
			putsUSART(error_code_desc[get_i2c_error_str()]);
			putsUSART("\r\n" );	
		}
	}else if (status == HAL_BUSY){
		putsUSART("I2C BUSY\r\n");	
	}else if (status == HAL_TIMEOUT){
		putsUSART("I2C TIMEOUT\r\n");						
	}	


}

void I2C_Scan(){    
		
		putsUSART("Scanning I2C bus...\r\n");
		putsUSART( "Dev address: \r\n")
    for(uint16_t i = 0; i < 128; i++){					
				HAL_I2C_StateTypeDef state =  HAL_I2C_GetState(I2C_PORT);
				if (state == HAL_I2C_STATE_READY){					
					HAL_StatusTypeDef status =  HAL_I2C_IsDeviceReady(I2C_PORT, i << 1, 1, 1000);
					//HAL_Delay(200);
					if(status == HAL_OK){		
						snprintf(msg, sizeof(msg), "0x%02X\r\n", i);
						//snprintf(msg, sizeof(msg), "------FOUND------\r\n");
						putsUSART(msg);								
					}else{
						print_i2c_status(status);
					}
				}
		}    
}

void I2C_write_byte (uint8_t dbyte){
	
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(I2C_PORT,I2C_ADDRESS<<1, &dbyte, 1, 1000);
	if(status == HAL_OK){	
		putsUSART("OK\r\n");				
	}else{
		print_i2c_status(status);
	}
}

void I2C_read_byte (uint8_t* dbyte){
	
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(I2C_PORT,I2C_ADDRESS<<1, dbyte, 1, 1000);
	if(status == HAL_OK){	
		snprintf(msg, sizeof(msg), "READ: 0x%02X\r\n", *dbyte);
		putsUSART(msg);				
	}else{
		print_i2c_status(status);
	}
}

void I2C_write_reg (uint8_t reg_addr, uint8_t dbyte){
	
	uint8_t wr_data[2] = {reg_addr, dbyte};
	
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(I2C_PORT,I2C_ADDRESS<<1, wr_data, 2, 1000) ;
	if(status == HAL_OK){	
		putsUSART("OK\r\n");				
	}else{
		print_i2c_status(status);
	}	
}

void I2C_read_reg (uint8_t reg_addr, uint8_t* dbyte){
	
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(I2C_PORT, I2C_ADDRESS<<1, &reg_addr, 1, 1000) | HAL_I2C_Master_Receive(I2C_PORT,I2C_ADDRESS<1, dbyte, 1, 1000);
	if(status == HAL_OK){	
		snprintf(msg, sizeof(msg), "READ: 0x%02X\r\n", *dbyte);
		putsUSART(msg);					
	}else{
		print_i2c_status(status);
	}	
}



uint8_t I2C_write_eeprom (uint16_t mem_addr, uint16_t size, uint8_t* pData){

	HAL_StatusTypeDef status  = HAL_I2C_Mem_Write (I2C_PORT, I2C_ADDRESS<<1, mem_addr, I2C_MEMADD_SIZE_16BIT, pData, size, HAL_MAX_DELAY);
	if(status == HAL_OK){	
		snprintf(msg, sizeof(msg), "Write %d byte OK\r\n", size);
		putsUSART(msg);					
	}else{
		print_i2c_status(status);
	}
}


uint8_t I2C_read_eeprom(uint16_t mem_addr, uint16_t size, uint8_t* pData){
	
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read (I2C_PORT, I2C_ADDRESS<<1, mem_addr, I2C_MEMADD_SIZE_16BIT, pData, size, HAL_MAX_DELAY);	 
	if(status == HAL_OK){	
		snprintf(msg, sizeof(msg), "READ ADDR: 0x%02X %d byte\r\n", mem_addr, size);
		putsUSART(msg);		


		for(uint16_t i=0; i < size; i++){
			
			if(i%8==0)
					putsUSART("\r\n");	
			snprintf(msg, sizeof(msg), "0x%02X ", pData[i]);
			putsUSART(msg);	
    }
					
	}else{
		print_i2c_status(status);
	}
}

uint8_t get_i2c_state_str(){


	uint8_t state_code[state_code_cnt] = {
		
		HAL_I2C_STATE_RESET ,//           	= 0x00U,   /*!< Peripheral is not yet Initialized         */
		HAL_I2C_STATE_READY,//            	= 0x20U,   /*!< Peripheral Initialized and ready for use  */
		HAL_I2C_STATE_BUSY,//             	= 0x24U,   /*!< An internal process is ongoing            */
		HAL_I2C_STATE_BUSY_TX,//           	= 0x21U,   /*!< Data Transmission process is ongoing      */
		HAL_I2C_STATE_BUSY_RX,//           	= 0x22U,   /*!< Data Reception process is ongoing         */
		HAL_I2C_STATE_LISTEN,//            	= 0x28U,   /*!< Address Listen Mode is ongoing            */
		HAL_I2C_STATE_BUSY_TX_LISTEN,//    	= 0x29U,   /*!< Address Listen Mode and Data Transmission
				 //																											 process is ongoing                         */
		HAL_I2C_STATE_BUSY_RX_LISTEN,//    	= 0x2AU,   /*!< Address Listen Mode and Data Reception
				 //																											 process is ongoing                         */
		HAL_I2C_STATE_ABORT,//             	= 0x60U,   /*!< Abort user request ongoing                */
		HAL_I2C_STATE_TIMEOUT,//           	= 0xA0U,   /*!< Timeout state                             */
		HAL_I2C_STATE_ERROR//             	= 0xE0U    /*!< Error    
	};
	
	uint8_t code;
	code = HAL_I2C_GetState(I2C_PORT);
	for(uint8_t i  = 0; i<state_code_cnt; i++){
		if(code == state_code[i]){
			return i;
		}
	}
	
}
uint8_t get_i2c_error_str(){	
	
		uint32_t error_code[error_code_cnt] = {
		HAL_I2C_ERROR_NONE,//              0x00000000U    /*!< No error              */
		HAL_I2C_ERROR_BERR,//              0x00000001U    /*!< BERR error            */
		HAL_I2C_ERROR_ARLO,//              0x00000002U    /*!< ARLO error            */
		HAL_I2C_ERROR_AF,//                0x00000004U    /*!< AF error              */
		HAL_I2C_ERROR_OVR,//               0x00000008U    /*!< OVR error             */
		HAL_I2C_ERROR_DMA,//               0x00000010U    /*!< DMA transfer error    */
		HAL_I2C_ERROR_TIMEOUT,//           0x00000020U    /*!< Timeout Error         */
		HAL_I2C_ERROR_SIZE,//              0x00000040U    /*!< Size Management error */
		HAL_I2C_ERROR_DMA_PARAM,//         0x00000080U    /*!< DMA Parameter Error   */
		HAL_I2C_WRONG_START//              0x00000200U    /*!< Wrong start Error     */
	};
	
	uint32_t code;
	code = HAL_I2C_GetError(I2C_PORT);
	for(uint8_t i  = 0; i<error_code_cnt; i++){
		if(code == error_code[i]){
			return i;
		}
	}	

}	


uint8_t str_to_int32(char* str, char** end, uint8_t str_format, uint32_t* val, uint32_t vmin, uint32_t vmax){
	uint32_t conv_val;			
	conv_val=strtol(str, end, str_format);
	if((conv_val>=vmin)&&(conv_val<=vmax)){
		*val=conv_val;
		return 0;
		
	}else{
		return 1;
	}
}

void remove_spaces(char *str){	
    char *dest = str;  
 
		if (*str==' ')
			str++;		
	
    while (*str != '\0'){  
			while (*str == ' ' && *(str + 1) == ' ') {
				str++;  
			}
			*dest++ = *str++;
    }
		if (*(dest-1)==' ')
			*(dest-1)= '\0';
		else		
			*dest = '\0';		
		
}

void get_cmd(char *str){

	cmd.argc = 0;	
	remove_spaces(str);	
	cmd.argv[cmd.argc] = str;
	cmd.argc++;
	while (*str != '\0'){  
		if (*str == ' ') {
			*str = 0;	
			cmd.argv[cmd.argc++] = str+1;
			//if(cmd.argc==10)
			//	break;
		}
		
		str++;
		
	} 	
}