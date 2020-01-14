#ifndef WIRELESS_CTRL_H_
#define WIRELESS_CTRL_H_

#include "stm32f1xx_hal.h"
#include "nrf24.h"
#include "crc.h"
#include "ds18b20.h"
#include "usart.h"
#include "hw_crc32.h"


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
extern uint16_t ds_check_timer;
extern uint8_t ds_init_state;
extern uint8_t nrf_init_state;

enum CMD{
	CMD_PRESENT,
	CMD_GET,
	CMD_SET,
	CMD_EEPROM_WR,
	CMD_ERR_CLR
};

enum VAR{
	VAR_TEMP,
	VAR_T_CTRL,
	VAR_T_SET,
	VAR_T_CTRL_TIME,
	VAR_T_UPDT_TIME,
	VAR_T_HYST,
	VAR_DRV_POS,
	VAR_DRV_POS_MAX,
	VAR_DRV_POS_DEST,
	VAR_DRV_STEPS

};

enum PACK_STATE{
	PS_NEW,									//0
	PS_DONE,								//1
	PS_CMD_NOT_SUPPORTED,		//2
	PS_VAR_NOT_SUPPORTED,		//3
	PS_VAL_NOT_SUPPORTED,		//4
	PS_SEND_OK,							//5
	PS_NO_ACK,							//6
	PS_NOT_FOUND,						//7
	PS_CRC_OK,							//8
	PS_CRC_BAD,							//9
	PS_ERROR								//10
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
