#include "drive.h"

drive_t drv_m1;

uint32_t Drive_HomeInit(void){
	

//	if(drv_m1.max_sens_f & drv_m1.min_sens_f){
//		drv_m1.fail_f=1;
//		if(DRIVE_DEBUG_PRINT_F){
//			dbg_print("DRV: ERROR\r\n");
//		}
//		return 1;
//	}else	if(drv_m1.min_sens_f){
//		DRV_POS=0;
//		if(DRIVE_DEBUG_PRINT_F){
//			dbg_print("DRV HOME DETECT\r\n");
//		}
//		return 0;
//	}else{

		
		DRV_POS=0;
		drv_m1.dest_pos=100;
		DRV_CLOSE();
		
		if(DRIVE_DEBUG_PRINT_F){
			dbg_print("DRV HOME MOVE START\r\n");
		}
		
			while(1){		
				if(drv_m1.min_sens_f){
					DRV_STOP();
					if(DRIVE_DEBUG_PRINT_F){
						dbg_print("DRV HOME DETECT\r\n");
					}
					DRV_POS=0;
					drv_m1.dest_pos=0;
					return 0;
				}				
				if(drv_m1.max_sens_f){
					DRV_STOP();
					if(DRIVE_DEBUG_PRINT_F){
						dbg_print("ERROR: DRV MAX DETECT\r\n");
					}
					DRV_POS=0;
					drv_m1.dest_pos=0;
					drv_m1.fail_f=1;
					return 1;
				}				
				if(drv_m1.fail_f){ //SysCnt.drv_move>DRIVE_ROTATION_TIMEOUT
					DRV_POS=0;
					drv_m1.dest_pos=0;
					return 1;
				}
				
//				if(t!=DRV_POS){
//					t=DRV_POS;
//					if(DRIVE_DEBUG_PRINT_F){
//						sprintf(dbg_str,"POS = %d\r\n", DRV_POS);
//						dbg_print(dbg_str);
//					}
//				}
//			}
			
	}
}

_Bool Drive_CheckRotation(_Bool direction){
	//RUN OPEN
//	DRV_DIR=1;  
//	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
//	HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_SET); 
//	drv_m1.run_f=1;
//	SysCnt.drv_move=0;
	//RUN CLOSE
//	DRV_DIR=0;  
//	HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); 
//	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_SET); 
//	drv_m1.run_f=1;	
//	SysCnt.drv_move=0;
//STOP
//	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
//	HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); 
//	drv_m1.run_f = 0; 
	drv_m1.direction=direction;
	uint16_t temp_pos=drv_m1.pos;
	uint32_t t = HAL_GetTick();
	SS_PRINT_CUR_POS();
	//RUN 
	if(direction){
		DRV_OPEN();
		
	}else{
		DRV_CLOSE();
	}
	HAL_Delay(100);
	while(1){
		if(HAL_GetTick()-t>2000){
			DRV_STOP();
			drv_m1.fail_f=1;
			return 1;
		}
		HAL_Delay(1);
		//drv_m1.pos changed un EXTI4_IRQHandler(void)
		if(temp_pos!=drv_m1.pos){
			SS_PRINT_CUR_POS();
			break;
		}		
	}	
	DRV_STOP();
	drv_m1.fail_f=0;
	return 0;
	
}
