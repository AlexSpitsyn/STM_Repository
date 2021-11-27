#ifndef MCP24S17_H_

#define MCP24S17_H_

#include <stdlib.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "spi.h"
#include "sys_config.h"

//#define MCP23S17_ADDRESS    0x40
//////CONFIG DEFINES
//#define MCP23S17_SPI_PORT 		&hspi1
//#define MCP23S17_NSS_PORT			XP9_5o_GPIO_Port
//#define MCP23S17_NSS_PIN			XP9_5o_Pin
//#define MCP23S17_RESET_PORT		XP9_6o_GPIO_Port
//#define MCP23S17_RESET_PIN		XP9_6o_Pin
////#define MCP23S17_IRQA_PORT		MCP_IRQ_GPIO_Port
////#define MCP23S17_IRQA_PIN			XP9_1o_Pin
////#define MCP23S17_IRQB_PORT		MCP_IRQ_GPIO_Port	
////#define MCP23S17_IRQB_PIN			MCP_IRQ_1o_Pin
//#define MCP23S17_IRQ_PORT		MCP_IRQ_GPIO_Port	
//#define MCP23S17_IRQ_PIN		MCP_IRQ_Pin
//================================================

//XP9	- MCP

//1.IRQ2		ITAB (A2)
//2.MISO		6.SO
//3.MOSI		4.SI
//4.SCLK		3.SCK	
//5.NSS2		5.CS
//6.CE2			7.RESETn	
//7.VCC			1.VCC	
//8.GND			2.GND		



#define MCP23S17_NSS_RESET 		HAL_GPIO_WritePin(MCP23S17_NSS_PORT, MCP23S17_NSS_PIN, GPIO_PIN_RESET)
#define MCP23S17_NSS_SET 			HAL_GPIO_WritePin(MCP23S17_NSS_PORT, MCP23S17_NSS_PIN, GPIO_PIN_SET)


//Default Config Set
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

//INTCON
//1 = Controls how the associated pin value is compared for interrupt-on-change.
//0 = Pin value is compared against the previous pin value.
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

//If BANK=1
#define MCP_PORTA 0x00 
#define MCP_PORTB 0x10



enum MCP23S17_regs {
	IODR, 
	IPOL, 
	GPINTEN, 
	DEFVAL, 
	INTCON, 
	IOCON,
	GPPU,
	INTF,
	INTCAP,
	GPIO,
	OLAT
}; 


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

void MCP23S17_Reset();

void MCP23S17_Init();

uint8_t MCP23S17_ReadReg(uint8_t port, uint8_t reg_addr);

void MCP23S17_WriteReg(uint8_t port, uint8_t reg_addr, uint8_t cmd) ;

void MCP23S17_ReadSeq(void);

void MCP23S17_WriteSeq(void) ;

void MCP23S17_PortConfig(uint8_t port, uint8_t direction);
	
uint8_t MCP23S17_PortGet(uint8_t port) ;

void MCP23S17_PortSet(uint8_t port, uint8_t port_val);

void MCP23S17_IRQen(uint8_t port, uint8_t IRQ_pins);

void MCP23S17_PortPUP(uint8_t port, uint8_t pins);

#endif /* MCP24S17_H_ */
