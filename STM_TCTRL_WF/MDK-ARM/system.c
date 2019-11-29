#include "system.h"


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
volatile SysState_t SysState;
volatile SysCouners_t SysCnt;
volatile _Bool btn_pressed_f;

//enum VarName{
//	vn_T_CTRL_F=1,
//	vn_T_SET,
//	vn_T_CTRL_TIME,
//	vn_T_UPDT_TIME,
//	vn_T_HYST,
//	vn_DRIVE_POS,
//	vn_DRIVE_STEPS,
//	vn_DRIVE_MAX_POS
//};

// SysVar SV[1]= {
//{"temp",		 		0, 0, 		(int16_t*)(&DS_TEMP), 				RO, 0},
//{"t_ctrl", 			0, 1, 		&SysState.temp_ctrl_f, 				WE, vn_T_CTRL_F * EEPROM_OFFSET}, //EEPROM - TEMP_CTRL_F
//{"t_set", 			0, 50, 		&SysState.set_temp, 					WE, vn_T_SET * EEPROM_OFFSET},		//EEPROM - TEMP_SET
//{"t_ctrl_time", 0, 5000, 	&SysState.t_ctrl_time, 				WE, vn_T_CTRL_TIME * EEPROM_OFFSET},//EEPROM - TEMP_CTRL_TIME
//{"t_updt_time", 0, 5000, 	&SysState.t_updt_time, 				WE, vn_T_UPDT_TIME * EEPROM_OFFSET},//EEPROM - TEMP_UPDT_TIME
//{"t_hyst", 			0, 10, 		&SysState.t_hyst, 						WE, vn_T_HYST * EEPROM_OFFSET},//EEPROM - TEMP_HYST

//{"drv_pos", 		0, 999, 	&drv_m1.position, 						WE, vn_DRIVE_POS*EEPROM_OFFSET},//EEPROM , when set drv_m1.position = drv_m1.dest_pos
//{"drv_pos_max", 0, 999, 	&drv_m1.max_pos, 							WE, vn_DRIVE_MAX_POS * EEPROM_OFFSET},//EEPROM 
//{"drv_pos_dest",0, 0, 		&drv_m1.dest_pos, 						WE, 0},
//{"drv_steps", 	0, 100, 		&drv_m1.steps, 								WE, vn_DRIVE_STEPS * EEPROM_OFFSET}//EEPROM - DRIVE_STEPS
//    //{"all", 0, 0, NULL, 0},
//};



//__STATIC_INLINE void DelayMicro(__IO uint32_t micros) {

//  micros *= (SystemCoreClock / 1000000) / 9;
//  /* Wait till done */
//  while (micros--);

//}

//========================     SYS TASK    =====================

