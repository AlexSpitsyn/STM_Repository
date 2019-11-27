#ifndef WIRELESS_CTRL_H_
#define WIRELESS_CTRL_H_

#include "stm32f1xx_hal.h"
#include "nrf24.h"
#include "crc.h"
#include "ds18b20.h"
#include "usart.h"
#include "system.h"


typedef struct {//__attribute__((__packed__)){
uint8_t state;	
uint8_t host_addr;
uint8_t cmd;
uint8_t var;

uint16_t val;
uint16_t pack_ID;
	
uint32_t crc;	
}WL_Packet;

extern WL_Packet TX_packet;
extern WL_Packet RX_packet;
extern uint16_t packet_handler_timer;
extern _Bool packet_handler_f;

enum CMD{
	CMD_PRESENT,			//0
	CMD_GET,					//1
	CMD_SET,					//2
	CMD_EEPROM_WR,		//3
	CMD_ERR_CLR				//4
};

enum VAR{
	VAR_TEMP,					//0
	VAR_T_CTRL,				//1
	VAR_T_SET,				//2
	VAR_T_CTRL_TIME,	//3
	VAR_T_UPDT_TIME,	//4
	VAR_T_HYST,				//5
	VAR_DRV_POS,			//6
	VAR_DRV_POS_MAX,	//7
	VAR_DRV_POS_DEST,	//8
	VAR_DRV_STEPS,			//9
	VAR_ERR_CODE
};

enum PACK_STATE{
	PS_NEW,									
	PS_DONE,
	PS_CMD_NOT_SUPPORTED,
	PS_VAR_NOT_SUPPORTED,
	PS_VAL_NOT_SUPPORTED,
	PS_SEND_OK,
	PS_NO_ACK,
	PS_NOT_FOUND,
	PS_CRC_OK,
	PS_CRC_BAD,
	PS_ERROR
};

 struct packet_buf{

	 uint8_t pack_state;
	 uint8_t host_addr;
	 uint8_t cmd;
	 uint8_t var;
   uint16_t val;
	 uint16_t pack_ID;
	 uint32_t time_stamp;
struct packet_buf *next;
};




void WL_Handler(void);
uint8_t WL_Send_Packet(void);
void add_packet(uint16_t p_id, uint8_t p_state, uint8_t p_host_addr, uint8_t p_cmd, uint8_t p_var, uint8_t p_val);
uint8_t WL_Check_Addr(uint8_t base_addr);
#endif
