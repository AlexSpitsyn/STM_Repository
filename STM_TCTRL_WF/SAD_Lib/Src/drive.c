#include "drive.h"

drive_t drv_m1;

uint32_t Drive_HomeInit(void){
	
	int16_t temp_pos;

	GPIO_PinState photo_sensor;
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

	
	if(PHOTO_SENSOR_GET_STATE()){
		drv_m1.dest_pos=0;
		drv_m1.pos=100;		
		DRV_CLOSE();		
	}else{
		drv_m1.dest_pos=0;
		drv_m1.pos=5;
		DRV_CLOSE();
		while(drv_m1.run_f){
			if(drv_m1.fail_f)
				HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
				return 1;			
		}
		HAL_Delay(200);
		if(!PHOTO_SENSOR_GET_STATE()){			
			drv_m1.dest_pos=10;
			drv_m1.pos=0;
			DRV_OPEN();
			while(drv_m1.run_f){
				if(drv_m1.fail_f)
					HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
					return 1;			
			}
		}

	}	
	
	photo_sensor=PHOTO_SENSOR_GET_STATE();
	if(photo_sensor){
		drv_m1.dest_pos=0;
		drv_m1.pos=40;
		DRV_CLOSE();
	}else{			
			drv_m1.dest_pos=120;
			drv_m1.pos=0;
			DRV_OPEN();
	}
				
	temp_pos=drv_m1.pos;	

	while(photo_sensor==PHOTO_SENSOR_GET_STATE()){					
		if(drv_m1.fail_f | !drv_m1.run_f){		
			return 1;			
		}			
	}	
	DRV_STOP();
	drv_m1.pos=0;
	drv_m1.dest_pos=0;

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	return 0;


}

_Bool Drive_CheckRotation(_Bool direction){

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
