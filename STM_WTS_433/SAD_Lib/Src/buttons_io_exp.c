#include "buttons_io_exp.h"

volatile _Bool btn_pressed_f=0;




 void Buttons_Reset(void) {   
	 PCF8574_Init(0xFF);
	 PCF8574_ReadReg();
   btn_pressed_f = 0;
}


uint8_t Buttons_GetCode(void) {
   uint8_t btn_code;
	uint32_t tickstart = 0x00U;
	
   PCF8574_ReadReg();
	

   btn_code = (~PCF8574_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE);

   if (!btn_code) {
      btn_pressed_f = 0;
      return 0;
   }
	
		if(BUTTON_DBG_PRINT_F){
		sprintf(dbg_str, "BTN Code: 0x%02X\r\nIO_EXP_Val: 0x%02X\r\n", btn_code, PCF8574_reg);
		dbg_print(dbg_str);
	 }
	 //================
		tickstart = HAL_GetTick();
	 
   while ((~PCF8574_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE)) {
      PCF8574_ReadReg();
		 
		 if (HAL_GetTick()-tickstart > LONG_PRESS_TIME){
			 btn_pressed_f = 0;
			 return btn_code = ((~PCF8574_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE)) << 4;//Long Press   
		 }
   }
	 btn_pressed_f = 0;
	 return btn_code;//Short Press
	 
	   

}
