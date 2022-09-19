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
{"temp",		 		0, 0, 										(int16_t*)(&DS18B20_TEMP), 		RO, 0},
{"t_ctrl", 			0, 1, 										&SysState.temp_ctrl_f, 				WE, vn_T_CTRL_F}, 
{"t_set", 			0, TEMP_MAX_LIMIT, 				&SysState.set_temp, 					WE, vn_T_SET},		
{"t_ctrl_time", 0, 5000, 									&SysState.t_ctrl_time, 				WE, vn_T_CTRL_TIME},
{"t_updt_time", 0, 5000, 									&SysState.t_updt_time, 				WE, vn_T_UPDT_TIME},
{"t_hyst", 			0, 10, 										&SysState.t_hyst, 						WE, vn_T_HYST},
{"pump", 				0, 0x10FF,								&SysState.pump, 							WE, vn_PUMP},
{"drv_pos", 		0, DRIVE_MAX_POS_LIMIT, 	&drv_m1.pos, 									RO, vn_DRIVE_POS},
{"drv_pos_max", 0, DRIVE_MAX_POS_LIMIT, 	&drv_m1.max_pos, 							WE, vn_DRIVE_MAX_POS},
{"drv_pos_dest",0, DRIVE_MAX_POS_LIMIT, 	&drv_m1.dest_pos, 						WE, vn_DRIVE_DRIVE_POS_DEST},
{"drv_steps", 	0, DRIVE_MAX_POS_LIMIT, 	&drv_m1.steps, 								WE, vn_DRIVE_STEPS }

////{"all", 0, 0, NULL, 0},
};



//========================     SYS TASK    =====================

