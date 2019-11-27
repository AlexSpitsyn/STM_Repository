#include "nrf24.h"

//------------------------------------------------
#define NRF_DBG

#ifdef NRF_DBG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t dt_reg;
char dbg_str[64];
//#define dbg_print(s) printf(s)
#define dbg_print(s) putsUSART(s)
#endif
//------------------------------------------------

uint8_t NRF_TX_ADDR[ADR_WIDTH] = { 0xb7, 0xb5, 0xa1 }; //HOST ADDRESS s0
uint8_t NRF_RX_ADDR[ADR_WIDTH] = { 0xb5, 0xb5, 0xa1 }; //SELF ADDRESS S1

uint8_t NRF_RX_BUF[TX_PLOAD_WIDTH + 1] = { 0 };
uint8_t NRF_RF_CH= 76; //2476 MHz


volatile uint8_t nrf_rx_flag = 0, nrf_tx_flag = 0, nrf_tx_fail=0;

uint8_t NRF_DBG_PRINT_F=0;


uint8_t str1[64];
//------------------------------------------------

__STATIC_INLINE void DelayMicro(__IO uint32_t micros) {

	micros *= (SystemCoreClock / 1000000) / 9;

	/* Wait till done */

	while (micros--)
		;

}

//------------------------------------------------

void NRF24_ToggleFeatures(void) {

	uint8_t dt[1] = { ACTIVATE };

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, dt, 1, 1000);

	DelayMicro(1);

	dt[0] = 0x73;

	HAL_SPI_Transmit(NRF_SPI_PORT, dt, 1, 1000);

	CS_OFF;

}
//------------------------------------------------
void NRF24_FlushRX(void) {

	uint8_t dt[1] = { FLUSH_RX };

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, dt, 1, 1000);

	DelayMicro(1);

	CS_OFF;

}

//------------------------------------------------

void NRF24_FlushTX(void) {

	uint8_t dt[1] = { FLUSH_TX };

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, dt, 1, 1000);

	DelayMicro(1);

	CS_OFF;

}

//------------------------------------------------

void NRF24_RX_Mode(void) {

	uint8_t regval = 0x00;

	regval = NRF24_ReadReg(CONFIG);

	

	regval |= (1 << PWR_UP) | (1 << PRIM_RX);

	NRF24_WriteReg(CONFIG, regval);

	NRF24_Write_Buf(TX_ADDR, NRF_RX_ADDR, ADR_WIDTH);

	NRF24_Write_Buf(RX_ADDR_P0, NRF_RX_ADDR, ADR_WIDTH);

	CE_SET;

	DelayMicro(150);

	// Flush buffers

	NRF24_FlushRX();

	NRF24_FlushTX();

}
//------------------------------------------------

void NRF24_TX_Mode(uint8_t *pBuf) {

	NRF24_Write_Buf(TX_ADDR, NRF_TX_ADDR, ADR_WIDTH); //set remote_addr

	NRF24_Write_Buf(RX_ADDR_P0, NRF_TX_ADDR, ADR_WIDTH); //ACK addr

	CE_RESET;

	// Flush buffers

	NRF24_FlushRX();

	NRF24_FlushTX();

}

//------------------------------------------------

void NRF24_Transmit(uint8_t addr, uint8_t *pBuf, uint8_t bytes) {

	CE_RESET;

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, &addr, 1, 1000); 

	DelayMicro(1);

	HAL_SPI_Transmit(NRF_SPI_PORT, pBuf, bytes, 1000);

	CS_OFF;

	CE_SET;

}

//------------------------------------------------

void NRF24_Send(uint8_t *pBuf) {

	uint8_t regval = 0x00;

	NRF24_TX_Mode(pBuf);

	regval = NRF24_ReadReg(CONFIG);

	

	regval |= (1 << PWR_UP);

	regval &= ~(1 << PRIM_RX);

	NRF24_WriteReg(CONFIG, regval);

	DelayMicro(150);

	

	NRF24_Transmit(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);

	CE_SET;

	DelayMicro(15); //minimum 10us high pulse (Page 21)

	CE_RESET;

}
//--------------------------------------------------
//function to receive data without IRQ
//uint8_t NRF24_Receive(uint8_t *pBuf) {
//
//	uint8_t status = 0x01;
//
//	status = NRF24_ReadReg(STATUS);
//
//#ifdef NRF_DBG
//	sprintf(dbg_str, "STATUS: 0x%02X\r\n", status);
//	printf(dbg_str);
//#endif
//
//	DelayMicro(10);
//
//	status = NRF24_ReadReg(STATUS);
//
//	if (status & 0x40) { // if data in PIPE0
//
//		NRF24_Read_Buf(RD_RX_PLOAD, pBuf, TX_PLOAD_WIDTH);
//
//		NRF24_WriteReg(STATUS, 0x40);
//
//		return 1;
//	}
//
//	return 0;
//}
//--------------------------------------------------