void SysInit(void) {
	LED_GREEN_Toggle();
  HAL_StatusTypeDef res = HAL_OK;
	
	//---------------------Sys State-------------------------------
	
  SysState.error_code = 0;
  SysState.error_f = 0;
	
  SysState.set_temp = 25; //EEPROM
#warning "SET TEMP NOT STORED IN EEPROM"
  SysState.ss_blink_f = 0;
  SysState.ss_update_f = 0;
	
	SysState.t_ctrl_time=2000;
	SysState.t_updt_time=1000;
	SysState.t_hyst=2;
 
	SysState.uart_dbg_f=0;
	
	SysState.nrf24_en_f=0;
//if counters not stored in eeprom
	
	
 
	
	//---------------------Sys Counters-------------------------------
  SysCnt.blink = 0;
  SysCnt.error_check = 0;
  SysCnt.temp_ctrl = 0;
  SysCnt.temp_update = 0;
  SysCnt.timeout = 0;
  SysCnt.drv_move = 0;
  
	drv_m1.fail_f = 0;
  drv_m1.run_f = 0;
	
	drv_m1.max_pos=20;
	drv_m1.steps=2;
	
SysState.set_temp=35;
	
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	SevSegInit();
	
  res = HAL_I2C_IsDeviceReady( &hi2c2, EEPROM_ADDR, 3, HAL_MAX_DELAY);
  if (res != HAL_OK) {
    SysState.error_code |= EEPROM_READING_ERROR;
    SysState.error_f = 1;
    printf("EEPROM READ ERROR!\r\n");
    printf("I2C STATUS CODE: %d\r\n", res);
    printf("I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( &hi2c2));

  }else{
		printf("EEPROM OK\r\n");
	}
	
  res = HAL_I2C_IsDeviceReady( &hi2c2, PCF8574A_ADDR, 3, HAL_MAX_DELAY);
  if (res != HAL_OK) {
    SysState.error_code |= IO_EXPANDER_READING_ERROR;
    SysState.error_f = 1;
    printf("PCF8574A READ ERROR!\r\n");
    printf("I2C STATUS CODE: %d\r\n", res);
    printf("I2C ERROR CODE: %d\r\n", HAL_I2C_GetError( &hi2c2));

  }else{
		printf("PCF8574A OK\r\n");
	}
	Buttons_Reset();

		
//	SysVarRW(RD,&SV[vn_T_CTRL_F]);
//	
//	SysVarRW(RD,&SV[vn_T_SET]);
//	
//	SysVarRW(RD,&SV[vn_T_CTRL_TIME]);
//	
//	SysVarRW(RD,&SV[vn_T_UPDT_TIME]);
//	
//	SysVarRW(RD,&SV[vn_T_HYST]);
//	
//	SysVarRW(RD,&SV[vn_DRIVE_POS]);
//	
//	SysVarRW(RD,&SV[vn_DRIVE_MAX_POS]);
//	
//	SysVarRW(RD,&SV[vn_DRIVE_STEPS]);
	
	
	drv_m1.start_pos=0;


  if (ds18b20_Init(RESOLUTION_9BIT)) {
    SysState.error_code |= TEMP_SENSOR_READING_ERROR;
    SysState.error_f = 1;
		SS_PRINT_T_NONE();
    printf("TEMP SENSOR ERROR\r\n");
  }else{
		ds18b20_GetTemp();
		SS_PRINT_TEMP(DS_TEMP);
		printf("TEMP SENSOR OK\r\n");			
		sprintf(sprintf_str, "Temp = %d\r\n", DS_TEMP);		
		printf(sprintf_str);	
	}
	//printf("stage 4\r\n");
  
  

  //Start timer1 IT
  HAL_TIM_Base_Start_IT( &htim1);//system tymer
	
	//Start timer 3 
	//TIM3->CNT - motor encoder counter (position)
	TIM3->CNT=0;
	HAL_TIM_Base_Start_IT( &htim3);
	
	
	Drive_HomeInit();
	
	
 // HAL_Delay(1000);	
	
//  if (Drive_CheckRotation()) {
//    SysState.error_code |= DRV_OPEN_ERROR;
//    SysState.error_f = 1;
//    printf("DRIVE OPEN ERROR!\r\n");
//  }
	
	

	
//  DRV_CLOSE();
//  if (Drive_CheckRotation()) {
//    SysState.error_code |= DRV_CLOSE_ERROR;
//    SysState.error_f = 1;
//    printf("DRIVE CLOSE ERROR!\r\n");
//  }
//  DRV_STOP();
//  HAL_Delay(500);
	
  //Arm UART1
//  HAL_UART_Receive_IT( & huart1, & receive_val, 1);

  //HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);

	SysState.ss_update_f = 1;
	LED_GREEN_Toggle();
	
}

void SystemTask(void) {
    // unsigned int i;

//    //----------------      DISPLAY        --------------------------------------
//    if (SysState.ss_update_f) {
//      SysState.ss_update_f = 0;
//			//printf("SS update\r\n");
//      if (SysState.error_f) {
//        SS_PRINT_ERROR_CODE(SysState.error_code);
//      } else {
//        if (SysState.view_mode_f) {
//          SS_PRINT_CUR_POS();
//        } else {
//          if (SysState.temp_ctrl_f) {
//            SS_PRINT_TEMP(DS_TEMP);
//          } else {

//            SS_PRINT_T_NONE();
//          }

//        }
//      }
//    }

    //----------------      TEMP UPDATE     --------------------------------------

    if (!SysState.error_f) {

      if (SysCnt.temp_update >= SysState.t_updt_time){
					printf("TEMP update\r\n");
          SysCnt.temp_update = 0;
          SysState.ss_update_f = 1;
				
          LED_GREEN_Toggle(); 				


          if (ds18b20_GetTemp()) {
            SysState.error_f = 1;
            SysState.error_code |= TEMP_SENSOR_READING_ERROR;
          }	else {
            SysState.error_code &= ~TEMP_SENSOR_READING_ERROR;
						SS_PRINT_TEMP(DS_TEMP);
						sprintf(sprintf_str, "Temp = %d\r\n", DS_TEMP);		
						printf(sprintf_str);	
          }
					
        }
      }

      //----------------      TEMP CONTROL     --------------------------------------
      if (!SysState.error_f) {

          if (SysCnt.temp_ctrl >= SysState.t_ctrl_time) {            
            SysCnt.temp_ctrl = 0;
						printf("TEMP CONTROL\r\n");
            if (!drv_m1.run_f) {
							//OPEN
							
              if ((DS_TEMP < SysState.set_temp + SysState.t_hyst) & (DRV_POS < drv_m1.max_pos)) {	
								printf("OPEN\r\n");	
                if (DRV_POS +  drv_m1.steps < drv_m1.max_pos) {
                  drv_m1.dest_pos +=  drv_m1.steps;
                } else {
                  drv_m1.dest_pos = drv_m1.max_pos;
                }
              }
							//CLOSE
							
              if ((DS_TEMP > SysState.set_temp - SysState.t_hyst) & (DRV_POS > 0)) {
								printf("CLOSE\r\n");
                if (DRV_POS > drv_m1.steps) {
                  drv_m1.dest_pos -= drv_m1.steps;
                } else {
                  drv_m1.dest_pos = 0;
                }
              }
            }
          }

      }
      //----------------      MOTO CONTROL     --------------------------------------
					
		
       if (!SysState.error_f) {
				if(!drv_m1.run_f){
        if (drv_m1.dest_pos > DRV_POS) {				
					printf("DRV OPEN\r\n");
          DRV_OPEN();

        } else if (drv_m1.dest_pos < DRV_POS) {
					printf("DRV CLOSE\r\n");
          DRV_CLOSE();
        }
			}
//				if(drv_m1.end_sens_f){
//					Drive_HomeInit();				
//				}
      }
			
			

      //---------------------------   ERROR routine --------------------------------

      if (SysCnt.error_check > ERROR_CHECK_TIME) {
        SysCnt.error_check = 0;

				#warning "Check rotation not set"
//        //Check moto for rotation
//        if ((SysState.error_code & DRV_CLOSE_ERROR) | (SysState.error_code & DRV_OPEN_ERROR)) {

//          DRV_OPEN();
//          if (Drive_CheckRotation()) {
//            SysState.error_code |= DRV_CLOSE_ERROR;
//          } else
//            SysState.error_code &= ~DRV_CLOSE_ERROR;
//          DRV_STOP();
//          HAL_Delay(1000);
//          DRV_CLOSE();
//          if (Drive_CheckRotation()) {
//            SysState.error_code |= DRV_OPEN_ERROR;
//          } else
//            SysState.error_code &= ~DRV_OPEN_ERROR;
//          DRV_STOP();
//        }
				

        //Check temp sensor
        if (SysState.error_code & TEMP_SENSOR_READING_ERROR) {
          if (ds18b20_Reset()){
            SysState.error_code |= TEMP_SENSOR_READING_ERROR;
					}else {
            SysState.error_code &= ~TEMP_SENSOR_READING_ERROR;
            ds18b20_GetTemp();
						HAL_Delay(10);
						DS_TEMP = ds18b20_GetTemp();
						
          }
        }
				
				
        //Check io_expander
        if (HAL_I2C_IsDeviceReady( & hi2c2, PCF8574A_ADDR, 3, HAL_MAX_DELAY)) {
          SysState.error_code |= IO_EXPANDER_READING_ERROR;  					
        } else {
          SysState.error_code &= ~IO_EXPANDER_READING_ERROR;
        }		
				if(SysState.error_code){
					SS_PRINT_ERROR_CODE(SysState.error_code);
				}else{
					SS_PRINT_TEMP(DS_TEMP);
				}
      }

			
      if (SysState.error_code) {
        SysState.error_f = 1;
        

      } else {
        SysState.error_f = 0;
      }
			// --------------- END ERROR routine -------------------------------
			//---------------------------   UART --------------------------------
			if(SysState.uart_en_f){
			
				if (uart1_rx_flag) {			
					parseCommand(uart_rx_buf);			
					uart1_rx_flag = 0;		
				}
			}
//			if(SysState.uart_dbg_f){
//			
//				UART_debug_print();
//			}
			//---------------------------   NRF24 --------------------------------
			if(SysState.nrf24_en_f){
				WL_Handler();				
//				
			}

}
//========================     SYS TASK END    =====================
		
	
		
  void SetTemp(void) {

   int16_t tmp = SysState.set_temp;
   SS_PRINT_TEMP(tmp);
   SS_BLINK(ON);
   SysCnt.timeout = 0;
   while (1) {
     if (btn_pressed_f) {
       switch (Buttons_GetCode()) {
       case BTN_UP:

         if (tmp < TEMP_MAX_LIMIT) {
           tmp ++;
           SS_PRINT_TEMP(tmp);
         }
         SysCnt.timeout = 0;

         break;
       case BTN_DOWN:
         if (tmp > TEMP_MIN_LIMIT) {
           tmp --;
           SS_PRINT_TEMP(tmp);
         }
         SysCnt.timeout = 0;
         break;

//       case BTN_SEL:
//         SysState.set_temp = tmp;
//         SS_BLINK(OFF);
//         trulala();
//         SysVarRW(WR,&SV[vn_T_SET]);                   
//         return;

       }
     }
     if (SysCnt.timeout > SS_SET_MODE_TIME) {
			 SysState.set_temp = tmp;
       SS_BLINK(OFF);
				if(SysState.error_f){
					SS_PRINT_ERROR_CODE(SysState.error_code);
				}else{
					SS_PRINT_TEMP(DS_TEMP);
				}
       return;
     }
   }
 }

 
 
//void SysVarRW(_Bool rw, SysVar* sv) {
// 
//if (rw){

//		EEPROM_Write(sv->mem_addr, 2, (uint8_t*)sv->pVal);
//		
//	} else {
//		EEPROM_Read(sv->mem_addr, 2, (uint8_t*)sv->pVal);


//		
//	}	
//}

    //==============================================================
    //========================     BUTTONS     ========================



 void Buttons_Reset(void) {   
	 PCF8574_Init(0xFF);
	 PCF8574_ReadReg();
   btn_pressed_f = 0;
}




uint8_t Buttons_GetCode(void) {
   uint8_t btn_code;
	uint32_t tickstart = 0x00U;
	
   PCF8574_ReadReg();
   
   btn_code = (~PCF8574A_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE);


   if (!btn_code) {
      btn_pressed_f = 0;
      return 0;
   }
	 
	 //================
		tickstart = HAL_GetTick();
	 
   while ((~PCF8574A_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE)) {
      PCF8574_ReadReg();
		 
		 if (HAL_GetTick()-tickstart > LONG_PRESS_TIME){
			 btn_pressed_f = 0;
			 return btn_code = ((~PCF8574A_reg) & (BTN1_CODE | BTN2_CODE | BTN3_CODE | BTN4_CODE)) << 4;//Long Press   
		 }
   }
	 btn_pressed_f = 0;
	 return btn_code;//Short Press
	 
	   

}
    //========================     BUTTONS  END   =====================
