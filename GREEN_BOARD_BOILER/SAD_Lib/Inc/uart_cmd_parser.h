
#ifndef UART_CMD_PARSER_H
#define	UART_CMD_PARSER_H

#include "sys_config.h"


void uartInit(void);

void parseCommand(char *buf);

uint8_t str_to_int32(char* str, char** end, uint8_t str_format, uint32_t* val, uint32_t vmin, uint32_t vmax);
void remove_spaces(char *str);
#endif	/* CMAD_PARSER_H */