uint8_t NRF24_Init(void) {
	uint8_t status = 0;
	GPIO_InitTypeDef GPIO_InitStruct;	
	
	HAL_GPIO_DeInit(CE_GPIO_PORT,CE_PIN);
	HAL_GPIO_DeInit(CS_GPIO_PORT,CS_PIN);
	HAL_GPIO_DeInit(IRQ_GPIO_PORT,IRQ_PIN);
	
	
	GPIO_InitStruct.Pin = CE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CE_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_PORT, &GPIO_InitStruct);
	
	//TODO: ENABLE EXTI
  GPIO_InitStruct.Pin = IRQ_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IRQ_GPIO_PORT, &GPIO_InitStruct);
	
	CE_RESET;
	DelayMicro(5000);
	NRF24_WriteReg(CONFIG, 0x0A); // Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
	DelayMicro(5000);

	NRF24_WriteReg(EN_AA, 0x01); // Enable Pipe0
	NRF24_WriteReg(EN_RXADDR, 0x01); // Enable Pipe0

	//NRF24_WriteReg(EN_AA, 0x02); // Enable Pipe1
	//NRF24_WriteReg(EN_RXADDR, 0x02); // Enable Pipe1

	NRF24_WriteReg(SETUP_AW, 0x01); // Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x5F); // // 1500us, 15 retrans
	NRF24_ToggleFeatures();
	NRF24_WriteReg(FEATURE, 0);
	NRF24_WriteReg(DYNPD, 0);
	NRF24_WriteReg(STATUS, 0x70); //Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76); // 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); //TX_PWR:0dBm, Datarate:1Mbps

	//PIPE CONF
	NRF24_Write_Buf(TX_ADDR, NRF_TX_ADDR, ADR_WIDTH);
	NRF24_Write_Buf(RX_ADDR_P0, NRF_TX_ADDR, ADR_WIDTH);
	NRF24_WriteReg(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 0

	NRF24_RX_Mode(); //Go to RX mode

	status = NRF24_ReadReg(CONFIG);
	if (status == 0x0B) {
		
#ifdef NRF_DBG
		if(NRF_DBG_PRINT_F){
			dbg_print("NRF Init OK\r\n");
			dt_reg = NRF24_ReadReg(CONFIG);
				
			sprintf(dbg_str, "CONFIG: 0x%02X\r\n", dt_reg);
			dbg_print(dbg_str);

			dt_reg = NRF24_ReadReg(EN_AA);
			sprintf(dbg_str, "EN_AA: 0x%02X\r\n", dt_reg);
			dbg_print(dbg_str);

			dt_reg = NRF24_ReadReg(EN_RXADDR);
			sprintf(dbg_str, "EN_RXADDR: 0x%02X\r\n", dt_reg);
			dbg_print(dbg_str);

			dt_reg = NRF24_ReadReg(STATUS);
			sprintf(dbg_str, "STATUS: 0x%02X\r\n", dt_reg);
			dbg_print(dbg_str);
			
			dt_reg = NRF24_ReadReg(RF_CH);
			sprintf(dbg_str, "RF_CH: 0x%02X\r\n", dt_reg);
			dbg_print(dbg_str);

			dt_reg = NRF24_ReadReg(RF_SETUP);
			sprintf(dbg_str, "RF_SETUP: 0x%02X\r\n", dt_reg);
			dbg_print(dbg_str);

			NRF24_Read_Buf(TX_ADDR, (uint8_t*)dbg_str, 3);
			sprintf(dbg_str, "TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n", dbg_str[0],
					dbg_str[1], dbg_str[2]);
			dbg_print(dbg_str);

			NRF24_Read_Buf(RX_ADDR_P0, (uint8_t*)dbg_str, 3);
			sprintf(dbg_str, "RX_ADDR_P0: 0x%02X, 0x%02X, 0x%02X\r\n", dbg_str[0],
					dbg_str[1], dbg_str[2]);
			dbg_print(dbg_str);

			sprintf(dbg_str, "NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n", NRF_TX_ADDR[0],
					NRF_TX_ADDR[1], NRF_TX_ADDR[2]);
			dbg_print(dbg_str);

			sprintf(dbg_str, "NRF_RX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n", NRF_RX_ADDR[0],
					NRF_RX_ADDR[1], NRF_RX_ADDR[2]);
			dbg_print(dbg_str);
	}
#endif
		return 0;
	} else {
#ifdef NRF_DBG
		if(NRF_DBG_PRINT_F){
			dbg_print("NRF Init FAIL\r\n");
		}
#endif
		return 1;

	}
	//0 - OK
	//1 - FAIL
}

//--------------------------------------------------

uint8_t NRF24_ReadReg(uint8_t addr) {

	uint8_t dt = 0, cmd;

	CS_ON;

	HAL_SPI_TransmitReceive(NRF_SPI_PORT, &addr, &dt, 1, 1000);

	if (addr != STATUS) {

		cmd = 0xFF;

		HAL_SPI_TransmitReceive(NRF_SPI_PORT, &cmd, &dt, 1, 1000);
	}

	CS_OFF;

	return dt;

}