void SysInit(void) {
	
	WF_PUMP_OFF();
	SevSeg_Init();
	
	//if eeprom need to restore
	PCF8574_ReadReg();
	if((~PCF8574_reg) & 0x04){ //Button SEL pressed		
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
		if((~PCF8574_reg) & 0x04){
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
//end restoring
	
	
	SysState.error_code =0;
	SysState.error_code |= EEPROM_IsDeviceReady()<< EEPROM_READING_ERROR;
	SysState.error_code |= PCF8574_IsDeviceReady()<< IO_EXPANDER_READING_ERROR;  
	SysState.error_code |= ds18b20_Init(1, RESOLUTION_9BIT) << TEMP_SENSOR_READING_ERROR;  
	SysState.error_code |= Drive_CheckRotation(1) << DRV_CLOSE_ERROR;
	HAL_Delay(500);
	SysState.error_code |= Drive_CheckRotation(0) << DRV_OPEN_ERROR;
		
	ds18b20_GetTemp(0);
	
	SysVarRW(RD,&SV[vn_T_CTRL_F]);	
	SysVarRW(RD,&SV[vn_T_SET]);	
	SysVarRW(RD,&SV[vn_T_CTRL_TIME]);
	SysVarRW(RD,&SV[vn_T_UPDT_TIME]);
	SysVarRW(RD,&SV[vn_T_HYST]);
	SysVarRW(RD,&SV[vn_PUMP]);
	
	
	//SysVarRW(RD,&SV[vn_DRIVE_POS]);
	//SysVarRW(RD,&SV[vn_DRIVE_MAX_POS]);
	//SysVarRW(RD,&SV[vn_DRIVE_DRIVE_POS_DEST]);
	//SysVarRW(RD,&SV[vn_DRIVE_STEPS]);
	
	
	drv_m1.pos=0;
	drv_m1.dest_pos=0;
	drv_m1.steps=DRIVE_STEPS;
	drv_m1.max_pos=DRIVE_MAX_POS_LIMIT;
	
	MCP23S17_Init();	
	
	
	MCP23S17_PortSet(MCP_PORTA, 0xFF);
	MCP23S17_PortConfig(MCP_PORTA, 0x00);
	
	MCP23S17_PortConfig(MCP_PORTB, 0xFF);
 	MCP23S17_IRQen(MCP_PORTB, 0xFF);
	MCP23S17_PortPUP(MCP_PORTB, 0xFF);
	MCP23S17_PortGet(MCP_PORTB);


	pump_set((uint8_t)SysState.pump);

	//Start timer1 IT
	HAL_TIM_Base_Start_IT( &htim1);

	//Start timer3 IT DRIVE ENCODER
	//HAL_TIM_Encoder_Start( &htim3, TIM_CHANNEL_ALL);
	//HAL_TIM_Base_Start_IT( &htim3);

	//Arm UART1
//	HAL_UART_Receive_IT( & huart1, & receive_val, 1);
	
	Buttons_Init();	
	
	LED_OFF(LED_BLUE);
	LED_OFF(LED_RED);	
	
	

	SysState.ss_update_f = 1;
	
	if((SysState.error_code & (1<<DRV_OPEN_ERROR)) || (SysState.error_code & (1<<DRV_CLOSE_ERROR))){
		if(SYS_DBG_PRINT_F){
			dbg_print("DRIVE ROTATION ERROR\r\n");
		}
	}else{
		SysState.error_code |= Drive_HomeInit() << DRV_HOME_INIT_ERROR;			

	}	
	
	
	
	ds18b20_GetTemp(0);
	
	SysCnt.temp_ctrl = SysState.t_ctrl_time;
			
		
	
	WL_Init();
	
	

	
	if(SYS_DBG_PRINT_F){
		dbg_print("===== INIT DONE ======\r\n");
	}
}

void SystemTask(void) {

	//-----------------------------------------------------------------------------------    
	//----------------      MAIN PUMP              --------------------------------------
	//----------------------------------------------------------------------------------- 
	static int16_t old_temp_ctrl_f;
	
	 //Reset sys counters if temp_ctrl_f 0->1
	if(old_temp_ctrl_f != SysState.temp_ctrl_f){
		old_temp_ctrl_f = SysState.temp_ctrl_f;
		if(SysState.temp_ctrl_f){
			SysCnt.temp_update = SysState.t_updt_time;
			SysCnt.temp_ctrl = SysState.t_ctrl_time;	
			WF_PUMP_ON();
		}else{
			WF_PUMP_OFF();
		}
	}
	
		
		
	//-----------------------------------------------------------------------------------    
	//----------------      DISPLAY UPDATE         --------------------------------------
	//-----------------------------------------------------------------------------------  
    

	
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
	
	//-----------------------------------------------------------------------------------
	//----------------      TEMP UPDATE            --------------------------------------
	//-----------------------------------------------------------------------------------

    
	if (!SysState.error_code) {
		if (SysCnt.temp_update > SysState.t_updt_time){			
			if(SYS_DBG_PRINT_F){
				dbg_print("TEMP update\r\n");
			}
			SysCnt.temp_update = 0;			
			SysState.ss_update_f = 1;			
			
			//LED_TOGGLE(LED_BLUE); 	

			if (ds18b20_GetTemp(0)) {				
				SysState.error_code |= 1<<TEMP_SENSOR_READING_ERROR;
			}	
			//LED_TOGGLE(LED_BLUE);        
		}      
	}
	//-----------------------------------------------------------------------------------
	//----------------      TEMP CONTROL           --------------------------------------
	//-----------------------------------------------------------------------------------
	 
	if (!SysState.error_code){
		if (SysState.temp_ctrl_f){
			if (SysCnt.temp_ctrl >= SysState.t_ctrl_time) {
				SysCnt.temp_ctrl = 0;
				if(SYS_DBG_PRINT_F){
					dbg_print("TEMP CONTROL\r\n");
				}
				if (!drv_m1.run_f) {
					//OPEN
					
					//if ((DS18B20_TEMP < SysState.set_temp + SysState.t_hyst) & (DRV_POS < drv_m1.max_pos)) {	
					if ((DS18B20_TEMP < SysState.set_temp + SysState.t_hyst)) {		
						if (DRV_POS +  drv_m1.steps < drv_m1.max_pos) {
							drv_m1.dest_pos +=  drv_m1.steps;
						} else {
							drv_m1.dest_pos = drv_m1.max_pos;
						}
						if(SYS_DBG_PRINT_F){
							dbg_print("OPEN\r\n");	
							sprintf(dbg_str,"DEST POS = %d\r\n", drv_m1.dest_pos);
							dbg_print(dbg_str);
						}
					}
					
					//CLOSE
					//if ((DS18B20_TEMP > SysState.set_temp - SysState.t_hyst) & (DRV_POS > DRIVE_MIN_POS_LIMIT)) {		
					if ((DS18B20_TEMP > SysState.set_temp - SysState.t_hyst)) {						
						if (DRV_POS > drv_m1.steps) {
							drv_m1.dest_pos -= drv_m1.steps;
						} else {
							drv_m1.dest_pos = 0;
						}
						if(SYS_DBG_PRINT_F){
							dbg_print("CLOSE\r\n");	
							sprintf(dbg_str,"DEST POS = %d\r\n", drv_m1.dest_pos);
							dbg_print(dbg_str);
						}
					}
				}
			}
		}
	}
	//-----------------------------------------------------------------------------------
	//----------------      MOTO CONTROL           --------------------------------------
	//-----------------------------------------------------------------------------------

	if (!drv_m1.fail_f) {				
		if(!drv_m1.run_f){        
			if (drv_m1.dest_pos > DRV_POS) {
				if(SYS_DBG_PRINT_F){						
					dbg_print("DRV MOVE OPEN\r\n");
				}
				DRV_OPEN();
			} else if (drv_m1.dest_pos < DRV_POS) {
				if(SYS_DBG_PRINT_F){
					dbg_print("DRV MOVE CLOSE\r\n");
				}
				DRV_CLOSE();
			}
		}
	}
	
	//-----------------------------------------------------------------------------------
	//----------------      PUMP CONTROL           --------------------------------------
	//-----------------------------------------------------------------------------------


	//SysState.pump 0x XX X X
	//command to   sens---|	|--set
	if(SysState.pump>>8){		
		
		SysState.pump&=0x00FF; //reset flag (last bit)
		pump_set((uint8_t)SysState.pump&0x000F);
		SysVarRW(WR,&SV[vn_PUMP]);
	}	
	if(!HAL_GPIO_ReadPin(MCP_IRQ_GPIO_Port, MCP_IRQ_Pin)){
//		volatile uint8_t pb = MCP23S17_PortGet(MCP_PORTB);
//		pb = pb & 0x0F;
//		pb = pb<<4;
//		pb |= SysState.pump;
		
		
		
		SysState.pump&= 0x0F; //Reset sens bits
		SysState.pump|=(MCP23S17_PortGet(MCP_PORTB)&0x0F)<<4;	
		
	}
			
				
//			if((SysState.burner==0) & (SysState.pump1==1) & DS18B20_TEMP<20){
//			}

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
			
	WL_Handler();	
		
//---------------------------   ERROR check --------------------------------
	if (SysState.error_code) {
		//SysState.error_f = 1;
		
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
//		 if(BTN_SEL==SHORT_PRESS){
//				BTN_SEL=0;
//				SysState.set_temp = tmp;
//				SS_BLINK(OFF);
//				trulala();
//				SysVarRW(WR,&SV[vn_T_SET]);  
//				SysCnt.temp_update = 0;
//				SysCnt.temp_ctrl = 0;
//				SysState.ss_update_f=1;
//				return;				
//			
//		 }     
     
     if ( (SysCnt.timeout > SET_MODE_TIME) || (BTN_SEL==SHORT_PRESS)){
			 BTN_SEL=0;
			 SysState.set_temp = tmp;
			 SS_BLINK(OFF);
			 SysVarRW(WR,&SV[vn_T_SET]); 	
			 trulala();
			 if(SysState.error_code){
				 SS_PRINT_ERROR_CODE(SysState.error_code);
			 }else{					
				 SS_PRINT_TEMP(DS18B20_TEMP);
			 }
			 SysCnt.temp_update = 0;
			 SysCnt.temp_ctrl = 0;			 
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

 void pump_set(uint8_t channels) {
	if(RELE_ON_LEVEL==0){ 
		channels=~channels; 
	}	
//	HAL_GPIO_WritePin(RELE_OE_PORT, RELE_OE_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(RELE_LA_PORT, RELE_LA_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(RELE_RESET_PORT, RELE_RESET_PIN, GPIO_PIN_RESET);
//	HAL_Delay(1);
//	HAL_GPIO_WritePin(RELE_RESET_PORT, RELE_RESET_PIN, GPIO_PIN_SET);
//	HAL_SPI_Transmit(RELE_SPI_PORT,&channels, 1, 1000);
//	HAL_GPIO_WritePin(RELE_LA_PORT, RELE_LA_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(RELE_LA_PORT, RELE_LA_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(RELE_OE_PORT, RELE_OE_PIN, GPIO_PIN_RESET);
	

	MCP23S17_PortSet(MCP_PORTA, channels);
//	volatile uint8_t tx_data[24];
//	volatile uint8_t 	rx_data[24];
//	tx_data[0]=	MCP23S17_ADDRESS | 0x01;
//	tx_data[1]=	0;
//	 
//	MCP23S17_NSS_RESET;		
//	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, tx_data, rx_data, 22, 1000);
//	MCP23S17_NSS_SET;

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
 