
#ifndef __BUTTONS_IO_EXP_H
#define __BUTTONS_IO_EXP_H


#include <stdio.h>
#include "io_expander.h"
#include "sys_config.h"
#include "stm32f1xx_hal.h"


//========================     BUTTONS     ========================
//#define BUTTON_DBG_PRINT_F		1
extern volatile _Bool btn_pressed_f;


#define BTN1_CODE 									0x01
#define BTN2_CODE 									0x02
#define BTN3_CODE 									0x04
#define BTN4_CODE 									0x00

#define BTN_SEL 										BTN1_CODE
#define BTN_UP 											BTN2_CODE
#define BTN_DOWN 										BTN3_CODE
//#define BTN_ESC										BTN4_CODE 

//P0 - UP
//P1 - DOWN
//P2 - SEL


#define LONG_PRESS(x)								((x) << 4)
#define LONG_PRESS_TIME							1000



void Buttons_Reset(void);
uint8_t Buttons_GetCode(void);



# endif