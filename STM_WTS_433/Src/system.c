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
//{"t_ctrl", 			0, 1, 		&SysState.temp_ctrl_f, 				WE, vn_T_CTRL_F}, 
//{"t_set", 			0, 50, 		&SysState.set_temp, 					WE, vn_T_SET},		
//{"t_ctrl_time", 0, 5000, 	&SysState.t_ctrl_time, 				WE, vn_T_CTRL_TIME},
{"t_updt_time", 0, 5000, 	&SysState.t_updt_time, 				WE, vn_T_UPDT_TIME}
//{"t_hyst", 			0, 10, 		&SysState.t_hyst, 						WE, vn_T_HYST},
//{"pump", 				0, 1, 		&SysState.pump, 							WE, vn_PUMP},
//{"burner", 			0, 1, 		&SysState.burner, 						RO, vn_BURNER},
//{"t_th", 				0, 30, 		&SysState.threshold_temp, 		WE, vn_T_THRESHOLD}
////{"drv_pos", 		0, 999, 	&drv_m1.position, 						WE, vn_DRIVE_POS*EEPROM_OFFSET},//EEPROM , when set drv_m1.position = drv_m1.dest_pos
////{"drv_pos_max", 0, 999, 	&drv_m1.max_pos, 							WE, vn_DRIVE_MAX_POS * EEPROM_OFFSET},//EEPROM 
////{"drv_pos_dest",0, 0, 		&drv_m1.dest_pos, 						WE, 0},
////{"drv_steps", 	0, 100, 		&drv_m1.steps, 								WE, vn_DRIVE_STEPS * EEPROM_OFFSET}//EEPROM - DRIVE_STEPS
//{"t_hyst", 			0, 10, 		&SysState.t_hyst, 						WE, vn_T_HYST * EEPROM_OFFSET}//EEPROM - TEMP_HYST
////{"all", 0, 0, NULL, 0},
};



//========================     SYS TASK    =====================

void SysInit(void) {
	
	
  SysState.error_code =0;
	SysState.error_code |= EEPROM_IsDeviceReady()<< EEPROM_READING_ERROR; 
  SysState.error_code |= ds18b20_Init(1, RESOLUTION_9BIT) << TEMP_SENSOR_READING_ERROR;  
	SysState.error_code |= WL_Init()<< WL_ERROR;
	
	ds18b20_Init(1,RESOLUTION_9BIT);
  ds18b20_GetTemp(0);
		
//	SysVarRW(RD,&SV[vn_T_CTRL_F]);	
//	SysVarRW(RD,&SV[vn_T_SET]);	
//	SysVarRW(RD,&SV[vn_T_CTRL_TIME]);
	SysVarRW(RD,&SV[vn_T_UPDT_TIME]);
//	SysVarRW(RD,&SV[vn_T_HYST]);

//	SysVarRW(RD,&SV[vn_T_THRESHOLD]);

	

  //Start timer1 IT
  HAL_TIM_Base_Start_IT( &htim1);

  //Start timer3 IT DRIVE ENCODER
  //HAL_TIM_Encoder_Start( &htim3, TIM_CHANNEL_ALL);
  //HAL_TIM_Base_Start_IT( &htim3);
	
  //Arm UART1
  HAL_UART_Receive_IT( & huart1, & receive_val, 1);


//	SysState.ss_update_f = 1;
//	SysCnt.temp_ctrl = SysState.t_ctrl_time;

			
//	PUMP(OFF);
//	BURNER(OFF);			

	//if eeprom need to restore
	while(HAL_UART_Transmit(&huart1, "C" , 1, 0xFFFF)!=HAL_OK);
	HAL_Delay(100);	
	if (uart_rx_buf[0]=='C') {
		if (EEPROM_Write(0, strlen((char*)eeprom_dump), eeprom_dump)) {						
			print_to("EEPROM RESTORE: FAIL\r\n");
			LED_ON(LED_BLUE);			
		}else{
			print_to("EEPROM RESTORE: DONE\r\n");
			LED_ON(LED_RED);
		}			
		HAL_Delay(1000);
		LED_OFF(LED_BLUE);
		LED_OFF(LED_RED);
	}
//end restoring
	
}

void SystemTask(void) {



    //----------------      TEMP UPDATE     --------------------------------------

    if (!SysState.error_f) {

      if (SysCnt.temp_update >= SysState.t_updt_time){
				
				if(SYS_DBG_PRINT_F){
					dbg_print("TEMP update\r\n");
				}
        SysCnt.temp_update = 0;        
				
        LED_TOGGLE(LED_BLUE); 				


		SysState.error_code |= ds18b20_GetTemp(0) << TEMP_SENSOR_READING_ERROR;  
					
		LED_TOGGLE(LED_BLUE); 			
					
        }
      }

     

			//---------------------------   UART --------------------------------
//			if(SysState.uart_en_f){
//			
				if (uart1_rx_flag) {			
					parseCommand(uart_rx_buf);			
					uart1_rx_flag = 0;		
				}
//			}

			//---------------------------   CDC --------------------------------
			
			if(HAL_GPIO_ReadPin(USB_INT_GPIO_Port, USB_INT_Pin) & (cdc_init==0)){
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
		SysState.error_f = 1;
		LED_ON(LED_RED);
	} else {
		SysState.error_f = 0;
		LED_OFF(LED_RED);
	}
	
	if(SysState.error_f){
		SysErrorCheck();
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
		
			
		
		//Check eeprom
		if (SysState.error_code & (1<<EEPROM_READING_ERROR)) {
			if (EEPROM_IsDeviceReady())
				SysState.error_code |= (1<<EEPROM_READING_ERROR);
			else {
				SysState.error_code &= ~(1<<EEPROM_READING_ERROR);				
			}
		}
	}
}
// --------------- END ERROR -------------------------------		


 
uint32_t SysVarRW(_Bool rw, SysVar* sv) {
 
if (rw){
		return EEPROM_Write(sv->mem_addr*EEPROM_OFFSET, 2, (uint8_t*)sv->pVal);		
	} else {
		return EEPROM_Read(sv->mem_addr*EEPROM_OFFSET, 2, (uint8_t*)sv->pVal);
	}	
}

 
