
#ifndef UART_CMD_PARSER_H
#define	UART_CMD_PARSER_H

#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"
#include "i2c_tools.h"

//#define BUFFER_SIZE 64

//unsigned char msg_flag;
//unsigned char msg_len;



//#define putsUSART(x)	HAL_UART_Transmit(&huart1, (uint8_t*)x, strlen(x), 0xFFFF);
//#define putsUSART(x)	CDC_Transmit_FS( (uint8_t*)x, strlen(x));

void uartInit(void);

void parseCommand(char *buf);

#endif	/* CMAD_PARSER_H */

