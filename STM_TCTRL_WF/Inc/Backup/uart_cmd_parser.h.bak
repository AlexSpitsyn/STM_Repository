
#ifndef UART_CMD_PARSER_H
#define	UART_CMD_PARSER_H

#include "stm32f1xx_hal.h"

//#define BUFFER_SIZE 64

//unsigned char msg_flag;
//unsigned char msg_len;




//#define putsUSART(x)	HAL_UART_Transmit(&huart1, (uint8_t*)x, strlen(x), 0xFFFF);
//#define putsUSART(x)	CDC_Transmit_FS( (uint8_t*)x, strlen(x));

void uartInit(void);

void parseCommand(char *buf);
uint8_t strtodec(char* str, char** end, uint16_t* val, uint16_t vmin, uint16_t vmax);

#endif	/* CMAD_PARSER_H */

