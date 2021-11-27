#ifndef MCP24S17_H_

#define MCP24S17_H_

#include <stdlib.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "spi.h"

#define MCP23S17_ADDRESS    0x40
////CONFIG DEFINES
#define MCP23S17_SPI_PORT 		&hspi1
#define MCP23S17_NSS_PORT			GPIOA	
#define MCP23S17_NSS_PIN			GPIO_PIN_4	
#define MCP23S17_RESET_PORT		GPIOA
#define MCP23S17_RESET_PIN		GPIO_PIN_4
#define MCP23S17_IRQA_PORT		GPIOA	
#define MCP23S17_IRQA_PIN			GPIO_PIN_4
#define MCP23S17_IRQB_PORT		GPIOA	
#define MCP23S17_IRQB_PIN			GPIO_PIN_4
//================================================

#define MCP23S17_NSS_RESET 		HAL_GPIO_WritePin(MCP23S17_NSS_PORT, MCP23S17_NSS_PIN, GPIO_PIN_RESET)
#define MCP23S17_NSS_SET 			HAL_GPIO_WritePin(MCP23S17_NSS_PORT, MCP23S17_NSS_PIN, GPIO_PIN_SET)

//#define IOCON         	0x05

//#define IODIRA        0x00      //Data Direction Register for PORTA   
//#define IODIRB        0x01      //Data Direction Register for PORTB
//#define IPOLA         0x02      //Input Polarity Register for PORTA
//#define IPOLB         0x03      //Input Polarity Register for PORTB
//#define GPINTENA      0x04      //Interrupt-on-change enable Register for PORTA
//#define GPINTENB      0x05      //Interrupt-on-change enable Register for PORTB
//#define DEFVALA       0x06      //Default Value Register for PORTA   
//#define DEFVALB       0x07      //Default Value Register for PORTB     
//#define INTCONA       0x08      //Interrupt-on-change control Register for PORTA 
//#define INTCONB       0x09      //Interrupt-on-change control Register for PORTB     
//#define IOCON         0x0A      //Configuration register for device                     
//#define GPPUA         0x0C      //100kOhm pullup resistor register for PORTA (sets pin to input when set)   
//#define GPPUB         0x0D      //100kOhm pullup resistor register for PORTB (sets pin to input when set)     
//#define INTFA         0x0E      //Interrupt flag Register for PORTA       
//#define INTFB         0x0F      //Interrupt flag Register for PORTB   
//#define INTCAPA       0x10      //Interrupt captured value Register for PORTA 
//#define INTCAPB       0x11      //Interrupt captured value Register for PORTB   
//#define GPIO_A        0x12      //General purpose I/O Register for PORTA 
//#define GPIO_B        0x13      //General purpose I/O Register for PORTB
//#define OLATA         0x14      //Output latch Register for PORTA
//#define OLATB         0x15      //Output latch Register for PORTB 


//							IOCON: CONFIGURATION REGISTER (ADDR 0x05)
//---------------------------------------------------------------------
//|	R/W-0 | R/W-0		| R/W-0	| R/W-0		| R/W-0 | R/W-0 | R/W-0		| U-0 |
//| BANK	| MIRROR	| SEQOP	| DISSLW	| HAEN	| ODR		| INTPOL	| — 	|
//---------------------------------------------------------------------

//bit 7 BANK: Controls how the registers are addressed
//1 = The registers associated with each port are separated into different banks.
//0 = The registers are in the same bank (addresses are sequential).
//bit 6 MIRROR: INT Pins Mirror bit
//1 = The INT pins are internally connected
//0 = The INT pins are not connected. INTA is associated with PORTA and INTB is associated with PORTB
//bit 5 SEQOP: Sequential Operation mode bit
//1 = Sequential operation disabled, address pointer does not increment.
//0 = Sequential operation enabled, address pointer increments.
//bit 4 DISSLW: Slew Rate control bit for SDA output
//1 = Slew rate disabled
//0 = Slew rate enabled
//bit 3 HAEN: Hardware Address Enable bit (MCP23S17 only) (Note 1)
//1 = Enables the MCP23S17 address pins.
//0 = Disables the MCP23S17 address pins.
//bit 2 ODR: Configures the INT pin as an open-drain output
//1 = Open-drain output (overrides the INTPOL bit.)
//0 = Active driver output (INTPOL bit sets the polarity.)
//bit 1 INTPOL: This bit sets the polarity of the INT output pin
//1 = Active-high
//0 = Active-low 
//bit 0 Unimplemented: Read as 0

#define A 0x0F
#define B 0x1F

enum MCP23S17_regs {
	IODR, 
	IPOL, 
	INTEN, 
	DEFVAL, 
	INTCON, 
	IOCON,
	GPPU,
	INTF,
	INTCAP,
	GPIO,
	OLAT
}; 

//typedef struct MCP23S17_reg {
//	uint8_t adr; 
//	uint8_t value;
//} MCP23S17_reg_t; 
 
//typedef struct _MCP23S17_PORT {
//	uint8_t IODR; 
//	uint8_t IPOL; 
//	uint8_t INTEN; 
//	uint8_t DEFVAL; 
//	uint8_t INTCON;
//	
//	uint8_t GPPU;
//	uint8_t INTF;
//	uint8_t INTCAP;
//	uint8_t GPIO;
//	uint8_t OLAT;
//} MCP23S17_port_t; 

typedef union IOCON_reg{
	struct {
		int8_t imp 		: 1;
		int8_t INTPOL : 1;
		int8_t ODR 		: 1;
		int8_t HAEN 	: 1;
		int8_t DISSLW : 1;
		int8_t SEQOP	: 1;
		int8_t MIRROR : 1;
		int8_t BANK 	: 1;
	}bits;
	int8_t IOCON_reg;
}IOCON_reg_t;





typedef struct _MCP23S17_STATUS {
	IOCON_reg_t CONF; 
	uint8_t PORT_A[11]; 
	uint8_t PORT_B[11];
} MCP23S17_Status_t; 
 
extern MCP23S17_Status_t MCP23S17;

void MCP23S17_init();

uint8_t MCP23S17_ReadReg(uint8_t port, uint8_t reg_addr);

void MCP23S17_WriteReg(uint8_t port, uint8_t reg_addr, uint8_t cmd) ;

void MCP23S17_ReadSeq(uint8_t port, uint8_t reg_addr);

void MCP23S17_WriteSeq() ;

#endif /* MCP24S17_H_ */