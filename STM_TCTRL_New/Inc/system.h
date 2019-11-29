
#ifndef __system_H
#define __system_H

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include "io_expander.h"
#include "eeprom.h"
#include "7seg.h"
#include "drive.h"
#include "ds18b20.h"
#include "main.h"
#include "usart.h"
#include "tim.h"
#include "uart_cmd_parser.h"
#include "UART_DBG.h"
#include "nrf24.h"
#include "crc.h"
#include "wireless_ctrl.h"


#define DRIVE_CTRL
#define TEMP_SENS
#define TEMP_CTRL
#define SSEG
#define NRF24
#define BUTTONS
#define DS18B20
#define EEPROM


//========================     SYS 					=====================
#define ON 1
#define OFF 0

typedef struct {
  int16_t timeout;
  int16_t temp_update;
  int16_t error_check;
  int16_t temp_ctrl;
  int16_t blink;
  int16_t view_mode;
}SysCouners_t;

extern volatile SysCouners_t SysCnt;

typedef struct {
  //				Seven Seg
  _Bool ss_blink_f;
  _Bool view_mode_f;
  _Bool ss_update_f;
  //				TEMP
  int16_t set_temp;
  int16_t temp_ctrl_f;
	int16_t t_ctrl_time;
	int16_t t_updt_time;
	int16_t t_hyst;
  //				ERROR
  int8_t error_code;
  _Bool error_f;
	//				UART DBG
	_Bool uart_en_f;
	_Bool uart_dbg_f;
		//				NRF24
	_Bool nrf24_en_f;
	
}SysState_t;

extern volatile SysState_t SysState;

typedef struct {
    char name[14];
		uint16_t vmin, vmax;
    volatile int16_t *pVal;
		_Bool we;
		int16_t mem_addr;  
} SysVar;



enum VarName{
	vn_T_CTRL_F=1,
	vn_T_SET,
	vn_T_CTRL_TIME,
	vn_T_UPDT_TIME,
	vn_T_HYST,
	vn_DRIVE_POS,	
	vn_DRIVE_MAX_POS,
	vn_DRIVE_STEPS=9
};

extern SysVar SV[];

//Sys Var Type
//#define TEMP_CTRL_F									*(SV[vn_T_CTRL_F].pVal)
//#define TEMP_SET										*(SV[vn_T_SET].pVal)
//#define TEMP_CTRL_TIME							*(SV[vn_T_CTRL_TIME].pVal)
//#define TEMP_UPDT_TIME							*(SV[vn_T_UPDT_TIME].pVal)
//#define TEMP_HYST										*(SV[vn_T_HYST].pVal)
//#define DRIVE_STEPS									*(SV[vn_DRIVE_STEPS].pVal)

#define WE														1//Write Enable
#define RO														0//Read Only

// ERROR CODE

#define DRV_OPEN_ERROR 							0x01
#define DRV_CLOSE_ERROR 						0x02
#define TEMP_SENSOR_READING_ERROR 	0x04
#define IO_EXPANDER_READING_ERROR 	0x08
#define EEPROM_READING_ERROR 				0x10

#define ERROR_CHECK_TIME						100



void SysInit(void);
void SystemTask(void);
void SysVarRW(_Bool rw, SysVar* sv);
void SetTemp(void);
void SetMaxPosition(void);



//==============================================================

//========================     LED      ========================

#define LED_GREEN(x) 								HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, x);
#define LED_GREEN_Toggle() 					HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

//==============================================================

//========================     BUTTONS     ========================
#ifdef BUTTONS
extern volatile _Bool btn_pressed_f;

#define BTN1_CODE 									0x01
#define BTN2_CODE 									0x02
#define BTN3_CODE 									0x04
#define BTN4_CODE 									0x00

#define BTN_UP 											BTN1_CODE
#define BTN_DOWN 										BTN2_CODE
#define BTN_SEL 										BTN3_CODE
//#define BTN_ESC										BTN4_CODE 

//P0 - UP
//P1 - DOWN
//P2 - SEL


#define LONG_PRESS(x)								((x) << 4)
#define LONG_PRESS_TIME							1000



void Buttons_Reset(void);
uint8_t Buttons_GetCode(void);

//				if(btn_pressed_f ){
//					switch (Buttons_GetCode())
//          {
//          	case BTN_UP: 
//								printf("UP\r\n");
//          		break;
//          	case BTN_DOWN: 
//							printf("DOWN\r\n");
//          		break;
//						case BTN_ENTER: 
//							printf("ENTER\r\n");
//          		break;
//						case BTN_ESC: 
//							printf("ESC\r\n");
//          		break;
//						case LONG_PRESS(BTN_UP): 
//								printf("LONG UP\r\n");
//          		break;
//          	case LONG_PRESS(BTN_DOWN): 
//							printf("LONG DOWN\r\n");
//          		break;
//						case LONG_PRESS(BTN_ENTER): 
//							printf("LONG ENTER\r\n");
//          		break;
//						case LONG_PRESS(BTN_ESC): 
//							printf("LONG ESC\r\n");
//          		break;
//          	default:
//							printf("NONE\r\n");
//          		break;
//          }
//				} 
#endif

//==============================================================

//========================     7 SEG    ========================
#ifdef SSEG

#define SS_BLINK_TIME 							10
#define SS_VIEW_MODE_TIME 					120
#define SS_PRINT_CUR_POS() 					putcSS(SEV_SEG_P);PrintDecSevSeg(drv_m1.position);
#define SS_PRINT_POS(x) 						putcSS(SEV_SEG_P);PrintDecSevSeg(x);
#define SS_PRINT_TEMP(x) 						putcSS(SEV_SEG_t); PrintDecSevSeg(x);
#define SS_PRINT_ERROR_CODE(x) 			putcSS(SEV_SEG_E); PrintDecSevSeg(x);
#define SS_PRINT_T_NONE() 					putcSS(SEV_SEG_t); putcSS(0); putcSS(SEV_SEG_NONE); putcSS(SEV_SEG_NONE); SS_LATCH();
#define SS_BLINK(x) 								SysState.ss_blink_f = x; SS_OE(x); SysCnt.blink=0;
#define TEMP_VIEW 									0
#define POSITION_VIEW 							1
#define SS_SHOW(x) 									SysState.view_mode_f = x; SysState.ss_update_f = 1; SysCnt.view_mode = 0;

#endif
//==============================================================

//========================     TEMP SENSOR======================
# ifdef DS18B20
#define TEMP_MAX_LIMIT 					45
#define TEMP_MIN_LIMIT 					0

# endif
//==============================================================

//========================     EEPROM   ========================
# ifdef EEPROM
#define EEPROM_OFFSET									2

#define WR														1//Write
#define RD														0//Read



# endif
//==============================================================

//========================     NRF24    ========================
# ifdef NRF24

# endif
//==============================================================
//========================     DRIVE_CTRL   ====================
# ifdef DRIVE_CTRL


#define DRIVE_MAX_POS_LIMIT 					850
#define DRIVE_MIN_POS_LIMIT 					0

# endif
//==============================================================

# endif
