
#include "7seg.h"

#define WriteSS(data)			 HAL_SPI_Transmit(hSPI_PORT,data, 3, 1000);
uint8_t SS_CH;



void SevSegInit(void) {
	
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

void PrintDecSevSeg(int16_t dec){
uint8_t SS_data[]={0,0,0,0};
 if (dec > 999 || dec<-99) {
        
	 SS_data[0]=SEV_SEG_F;        
	 SS_data[1]=SEV_SEG_A;        
	 SS_data[2]=SEV_SEG_I;	      
	 SS_data[3]=SEV_SEG_L;
	 HAL_SPI_Transmit(hSPI_PORT,SS_data, 4, 1000);
   
 } else {
	// > 0		
	 #ifdef BLUE_BOARD
	 if (dec >= 0 ) {
		 //Xxx
		 SS_data[2]=decode_7SEG((dec % 100) % 10);
		//xXx				
		 if (dec >= 10 )
			 SS_data[1]=decode_7SEG((dec % 100) / 10);
   //xxX
		 if (dec >= 100 )
			 SS_data[0]=decode_7SEG(dec / 100);
		#endif
	 
	 #ifdef GREEN_BOARD
		if (dec >= 0 ) {
		 //Xxx
		 SS_data[1]=decode_7SEG((dec % 100) % 10);
		//xXx				
		 if (dec >= 10 )
			 SS_data[2]=decode_7SEG((dec % 100) / 10);
   //xxX
		 if (dec >= 100 )
			 SS_data[3]=decode_7SEG(dec / 100);
	 #endif
	 
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
 #ifdef BLUE_BOARD
		
		SS_LATCH();
 #endif
}

void trulala(void) {
	#ifdef BLUE_BOARD
	//uint8_t a[13]= {0x00, 0x04, 0x0C, 0x2C, 0x6C, 0xEC, 0xEE, 0xEA, 0xE2, 0xC2, 0x82, 0x2, 0x0};//BLUE SCREEN
	#endif
	#ifdef GREEN_BOARD
		uint8_t a[13]= {0x04, 0x05, 0x0D, 0x1D, 0x5D, 0x7D, 0x79, 0x78, 0x70, 0x60, 0x20, 0x00};// GREEN SCREEN
	#endif
	
	uint8_t b[4];
	uint8_t t = 100;
	uint8_t i=13;

	for(i=0;i<14;i++){//while(i--){
	
		b[0]=a[i];
		b[1]=a[i];
		b[2]=a[i];
		b[3]=a[i];

		HAL_SPI_Transmit(hSPI_PORT,b, 4, 1000);
		SS_LATCH();
		HAL_Delay(t);
	}

  
}
