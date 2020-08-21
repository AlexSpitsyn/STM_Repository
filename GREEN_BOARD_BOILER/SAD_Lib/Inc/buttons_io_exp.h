
#ifndef __BUTTONS_IO_EXP_H
#define __BUTTONS_IO_EXP_H


#include <stdio.h>
#include "io_expander.h"
#include "sys_config.h"
#include "stm32f1xx_hal.h"


//========================     BUTTONS     ========================
//#define BUTTON_DBG_PRINT_F		1
extern volatile _Bool btn_pressed_f;

typedef struct {	
	uint8_t line;
	uint8_t code;
	uint8_t state;
	uint32_t press_time;	
} btn_t; 

#define BTN_CNT 3
extern btn_t buttons[BTN_CNT];

//if not used set to 0x00
#define BTN1_CODE 									0x01
#define BTN2_CODE 									0x02
#define BTN3_CODE 									0x04
#define BTN4_CODE 									0x00



#define BTN_SEL 										buttons[2].code
#define BTN_UP 											buttons[1].code
#define BTN_DOWN 										buttons[0].code

//ext triggers
#define ETRG1_MASK 									0x20
#define ETRG2_MASK 									0x40
#define ETRG3_MASK 									0x80

extern volatile uint8_t extTrigger1;
extern volatile uint8_t extTrigger2;
extern volatile uint8_t extTrigger3;



//#define LONG_PRESS(x)								((x) << 4)
#define SHORT_PRESS										1
#define LONG_PRESS										2
#define LONG_PRESS_TIME								1000




void Buttons_Init(void);
//uint8_t Buttons_GetCode(void);
void buttons_handler(void);


# endif
