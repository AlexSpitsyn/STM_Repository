
#ifndef __system_H
#define __system_H

//#include "stm32f1xx_hal.h"
//#include <stdio.h>
//#include <string.h>
//#include "usbd_cdc_if.h"
//#include "tim.h"
//#include "usart.h"

//#include "io_expander.h"
//#include "eeprom.h"
//#include "7seg.h"
#include "sys_config.h"
//#include "ds18b20.h"
//#include "uart_cmd_parser.h"
////#include "hw_crc32.h"
//#include "wireless_ctrl.h"
//#include "1-wire.h"
//#include "SX1278.h"





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
	int16_t drv_move;
}SysCouners_t;

extern volatile SysCouners_t SysCnt;

typedef struct {
  //				Seven Seg
  _Bool ss_blink_f;
 // _Bool view_mode_f;
  _Bool ss_update_f;
  //				TEMP
  int16_t set_temp;
  int16_t temp_ctrl_f;
//	int16_t threshold_temp;
	int16_t t_ctrl_time;
	int16_t t_updt_time;
	int16_t t_hyst;
	int16_t 	pump;

	//int16_t 	burner;
  //				ERROR
  int8_t error_code;
  //_Bool error_f;
	//				UART DBG
	_Bool uart_en_f;
	_Bool uart_dbg_f;

	
}SysState_t;

extern volatile SysState_t SysState;

typedef struct {
    char name[14];
		uint16_t vmin, vmax;
    volatile int16_t *pVal;
		_Bool we;
		int16_t mem_addr;  
} SysVar;

#define SYS_VAR_CNT										11
extern SysVar SV[SYS_VAR_CNT];

enum VarName{
	//TEMP = 0 
	vn_T_CTRL_F=1,
	vn_T_SET,
	vn_T_CTRL_TIME,
	vn_T_UPDT_TIME,
	vn_T_HYST,
	vn_PUMP,	
	vn_DRIVE_POS,
	vn_DRIVE_MAX_POS,
	vn_DRIVE_DRIVE_POS_DEST,
	vn_DRIVE_STEPS
	
};


#define WE														1//Write Enable
#define RO														0//Read Only

// ERROR CODE


#define TEMP_SENSOR_READING_ERROR 		0
#define IO_EXPANDER_READING_ERROR 		1
#define EEPROM_READING_ERROR 					2
#define DRV_OPEN_ERROR								3
#define DRV_CLOSE_ERROR								4
#define DRV_HOME_INIT_ERROR						5

#define ERROR_CHECK_TIME						100



void SysInit(void);
void SystemTask(void);
uint32_t SysVarRW(_Bool rw, SysVar* sv);
void SetTemp(void);
void SetMaxPosition(void);
void pump_set(uint8_t channels);
uint32_t EEPROM_restore(void);


//==============================================================

//========================     LED      ========================


#define LED_ON(x)					PCF8574_PortSet(x)
#define LED_OFF(x)				PCF8574_PortReset(x)
#define LED_TOGGLE(x)			PCF8574_PortToggle(x)


//==============================================================
//========================     PUMP/BURNER      ================
//#define PUMP(x)							x ? PCF8574_PortSet(5): PCF8574_PortReset(5); SysState.pump1=x;
//#define BURNER(x)						x ? PCF8574_PortSet(6): PCF8574_PortReset(6); SysState.burner=x;

//#define PUMP(x)							x ? LED_ON(LED_BLUE): LED_OFF(LED_BLUE); SysState.pump=x;
//#define BURNER(x)						x ? LED_ON(LED_RED): LED_OFF(LED_RED); SysState.burner=x;

//#define WF_PUMP_ON()					SysState.pump|=0x0101;
//#define WF_PUMP_OFF()					SysState.pump&=0x00FE;SysState.pump|=0x0100;

#define WF_PUMP_ON()					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET)
#define WF_PUMP_OFF()					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET)

//======================================== ====================
//========================     OPTOCUPLER CH B      ===========

#define PHOTO_SENSOR_GET_STATE()		HAL_GPIO_ReadPin(PHOTOSENSOR_A_GPIO_Port, PHOTOSENSOR_A_Pin)

//==============================================================

//========================     7 SEG    ========================


#define SS_BLINK_TIME 							10
#define SS_VIEW_MODE_TIME 					120
#define SS_PRINT_CUR_POS() 					putcSS(SEV_SEG_P);SevSeg_PrintDec(drv_m1.position);
#define SS_PRINT_POS(x) 						putcSS(SEV_SEG_P);SevSeg_PrintDec(x);
#define SS_PRINT_TEMP(x) 						putcSS(SEV_SEG_t); SevSeg_PrintDec(x);
#define SS_PRINT_ERROR_CODE(x) 			putcSS(SEV_SEG_E); SevSeg_PrintDec(x);
#define SS_PRINT_T_NONE() 					putcSS(SEV_SEG_t); putcSS(0); putcSS(SEV_SEG_NONE); putcSS(SEV_SEG_NONE); SS_LATCH();
#define SS_BLINK(x) 								SysState.ss_blink_f = x; SS_OE(x); SysCnt.blink=0;
#define TEMP_VIEW 									0
#define POSITION_VIEW 							1
#define SS_SHOW(x) 									SysState.view_mode_f = x; SysState.ss_update_f = 1; SysCnt.view_mode = 0;


//==============================================================

//========================     TEMP SENSOR======================

#define TEMP_MAX_LIMIT 					50
#define TEMP_MIN_LIMIT 					0


//==============================================================

//========================     EEPROM   ========================

#define EEPROM_OFFSET									2

#define WR														1//Write
#define RD														0//Read




//==============================================================

//========================    SX1278   ========================



//==============================================================
//========================     DRIVE_CTRL   ====================






//==============================================================

# endif