//------------------------------------------------

void NRF24_WriteReg(uint8_t addr, uint8_t dt) {

	addr |= W_REGISTER;

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, &addr, 1, 1000);

	HAL_SPI_Transmit(NRF_SPI_PORT, &dt, 1, 1000);
	CS_OFF;

}

//------------------------------------------------

void NRF24_Read_Buf(uint8_t addr, uint8_t *pBuf, uint8_t bytes) {

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, &addr, 1, 1000);
	HAL_SPI_Receive(NRF_SPI_PORT, pBuf, bytes, 1000);

	CS_OFF;

}

//------------------------------------------------

void NRF24_Write_Buf(uint8_t addr, uint8_t *pBuf, uint8_t bytes) {

	addr |= W_REGISTER;

	CS_ON;

	HAL_SPI_Transmit(NRF_SPI_PORT, &addr, 1, 1000);

	DelayMicro(1);

	HAL_SPI_Transmit(NRF_SPI_PORT, pBuf, bytes, 1000);

	CS_OFF;

}

//------------------------------------------------

void NRF24_IRQ_Callback(void) {

	uint8_t status = 0x01;

	DelayMicro(10);

	status = NRF24_ReadReg(STATUS);
#ifdef NRF_DBG
			if(NRF_DBG_PRINT_F){	
				sprintf(dbg_str, "\r\nIRQ STATUS: 0x%02X\r\n", status);	
				dbg_print(dbg_str);
			}
#endif

	if (status & 0x40) { // check Pipe0
#ifdef NRF_DBG
				if(NRF_DBG_PRINT_F){		
					dbg_print("\r\nIRQ: PIPE0 Receive\r\n");
				}
#endif

		
		NRF24_Read_Buf(RD_RX_PLOAD, NRF_RX_BUF, TX_PLOAD_WIDTH);

		//*(RX_BUF + 5) = pipe;

		NRF24_WriteReg(STATUS, 0x40);

		nrf_rx_flag = 1;

	}

	if (status & TX_DS) { //tx_ds == 0x20 , data send OK

#ifdef NRF_DBG
		if(NRF_DBG_PRINT_F){
			dbg_print("\r\nIRQ: TX_DS\r\n");
		}
#endif

		NRF24_WriteReg(STATUS, 0x20);

		NRF24_RX_Mode();

		nrf_tx_flag = 1;

	}

	else if (status & MAX_RT) {
#ifdef NRF_DBG
		
		if(NRF_DBG_PRINT_F){		
			dbg_print("\r\nIRQ: MAX_RT\r\n");
		}
#endif

		NRF24_WriteReg(STATUS, 0x10);

		NRF24_FlushTX();

		NRF24_RX_Mode();

		nrf_tx_fail=1;
	}
}


void NRF24_Get_State(void) {

		dt_reg = NRF24_ReadReg(CONFIG);

		sprintf(dbg_str, "CONFIG: 0x%02X\r\n", dt_reg);
		dbg_print(dbg_str);

		dt_reg = NRF24_ReadReg(EN_AA);
		sprintf(dbg_str, "EN_AA: 0x%02X\r\n", dt_reg);
		dbg_print(dbg_str);

		dt_reg = NRF24_ReadReg(EN_RXADDR);
		sprintf(dbg_str, "EN_RXADDR: 0x%02X\r\n", dt_reg);
		dbg_print(dbg_str);

		dt_reg = NRF24_ReadReg(STATUS);
		sprintf(dbg_str, "STATUS: 0x%02X\r\n", dt_reg);
		dbg_print(dbg_str);
	
		dt_reg = NRF24_ReadReg(RF_CH);
		sprintf(dbg_str, "RF_CH: 0x%02X\r\n", dt_reg);
		dbg_print(dbg_str);

		dt_reg = NRF24_ReadReg(RF_SETUP);
		sprintf(dbg_str, "RF_SETUP: 0x%02X\r\n", dt_reg);
		dbg_print(dbg_str);

		NRF24_Read_Buf(TX_ADDR, (uint8_t*)dbg_str, 3);
		sprintf(dbg_str, "TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n", dbg_str[0],
				dbg_str[1], dbg_str[2]);
		dbg_print(dbg_str);

		NRF24_Read_Buf(RX_ADDR_P0, (uint8_t*)dbg_str, 3);
		sprintf(dbg_str, "RX_ADDR_P0: 0x%02X, 0x%02X, 0x%02X\r\n", dbg_str[0], dbg_str[1], dbg_str[2]);
		dbg_print(dbg_str);

		sprintf(dbg_str, "NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n", NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2]);
		dbg_print(dbg_str);

		sprintf(dbg_str, "NRF_RX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n", NRF_RX_ADDR[0],	NRF_RX_ADDR[1], NRF_RX_ADDR[2]);
		dbg_print(dbg_str);

}


