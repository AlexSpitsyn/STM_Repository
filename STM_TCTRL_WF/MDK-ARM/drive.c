
#include "drive.h"

drive_t drv_m1;

void Drive_HomeInit(void){
	

	if(HAL_GPIO_ReadPin(MAX_SENS_GPIO_Port, MAX_SENS_Pin) == GPIO_PIN_RESET){
		drv_m1.max_sens_f = 1;
		printf("DRV MAX SENS 1\r\n");
	}else{
		printf("DRV MAX SENS 0\r\n");
		drv_m1.max_sens_f = 0;
	}
 	if(HAL_GPIO_ReadPin(MIN_SENS_GPIO_Port, MIN_SENS_Pin) == GPIO_PIN_RESET){
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
		drv_m1.position=0;
		return;
	}else{
		uint32_t t=HAL_GetTick();
		drv_m1.position=drv_m1.max_pos;
		int16_t temp_pos=drv_m1.max_pos;
		DRV_CLOSE();
		printf("DRV HOME MOVE START\r\n");
			
			while(1){		
				if(drv_m1.min_sens_f){
					DRV_STOP();
					drv_m1.position=0;
					break;
				}		
				
				if((HAL_GetTick()-t)>2000){
					if(temp_pos==drv_m1.position){
						DRV_STOP();
						drv_m1.fail_f=1;
						printf("DRV STOP HI\r\n");
						return;
					}else{
						sprintf(sprintf_str, "time = %d\r\n", (HAL_GetTick()-t));		
						printf(sprintf_str);
						t=HAL_GetTick();
					}
				}			
			}
			
	}
}

