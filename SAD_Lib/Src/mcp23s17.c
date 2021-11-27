#include "mcp23s17.h" 
#include <stdlib.h>



MCP23S17_Status_t MCP23S17 ={
.CONF= 0x00
};


void MCP23S17_init(){  
	//uint8_t BANK, uint8_t MIRROR, uint8_t SEQOP, uint8_t DISSLW, uint8_t HAEN, uint8_t ODR, uint8_t INTPOL
	//MCP23S17.CONF =	(BANK<<7)|(MIRROR<<6)|(SEQOP<<5)|(DISSLW<<4)|(HAEN<<3)|(ODR<<2)|(INTPOL<<1);
	MCP23S17_WriteReg (0, IOCON, MCP23S17.CONF.IOCON_reg);

}                                                                   
    

uint8_t MCP23S17_ReadReg(uint8_t port, uint8_t reg_addr) {	
	uint8_t spi_tx_data[3];
	uint8_t 	rxByte;

	spi_tx_data[0]=	MCP23S17_ADDRESS | 0x01;
	spi_tx_data[1]=	reg_addr & port;
	MCP23S17_NSS_RESET;	
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 2, 1000);
	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, &rxByte, 1, 1000);
	MCP23S17_NSS_SET;
	return rxByte;
}

void MCP23S17_WriteReg(uint8_t port, uint8_t reg_addr, uint8_t cmd) {
	uint8_t spi_tx_data[3];
	spi_tx_data[0]=	MCP23S17_ADDRESS;
	spi_tx_data[1]=	reg_addr & port;
	spi_tx_data[2]=	cmd;
	
	MCP23S17_NSS_RESET;
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 3, 1000);
	
	MCP23S17_NSS_SET;
}

void MCP23S17_ReadSeq(uint8_t port, uint8_t reg_addr) {	
	uint8_t spi_tx_data[3];
	spi_tx_data[0]=	MCP23S17_ADDRESS;
	spi_tx_data[1]=	0;
	MCP23S17_NSS_RESET;
	HAL_SPI_Transmit(MCP23S17_SPI_PORT, spi_tx_data, 2, 1000);	
	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, MCP23S17.PORT_A, 11, 1000);
	HAL_SPI_TransmitReceive(MCP23S17_SPI_PORT, spi_tx_data, MCP23S17.PORT_B, 11, 1000);
	MCP23S17_NSS_SET;
}

void MCP23S17_WriteSeq() {
	uint8_t spi_tx_data[3];
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

