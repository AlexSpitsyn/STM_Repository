#include "system.h"


uint8_t cdc_init=0;
char dbg_str[64];

void SysErrorCheck(void);
//typedef struct {
//  //				Seven Seg
//  _Bool ss_blink_f;
//  _Bool view_mode_f;
//  _Bool ss_update_f;
//  //				TEMP
//  int16_t set_temp;
//  int16_t temp_ctrl_f;
//	int16_t t_ctrl_time;
//	int16_t t_updt_time;
//  //				ERROR
//  int8_t error_code;
//  _Bool error_f;
//	//				UART DBG
//	_Bool uart_en_f;
//	_Bool uart_dbg_f;
//}SysState_t;

//struct SysCouners_t {
//  int16_t 	timeout;
//	int16_t		temp_update;
//	int16_t		error_check;f?
//	int16_t		temp_ctrl;
//	int16_t		blink;
//	int16_t		view_mode;
//} SysCnt;

//struct SysVar {
//    char name[12];
//		int16_t vmax, vmin;
//    int16_t *pVal;
//		_Bool we;
//		int16_t mem_addr;  

//};
volatile SysState_t SysState={0};
volatile SysCouners_t SysCnt={0};

//struct ext_ctrl_t {    
//		int8_t burner,pump1, pump2;   

//}EXT_CTRL;

//enum VarName{
//	T_CTRL_F_vn,
//	T_SET_vn,
//	T_CTRL_TIME_vn,
//	T_UPDT_TIME_vn,
//	T_HYST_vn,
//	PUMP1_vn,
//	PUMP2_vn
// 
//};

 SysVar SV[SYS_VAR_CNT]= {//0};
{"temp",		 		0, 0, 		(int16_t*)(&DS18B20_TEMP), 		RO, 0},
{"temp_ctrl", 	0, 1, 		&SysState.temp_ctrl_f, 				WE, vn_T_CTRL_F}, 
{"t_set", 			0, TEMP_MAX_LIMIT, 		&SysState.set_temp, 					WE, vn_T_SET},		
{"t_ctrl_time", 0, 5000, 	&SysState.t_ctrl_time, 				WE, vn_T_CTRL_TIME},
{"t_updt_time", 0, 5000, 	&SysState.t_updt_time, 				WE, vn_T_UPDT_TIME},
{"t_hyst", 			0, 10, 		&SysState.t_hyst, 						WE, vn_T_HYST},
{"pump", 				0, 1, 		&SysState.pump, 							WE, vn_PUMP},
{"burner", 			0, 1, 		&SysState.burner, 						RO, vn_BURNER},
{"t_th", 				0, 30, 		&SysState.threshold_temp, 		WE, vn_T_THRESHOLD},
{"reset_cnt",		0, 10000,	&SysState.reset_cnt, 					WE, vn_RESET_CNT}

////{"drv_pos", 		0, 999, 	&drv_m1.position, 						WE, vn_DRIVE_POS*EEPROM_OFFSET},//EEPROM , when set drv_m1.position = drv_m1.dest_pos
////{"drv_pos_max", 0, 999, 	&drv_m1.max_pos, 							WE, vn_DRIVE_MAX_POS * EEPROM_OFFSET},//EEPROM 
////{"drv_pos_dest",0, 0, 		&drv_m1.dest_pos, 						WE, 0},
////{"drv_steps", 	0, 100, 		&drv_m1.steps, 								WE, vn_DRIVE_STEPS * EEPROM_OFFSET}//EEPROM - DRIVE_STEPS
//{"t_hyst", 			0, 10, 		&SysState.t_hyst, 						WE, vn_T_HYST * EEPROM_OFFSET}//EEPROM - TEMP_HYST
////{"all", 0, 0, NULL, 0},
};



//========================     SYS TASK    =====================

