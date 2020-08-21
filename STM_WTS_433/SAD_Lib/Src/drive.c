
#include "drive.h"

drive_t drv_m1;

void Drive_HomeInit(void){
	
	if(drv_m1.fail_f){
		return;
	}
	
	if(HAL_GPIO_ReadPin(E_SENS_GPIO_Port,E_SENS_Pin)==GPIO_PIN_SET){
		drv_m1.dest_pos=-100;
		DRV_CLOSE();
		
		while (HAL_GPIO_ReadPin(E_SENS_GPIO_Port,E_SENS_Pin==GPIO_PIN_SET)&& drv_m1.run_f) {					
			if(Drive_CheckRotation()){
				//drv_m1.fail_f=1;
				DRV_STOP();
				break;					
			}					
			//HAL_Delay(500);		
		}
	DRV_STOP();
	HAL_Delay(1000);

	}
drv_m1.dest_pos=100;
drv_m1.position=0;	
//SS_PRINT_POS(	drv_m1.position);

	DRV_OPEN();	

	while (HAL_GPIO_ReadPin(E_SENS_GPIO_Port,E_SENS_Pin==GPIO_PIN_RESET)&& drv_m1.run_f) {
        
		if(Drive_CheckRotation()){
			//drv_m1.fail_f=1;
			DRV_STOP();
			break;
		}
		//HAL_Delay(200);
	}			
	

	drv_m1.position=0;	  
	drv_m1.dest_pos=0;  
	drv_m1.run_f=0;
	drv_m1.end_sens_f=0;
	
	
			

}

_Bool Drive_CheckRotation(void) {
    uint16_t moto_timer;

    moto_timer = TIM3->CNT;
    HAL_Delay(1000);
    if (moto_timer == TIM3->CNT){
      drv_m1.fail_f=1;  
			#ifdef DRIVE_DEBUG
			return 0;
			#else
			return 1;// 1 - FAIL
			#endif
			
    } else {
			
			drv_m1.fail_f=0;
        return 0;// 0 - OK
    }

}