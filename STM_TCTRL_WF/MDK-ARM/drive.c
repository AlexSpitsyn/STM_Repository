
#include "drive.h"

drive_t drv_m1;

void Drive_HomeInit(void){
	

	if(HAL_GPIO_ReadPin(MAX_SENS_GPIO_Port, MAX_SENS_Pin) == GPIO_PIN_SET){
		drv_m1.max_sens_f = 1;
		printf("DRV MAX SENS 1\r\n");
	}else{
		printf("DRV MAX SENS 0\r\n");
		drv_m1.max_sens_f = 0;
	}
 	if(HAL_GPIO_ReadPin(MIN_SENS_GPIO_Port, MIN_SENS_Pin) == GPIO_PIN_SET){
		drv_m1.min_sens_f = 1;
		printf("DRV MIN SENS 1\r\n");
	}else{
		drv_m1.min_sens_f = 0;
		printf("DRV MIN SENS 0\r\n");
	}
	
	if(drv_m1.max_sens_f & drv_m1.min_sens_f){
		drv_m1.fail_f=1;
		return;
	}else	if(drv_m1.min_sens_f){
		DRV_POS=0;

		return;
	}else{

		
		DRV_POS=0;
		drv_m1.dest_pos=100;
		DRV_CLOSE();
		
		printf("DRV HOME MOVE START\r\n");
		uint32_t t=0;
			while(1){		
				if(drv_m1.min_sens_f){
					DRV_STOP();
					printf("DRV HOME DETECT\r\n");
					DRV_POS=0;
					drv_m1.dest_pos=0;
					return;
				}				
				if(drv_m1.fail_f){
					DRV_POS=0;
					drv_m1.dest_pos=0;
					return;
				}
				if(t!=DRV_POS){
					t=DRV_POS;
					sprintf(sprintf_str,"POS = %d\r\n", DRV_POS);
					printf(sprintf_str);
				}
		
		
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

	_Bool Photosensor = HAL_GPIO_ReadPin(PHOTOSENSOR_GPIO_Port, PHOTOSENSOR_Pin);
	uint32_t t = HAL_GetTick();
	//RUN 
	if(direction){
		HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); 
  	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_SET); 
	}else{
		HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_SET); 
	}
	HAL_Delay(100);
	while(1){
		if(HAL_GetTick()-t>2000){
			HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
			HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET);
			drv_m1.fail_f=1;
			return 1;
		}
		HAL_Delay(1);
		if(Photosensor != HAL_GPIO_ReadPin(PHOTOSENSOR_GPIO_Port, PHOTOSENSOR_Pin)){
			break;
		}
		
	}
	//STOP
	HAL_GPIO_WritePin(M_DRV1_GPIO_Port,M_DRV1_Pin,GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(M_DRV2_GPIO_Port,M_DRV2_Pin,GPIO_PIN_RESET); 
	drv_m1.fail_f=0;
	return 0;
	
}