void SysInit(void) {
	
	SysCnt.wl_offline=0;
	
	SevSeg_Init();
	
	//	eeprom need to restore
	PCF8574_ReadReg();
	if((~PCF8574_reg) & 0x04){		
		putcSS(SEV_SEG_DOT);
		HAL_Delay(750);
		SS_LATCH();
		putcSS(SEV_SEG_DOT);
		HAL_Delay(750);
		SS_LATCH();
		putcSS(SEV_SEG_DOT);
		HAL_Delay(750);
		SS_LATCH();
		putcSS(SEV_SEG_DOT);
		HAL_Delay(750);
		SS_LATCH();
		PCF8574_ReadReg();
		if((~PCF8574_reg) & 0x04){ //Button SEL pressed		
			if(EEPROM_restore()){
				putcSS(SEV_SEG_F);				
				putcSS(SEV_SEG_A);
				putcSS(SEV_SEG_I);
				putcSS(SEV_SEG_L);
				SS_LATCH();
				while(1);
			}else{
				trulala();
			}			
		}		
	}
//	end restoring
	
	SysState.error_code = 0;
	SysState.error_code |= EEPROM_IsDeviceReady()<< EEPROM_READING_ERROR;
	SysState.error_code |= PCF8574_IsDeviceReady()<< IO_EXPANDER_READING_ERROR;  
	SysState.error_code |= ds18b20_Init(1, RESOLUTION_9BIT) << TEMP_SENSOR_READING_ERROR;  

	
	ds18b20_GetTemp(0);
		
	SysVarRW(RD,&SV[vn_T_CTRL_F]);	
	SysVarRW(RD,&SV[vn_T_SET]);	
	SysVarRW(RD,&SV[vn_T_CTRL_TIME]);
	SysVarRW(RD,&SV[vn_T_UPDT_TIME]);
	SysVarRW(RD,&SV[vn_T_HYST]);
	SysVarRW(RD,&SV[vn_T_THRESHOLD]);	
	SysVarRW(RD,&SV[vn_RESET_CNT]);

	//Start timer1 IT
	HAL_TIM_Base_Start_IT( &htim1);

	
	//Arm UART1
	HAL_UART_Receive_IT( & huart1, & receive_val, 1);
	Buttons_Init();	
	LED_OFF(LED_BLUE);
	LED_OFF(LED_RED);	

	
	SysState.ss_update_f = 1;

	
	SysCnt.temp_ctrl = SysState.t_ctrl_time;

	PUMP(OFF);
	BURNER(OFF);		

			
	
	WL_Init();
	if(SYS_DBG_PRINT_F){
		dbg_print("===== INIT DONE ======\r\n");
	}				 
}

void SystemTask(void) {

static int16_t old_temp_ctrl_f;
	
	 //Reset sys counters if temp_ctrl_f 0->1
	if(old_temp_ctrl_f != SysState.temp_ctrl_f){
		old_temp_ctrl_f = SysState.temp_ctrl_f;
		if(SysState.temp_ctrl_f){
			SysCnt.temp_update = SysState.t_updt_time;
			SysCnt.temp_ctrl = SysState.t_ctrl_time;			
		}
	}
	
	
    //----------------      DISPLAY        --------------------------------------
  if (SysState.error_code) {
		if (SysState.ss_update_f) {
			SS_PRINT_ERROR_CODE(SysState.error_code);
			SysState.ss_update_f = 0;
		}
	} else {
		if(SysState.temp_ctrl_f){
			if (SysState.ss_update_f) {
				SysState.ss_update_f = 0;
				if(SYS_DBG_PRINT_F){
					dbg_print("SS update\r\n");
				}
				SS_PRINT_TEMP(DS18B20_TEMP);		
			}	
		}				
		else{
			SevSeg_Idle();
		}
	}	
    //----------------      TEMP UPDATE     --------------------------------------


	if (SysState.error_code==0) {
		if (SysCnt.temp_update >= SysState.t_updt_time){				
			if(SYS_DBG_PRINT_F){
				dbg_print("TEMP update\r\n");
			}
			SysCnt.temp_update = 0;
			SysState.ss_update_f = 1;				
			//LED_TOGGLE(LED_BLUE);
			SysState.error_code |= ds18b20_GetTemp(0) << TEMP_SENSOR_READING_ERROR; 
			//LED_TOGGLE(LED_BLUE);
			
			
			      
			//----------------      PUMP & BURNER CONTROL     --------------------------------------
     
			if((SysState.burner==1) &( SysState.pump==0)){
				PUMP(ON);

			}
			if(SysState.temp_ctrl_f==0){
				BURNER(OFF);	
			}
			if(!SysState.temp_ctrl_f & SysState.pump & (DS18B20_TEMP<SysState.threshold_temp)){
				PUMP(OFF);
			}
			if(DS18B20_TEMP>SysState.threshold_temp){
				PUMP(ON);
			}
		}
	}

      //----------------      TEMP CONTROL     --------------------------------------
      

	if (SysState.error_code==0) {
		if (SysState.temp_ctrl_f){
			if (SysCnt.temp_ctrl >= SysState.t_ctrl_time) {
				SysState.ss_update_f = 1;
				SysCnt.temp_ctrl = 0;			
				if(SYS_DBG_PRINT_F){
					dbg_print("TEMP control\r\n");							
				}
				if ((DS18B20_TEMP < SysState.set_temp - SysState.t_hyst) & (SysState.burner == 0)) {	
					if(SYS_DBG_PRINT_F){
						dbg_print("BURNER: ON\r\n");
					}
					BURNER(ON);
				}
				if ((DS18B20_TEMP > SysState.set_temp + SysState.t_hyst) & (SysState.burner == 1)) {
					if(SYS_DBG_PRINT_F){
						dbg_print("BURNER: OFF\r\n");
					}
					BURNER(OFF);			
				}	
			}  
		}
	}

	

			//---------------------------   UART --------------------------------
//			if(SysState.uart_en_f){
//			
//				if (uart1_rx_flag) {			
//					parseCommand(uart_rx_buf);			
//					uart1_rx_flag = 0;		
//				}
//			}

			//---------------------------   CDC --------------------------------
			
			if(HAL_GPIO_ReadPin(USB_INT_GPIO_Port, USB_INT_Pin) && (cdc_init==0)){
				HAL_GPIO_WritePin(USB_PULLUP_GPIO_Port, USB_PULLUP_Pin, GPIO_PIN_SET);
				cdc_init=1;
				
			}				
			if (vcp_rx_flag) {
				if(cdc_init==1){
					uartInit();					
					cdc_init=2;
				}else{
					print_to("\r\n");	
					parseCommand(vcp_rx_buf);						
				}
				vcp_rx_flag = 0;
			}					
			
			
			
//---------------------------   SX1278 --------------------------------
			char answ[64];
//			if (SysCnt.wl_offline >= 54000){ //45 min
//				SysState.reset_cnt++;
//				//SysCnt.wl_offline = 0;
//				SysVarRW(WR,&SV[vn_RESET_CNT]);
//				sprintf(answ, "RESET_CNT: %d\r\n", SysState.reset_cnt);		
//				print_to(answ);		
//				HAL_NVIC_SystemReset();	
//			}
			WL_Handler();	
			
//---------------------------   ERROR check --------------------------------
			if (SysState.error_code) {
				
				SysErrorCheck();
				SysState.ss_update_f=1;
			}
}


		
//========================     SYS TASK END    =====================
		
		

