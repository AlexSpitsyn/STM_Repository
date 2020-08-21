#include "1-wire.h"


//--------------------------------------------------
inline void ow_DelayMicro(__IO uint32_t micros) {
	micros *= (SystemCoreClock / 1000000) / 9;
	/* Wait till done */
	while (micros--);
}

	
void ow_low()
{
	OW_PORT->BSRR =OW_PIN<<16;
}	
void ow_high()
{
	OW_PORT->BSRR =OW_PIN;
}	
void ow_input()
{
	GPIO_InitTypeDef	gpinit;
	gpinit.Mode = GPIO_MODE_INPUT;
	gpinit.Pull = GPIO_NOPULL;
	gpinit.Speed = GPIO_SPEED_FREQ_HIGH;
	gpinit.Pin = OW_PIN;	
	HAL_GPIO_Init(OW_PORT,&gpinit);
}	
void ow_output()
{
	GPIO_InitTypeDef	gpinit;
	gpinit.Mode = GPIO_MODE_OUTPUT_OD;
	gpinit.Pull = GPIO_NOPULL;
	gpinit.Speed = GPIO_SPEED_FREQ_HIGH;
	gpinit.Pin = OW_PIN;
	HAL_GPIO_Init(OW_PORT,&gpinit);

}

inline uint8_t ow_Reset(void)
{
	uint8_t i;
	
	/* Line low, and wait 480us */
	ow_low();
	ow_output();
	ow_DelayMicro(480);
	ow_DelayMicro(20);
	/* Release line and wait for 70us */
	ow_input();
	ow_DelayMicro(70);
	/* Check bit value */
	i = HAL_GPIO_ReadPin(OW_PORT, OW_PIN);
	
	/* Delay for 410 us */
	ow_DelayMicro(410);
	/* Return value of presence pulse, 0 = OK, 1 = ERROR */
	return i;
}

inline void ow_WriteBit(uint8_t bit)
{
	if (bit) 
	{
		/* Set line low */
		ow_low();
		ow_output();
		ow_DelayMicro(10);
		
		/* Bit high */
		ow_input();
		
		/* Wait for 55 us and release the line */
		ow_DelayMicro(55);
		ow_input();
	} 
	else 
	{
		/* Set line low */
		ow_low();
		ow_output();
		ow_DelayMicro(65);
		
		/* Bit high */
		ow_input();
		
		/* Wait for 5 us and release the line */
		ow_DelayMicro(5);
		ow_input();
	}

}

inline uint8_t ow_ReadBit() 
{
	uint8_t bit = 0;
	
	/* Line low */
	ow_low();
	ow_output();
	ow_DelayMicro(2);
	
	/* Release line */
	ow_input();
	ow_DelayMicro(10);
	
	/* Read line value */
	if (HAL_GPIO_ReadPin(OW_PORT, OW_PIN)) {
		/* Bit is HIGH */
		bit = 1;
	}
	
	/* Wait 50us to complete 60us period */
	ow_DelayMicro(50);
	
	/* Return bit value */
	return bit;
}

void ow_WriteByte(uint8_t byte) {
	uint8_t i = 8;
	/* Write 8 bits */
	while (i--) {
		/* LSB bit is first */
		ow_WriteBit(byte & 0x01);
		byte >>= 1;
	}
}

uint8_t ow_ReadByte() {
	uint8_t i = 8, byte = 0;
	while (i--) {
		byte >>= 1;
		byte |= (ow_ReadBit() << 7);
	}
	
	return byte;
}
