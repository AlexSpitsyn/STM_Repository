#include "buttons_io_exp.h"

volatile _Bool btn_pressed_f=0;
volatile uint8_t extTrigger1;
volatile uint8_t extTrigger2;
volatile uint8_t extTrigger3;

btn_t buttons[BTN_CNT];



 void Buttons_Init(void) {   
	 PCF8574_Init(0xFF);
	 PCF8574_ReadReg();
   btn_pressed_f = 0;
	 	 
	 extTrigger1=PCF8574_reg & ETRG1_MASK;
	 extTrigger2=PCF8574_reg & ETRG2_MASK;
	 extTrigger3=PCF8574_reg & ETRG3_MASK;
	 
	 buttons[0].line = BTN1_CODE;
	 buttons[1].line = BTN2_CODE;
	 buttons[2].line = BTN3_CODE;
	 buttons[3].line = BTN4_CODE;
	 
}


//uint8_t Buttons_GetCode(void) {
//   uint8_t btn_code;
//	uint32_t tickstart = 0x00U;
//	
//   PCF8574_ReadReg();
//	

//   btn_code = (~PCF8574_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE );

//   if (!btn_code) {
//      btn_pressed_f = 0;
//      return 0;
//   }
//	
//		if(BUTTON_DBG_PRINT_F){
//		sprintf(dbg_str, "BTN Code: 0x%02X\r\nIO_EXP_Val: 0x%02X\r\n", btn_code, PCF8574_reg);
//		dbg_print(dbg_str);
//	 }
//	 //================
//		tickstart = HAL_GetTick();
//	 
//   while ((~PCF8574_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE )) {
//      PCF8574_ReadReg();
//		 
//		 if (HAL_GetTick()-tickstart > LONG_PRESS_TIME){
//			 btn_pressed_f = 0;
//			 return btn_code = ((~PCF8574_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE )) << 4;//Long Press   
//		 }
//   }
//	 btn_pressed_f = 0;
//	 return btn_code;//Short Press
//	 
//	   

//}


void buttons_handler(void){
uint8_t state, i;
	
	if (btn_pressed_f) {
		if(BUTTON_DBG_PRINT_F){
			sprintf(dbg_str, "PCF8574_reg: 0x%02X\r\n", PCF8574_reg);
			dbg_print(dbg_str);
		}
		btn_pressed_f = 0;
		for(i=0; i<BTN_CNT;i++){
			state = (~PCF8574_reg) & buttons[i].line;		
			if(BUTTON_DBG_PRINT_F){
				sprintf(dbg_str, "BTN state: 0x%02X\r\n state: 0x%02X\r\n", buttons[i].state, state);
				dbg_print(dbg_str);
			}
			if (buttons[i].state != state){
				buttons[i].state = state;
				if(buttons[i].state){						
					buttons[i].press_time = HAL_GetTick();
				}else{
					buttons[i].code=SHORT_PRESS;
				}					
			}
		}
		
	}
	for(i=0; i<BTN_CNT;i++){
		if((HAL_GetTick()-buttons[i].press_time>LONG_PRESS_TIME) && (buttons[i].state)){
			buttons[i].code=LONG_PRESS;
			buttons[i].state=0;
		}
	}	
}


