#include "drive.h"

drive_t drv_m1;

void Drive_HomeInit(void){
	

	if(drv_m1.max_sens_f & drv_m1.min_sens_f){
		drv_m1.fail_f=1;
		if(DRIVE_DEBUG_PRINT_F){
			dbg_print("DRV: ERROR\r\n");
		}
		return;
	}else	if(drv_m1.min_sens_f){
		DRV_POS=0;

		return;
	}else{

		
		DRV_POS=0;
		drv_m1.dest_pos=100;
		DRV_CLOSE();
		
		if(DRIVE_DEBUG_PRINT_F){
			dbg_print("DRV HOME MOVE START\r\n");
		}
		uint32_t t=0;
			while(1){		
				if(drv_m1.min_sens_f){
					DRV_STOP();
					if(DRIVE_DEBUG_PRINT_F){
						dbg_print("DRV HOME DETECT\r\n");
					}
					DRV_POS=0;
					drv_m1.dest_pos=0;
					return;
				}				
				if(drv_m1.max_sens_f){
					DRV_STOP();
					if(DRIVE_DEBUG_PRINT_F){
						dbg_print("ERROR: DRV MAX DETECT\r\n");
					}
					DRV_POS=0;
					drv_m1.dest_pos=0;
					drv_m1.fail_f=1;
					return;
				}				
				if(drv_m1.fail_f){
					DRV_POS=0;
					drv_m1.dest_pos=0;
					return;
				}
				
//				if(t!=DRV_POS){
//					t=DRV_POS;
//					if(DRIVE_DEBUG_PRINT_F){
//						sprintf(dbg_str,"POS = %d\r\n", DRV_POS);
//						dbg_print(dbg_str);
//					}
//				}
			}
			
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
	
	//RUN 
	if(direction){
		HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); 
  	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_SET); 
		
	}else{
		HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_SET); 
	}
	HAL_Delay(500);
	while(1){
		if(HAL_GetTick()-t>2000){
			HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
			HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET);
			drv_m1.fail_f=1;
			return 1;
		}
		HAL_Delay(1);
		if(temp_pos!=drv_m1.pos){
			break;
		}
		
	}
	//STOP
	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); 
	drv_m1.fail_f=0;
	return 0;
	
}
