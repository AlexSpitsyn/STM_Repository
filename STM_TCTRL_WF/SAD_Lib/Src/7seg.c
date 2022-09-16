
#include "7seg.h"





#define WriteSS(data)			 HAL_SPI_Transmit(SS_SPI_PORT,data, 3, 1000);

void putcSS(uint8_t c){
	HAL_SPI_Transmit(SS_SPI_PORT,&c, 1, 1000); 
}

void SevSeg_Init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;	
	uint8_t SS_data[]={0,0,0,0};
	
	HAL_GPIO_DeInit(SS_OE_PORT,SS_OE_PIN);
	HAL_GPIO_DeInit(SS_LE_PORT, SS_LE_PIN);
	
	HAL_GPIO_DeInit(SS_OE_PORT, SS_OE_PIN);
	HAL_GPIO_DeInit(SS_LE_PORT, SS_LE_PIN);
	
	GPIO_InitStruct.Pin = SS_OE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SS_OE_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = SS_LE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SS_LE_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(SS_LE_PORT, SS_LE_PIN, GPIO_PIN_RESET); 
  SS_OE(0);
	WriteSS(SS_data);
	SS_LATCH();
	trulala();
	
}

uint8_t decode_7SEG(int dec) {
    if (dec > 9)
        return 0;
    unsigned char v = 0;
    switch (dec) {
        case 0:
            v = SEV_SEG_0;
            break;
        case 1:
            v = SEV_SEG_1;
            break;
        case 2:
            v = SEV_SEG_2;
            break;
        case 3:
            v = SEV_SEG_3;
            break;
        case 4:
            v = SEV_SEG_4;
            break;
        case 5:
            v = SEV_SEG_5;
            break;
        case 6:
            v = SEV_SEG_6;
            break;
        case 7:
            v = SEV_SEG_7;
            break;
        case 8:
            v = SEV_SEG_8;
            break;
        case 9:
            v = SEV_SEG_9;
            break;
        default:
            v = 0;
            break;
    }
    return v;
}

void SevSeg_PrintDec(int16_t dec){
uint8_t SS_data[]={0,0,0,0};
 if (dec > 999 || dec<-99) {
        
	 SS_data[0]=SEV_SEG_F;        
	 SS_data[1]=SEV_SEG_A;        
	 SS_data[2]=SEV_SEG_I;	      
	 SS_data[3]=SEV_SEG_L;
	 HAL_SPI_Transmit(SS_SPI_PORT,SS_data, 4, 1000);
   
 } else {
	// > 0		

	 if (dec >= 0 ) {
		 //Xxx
		 SS_data[2]=decode_7SEG((dec % 100) % 10);
		//xXx				
		 if (dec >= 10 )
			 SS_data[1]=decode_7SEG((dec % 100) / 10);
   //xxX
		 if (dec >= 100 )
			 SS_data[0]=decode_7SEG(dec / 100);
	
	 

	 
	// < 0		
	 }else{			      
		 //Xxx
		 SS_data[2]=decode_7SEG((-dec % 100) % 10);     
		 //xXx
		 if (dec <= -10 ){
			 SS_data[1]=decode_7SEG((-dec % 100) / 10);            
			 SS_data[0]=SEV_SEG_NONE;
		 }else{
			 SS_data[1]=SEV_SEG_NONE;
		 }
	 }
	 
	WriteSS(SS_data);
 }

		
SS_LATCH();

}

void trulala(void) {

	uint8_t a[13]= {0x00, 0x04, 0x0C, 0x2C, 0x6C, 0xEC, 0xEE, 0xEA, 0xE2, 0xC2, 0x82, 0x2, 0x0};
	
	uint8_t b[4];
	uint8_t t = 100;
	uint8_t i=13;

	for(i=0;i<14;i++){//while(i--){
	
		b[0]=a[i];
		b[1]=a[i];
		b[2]=a[i];
		b[3]=a[i];

		HAL_SPI_Transmit(SS_SPI_PORT,b, 4, 1000);
		SS_LATCH();
		HAL_Delay(t);
	}  
}

void SevSeg_Idle(void) {

	static uint32_t timer=0;
	static uint8_t i=0;
	timer++;

	uint8_t ss_data[4] = {0};
	

	if(timer>0x0000A000){
		timer=0;
		ss_data[i]=SEV_SEG_NONE;
		
		switch (i){
			case 0:
				ss_data[0]=SEV_SEG_NONE; ss_data[1]=0; ss_data[2]=0; ss_data[3]=0;
				i++;
				break;
			case 1:
				ss_data[0]=0; ss_data[1]=SEV_SEG_NONE; ss_data[2]=0; ss_data[3]=0;
				i++;
				break;
			case 2:
				ss_data[0]=0; ss_data[1]=0; ss_data[2]=SEV_SEG_NONE; ss_data[3]=0;			
				i++;
				break;
			case 3:
				ss_data[0]=0; ss_data[1]=0; ss_data[2]=0; ss_data[3]=SEV_SEG_NONE;
				i++;
				break;
			case 4:
				ss_data[0]=0; ss_data[1]=0; ss_data[2]=SEV_SEG_NONE; ss_data[3]=0;
				i++;
				break;
			case 5:
				ss_data[0]=0; ss_data[1]=SEV_SEG_NONE; ss_data[2]=0; ss_data[3]=0;	
				i=0;
				break;
		}
		
		HAL_SPI_Transmit(SS_SPI_PORT,ss_data, 4, 1000);
		SS_LATCH();		
	}	
}
