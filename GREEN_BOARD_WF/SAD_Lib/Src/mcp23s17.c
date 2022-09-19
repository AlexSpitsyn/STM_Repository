#include "mcp23s17.h" 
#include <stdlib.h>



MCP23S17_Status_t MCP23S17 ={
.CONF= 0x00
};


void MCP23S17_Reset(){  
	HAL_GPIO_WritePin(MCP23S17_RESET_PORT, MCP23S17_RESET_PIN, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(MCP23S17_RESET_PORT, MCP23S17_RESET_PIN, GPIO_PIN_SET);	
	HAL_Delay(100);
	
	

} 
void MCP23S17_Init(){  
	//uint8_t BANK, uint8_t MIRROR, uint8_t SEQOP, uint8_t DISSLW, uint8_t HAEN, uint8_t ODR, uint8_t INTPOL
	//MCP23S17.CONF =	(BANK<<7)|(MIRROR<<6)|(SEQOP<<5)|(DISSLW<<4)|(HAEN<<3)|(ODR<<2)|(INTPOL<<1);
	MCP23S17_Reset();
	//BANK must be set to 1
	MCP23S17.CONF.bits.BANK=1; 
	//Write to default IOCON at 0x0A
	MCP23S17_NSS_RESET;
	HAL_Delay(100);
	MCP23S17_NSS_SET;
	MCP23S17_WriteReg (MCP_PORTA, 0x0A, MCP23S17.CONF.IOCON_reg);

}                                                                   
    

uint8_t MCP23S17_ReadReg(uint8_t port, uint8_t reg_addr) {	
	uint8_t spi_tx_data[3];
	uint8_t 	rxByte;

	spi_tx_data[0]=	MCP23S17_ADDRESS | 0x01;
	spi_tx_data[1]=	reg_addr + port;
	MCP23S17_NSS_RESET;	
	HAL_Delay(1);
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 2, 1000);
	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, &rxByte, 1, 1000);
	MCP23S17_NSS_SET;
	if(port){
		MCP23S17.PORT_B[reg_addr]=rxByte;
	}else{
		MCP23S17.PORT_A[reg_addr]=rxByte;
	}
	HAL_Delay(1);
	return rxByte;
}

void MCP23S17_WriteReg(uint8_t port, uint8_t reg_addr, uint8_t cmd) {
	volatile uint32_t stat;
	uint8_t spi_tx_data[3];
	spi_tx_data[0]=	MCP23S17_ADDRESS;
	spi_tx_data[1]=	reg_addr + port;
	spi_tx_data[2]=	cmd;
	if(port){
		MCP23S17.PORT_B[reg_addr]=cmd;
	}else{
		MCP23S17.PORT_A[reg_addr]=cmd;
	}
	
	MCP23S17_NSS_RESET;
	HAL_Delay(1);
	stat = HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 3, 1000);	
	MCP23S17_NSS_SET;
	HAL_Delay(1);
}

void MCP23S17_ReadSeq(void) {	
	uint8_t spi_tx_data[2];
	uint8_t spi_rx_data[22];
	spi_tx_data[0]=	MCP23S17_ADDRESS | 0x01;
	spi_tx_data[1]=	0;
	
	MCP23S17_NSS_RESET;
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 2, 1000);	
	HAL_Delay(100);
	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, MCP23S17.PORT_A, 11, 1000);
	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, MCP23S17.PORT_B, 11, 1000);
	//HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, spi_rx_data, 22, 1000);	
	MCP23S17_NSS_SET;
}

void MCP23S17_WriteSeq(void) {
	uint8_t spi_tx_data[12];
	spi_tx_data[0]=	MCP23S17_ADDRESS;
	spi_tx_data[1]=	0;
	MCP23S17.PORT_A[IOCON]=MCP23S17.CONF.IOCON_reg;
	MCP23S17.PORT_B[IOCON]=MCP23S17.CONF.IOCON_reg;
	
	MCP23S17_NSS_RESET;
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 2, 1000);	
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, MCP23S17.PORT_A, 11, 1000);
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, MCP23S17.PORT_B, 11, 1000);
	MCP23S17_NSS_SET;
	
}

void MCP23S17_PortConfig(uint8_t port, uint8_t direction) {

	MCP23S17_WriteReg(port, IODR, direction);
	//MCP23S17_WriteReg(port, GPIO, 0x00);
	
	//MCP23S17_WriteReg(port, OLAT, 0x00);

}

uint8_t MCP23S17_PortGet(uint8_t port) {
	return MCP23S17_ReadReg(port, GPIO);
	

}

void MCP23S17_PortPUP(uint8_t port, uint8_t pins) {
	MCP23S17_WriteReg(port, GPPU, pins);

}

void MCP23S17_PortSet(uint8_t port, uint8_t port_val) {
	MCP23S17_WriteReg(port, OLAT, port_val);
}



void MCP23S17_IRQen(uint8_t port, uint8_t IRQ_pins) {
	MCP23S17_WriteReg(port, GPINTEN, IRQ_pins);
	
}