//---------------------------   ERROR check --------------------------------
void SysErrorCheck(void){
	if (SysCnt.error_check > ERROR_CHECK_TIME) {
		SysCnt.error_check = 0; 
		
		//Check temp sensor
		if (SysState.error_code & (1<<TEMP_SENSOR_READING_ERROR)) {
			if (ow_Reset())
				SysState.error_code |= (1<<TEMP_SENSOR_READING_ERROR);
			else {
				SysState.error_code &= ~(1<<TEMP_SENSOR_READING_ERROR);
				ds18b20_GetTemp(0);
			}
		}
		
		//Check io_expander
		if (SysState.error_code & (1<<IO_EXPANDER_READING_ERROR)) {
			if (PCF8574_IsDeviceReady())
				SysState.error_code |= (1<<IO_EXPANDER_READING_ERROR);
			else {
				SysState.error_code &= ~(1<<IO_EXPANDER_READING_ERROR);				
			}
		}		
		
		//Check eeprom
		if (SysState.error_code & (1<<EEPROM_READING_ERROR)) {
			if (EEPROM_IsDeviceReady())
				SysState.error_code |= (1<<EEPROM_READING_ERROR);
			else {
				SysState.error_code &= ~(1<<EEPROM_READING_ERROR);				
			}
		}
		if(SysState.error_code){				
			LED_ON(LED_RED);			
		}else{
			LED_OFF(LED_RED);	
		}
	}
}
// --------------- END ERROR -------------------------------		


		
		
 void SetTemp(void) {	
	#define SET_MODE_TIME 120
	int16_t tmp = SysState.set_temp;
	SS_PRINT_TEMP(tmp);
	SS_BLINK(ON);
	SysCnt.timeout = 0;
	while (1) {
		buttons_handler();
		if(BTN_DOWN==SHORT_PRESS){
			BTN_DOWN=0;
			if (tmp > TEMP_MIN_LIMIT) {
				tmp --;
				SS_PRINT_TEMP(tmp);
			}
			SysCnt.timeout = 0;			
			
		}
		if(BTN_UP==SHORT_PRESS){
			BTN_UP=0;
			if (tmp < TEMP_MAX_LIMIT) {
				tmp ++;
				SS_PRINT_TEMP(tmp);
			}
			SysCnt.timeout = 0;					
		}
		if(BTN_SEL==SHORT_PRESS){
			BTN_SEL=0;
			SysState.set_temp = tmp;
			SS_BLINK(OFF);
			trulala();
			SysVarRW(WR,&SV[vn_T_SET]);                   
			return;		
		}    
     
    if (SysCnt.timeout > SET_MODE_TIME) {
			SysState.set_temp = tmp;
			SS_BLINK(OFF);
			SysVarRW(WR,&SV[vn_T_SET]); 
			if(SysState.error_code){
				SS_PRINT_ERROR_CODE(SysState.error_code);
			}else{
				 SS_PRINT_TEMP(DS18B20_TEMP);
			}
      return;
     }
	 }   
}
 


 
 
uint32_t SysVarRW(_Bool rw, SysVar* sv) {
	if (rw){
			return EEPROM_Write(sv->mem_addr*EEPROM_OFFSET, 2, (uint8_t*)sv->pVal);		
	} else {
			return EEPROM_Read(sv->mem_addr*EEPROM_OFFSET, 2, (uint8_t*)sv->pVal);
	}	
}

 uint32_t EEPROM_restore(void) {
	int len, res=0;
	
	len=EEPROM_DUMP_SIZE/8;
	 
	for( int i = 0; i<=len; i++){
		
		res|=EEPROM_Write(i*8, 8, eeprom_dump+i*8);
		HAL_Delay(100);
	}
	return res;
		
}
