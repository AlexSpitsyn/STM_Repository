#ifndef WIRELESS_CTRL_H_
#define WIRELESS_CTRL_H_

#include "stm32f1xx_hal.h"
#include "SX1278.h"
#include "crc.h"
#include "ds18b20.h"
#include "usart.h"
#include "hw_crc32.h"

extern _Bool dbg_f;
extern _Bool rx_handler;
#define WL_DEBUG_PRINT 				dbg_f
#define WL_PACKET_BUF 				SX1278.rxBuffer
#define WL_ADR_WIDTH 					4

#define WL_TX_COMPL 					SX1278.tx_complete_f
#define WL_RECEIVE						SX1278.receive_f
#define WL_TIMEOUT						SX1278.timeout_f
#define WL_PLOAD_WIDTH 				20 //bite in packet
//#define WL_SEND_BUF(buf) 			SX1278_LoRaEntryTx( WL_PLOAD_WIDTH, 2000); SX1278_LoRaTxPacket(buf, WL_PLOAD_WIDTH, SX1278_DEFAULT_TIMEOUT)
#define WL_SEND_BUF(buf) 			SX1278_transmit(buf, WL_PLOAD_WIDTH, SX1278_DEFAULT_TIMEOUT)
#define WL_PACKET_LIVE_TIME 	5000
#define WL_SEND_TIMEOUT	 			10000


//extern uint8_t WL_RX_ADDR[WL_ADR_WIDTH];
//extern uint8_t WL_RX_BUF[TX_PLOAD_WIDTH+1];

typedef union {
    
			uint32_t Val;
			char S[5];
		
}WL_ADDRESS;


extern WL_ADDRESS WL_ADDR;

typedef struct {//__attribute__((__packed__)){
uint32_t src_addr;
uint32_t dest_addr;

uint8_t flags;	
uint8_t state;
uint8_t cmd;
uint8_t var;
	
uint16_t val;	
uint16_t pack_ID;
	
uint32_t crc;	
}WL_Packet;

//extern WL_Packet TX_packet;
//extern WL_Packet RX_packet;
extern uint16_t packet_handler_timer;
extern uint16_t ds_check_timer;
extern uint8_t ds_init_state;
extern uint8_t wl_init_state;

enum CMD_STATE{
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
	PS_SEND_OK,							//2
	PS_ADDR_MATCH,					//3
	PS_ADDR_MISMATCH,				//4
	PS_CMD_NOT_SUPPORTED,		//5
	PS_VAR_NOT_SUPPORTED,		//6
	PS_VAL_NOT_SUPPORTED,		//7
	PS_CRC_BAD,							//8
	PS_CRC_OK,							//9
	PS_TIMEOUT,							//10
	PS_ERROR								//11
};

#define PACK_STATE_STR_CNT	13
static const char* PACK_STATE_STR[PACK_STATE_STR_CNT] = { 
		"NEW",
		"DONE",
		"SEND OK",
		"ADDR MATCH",
		"ADDR MISMATCH",
		"CMD NOT SUPPORTED",
		"VAR NOT SUPPORTED",
		"VAL NOT SUPPORTED",
		"CRC BAD",	
		"CRC OK",	
		"TIMEOUT",
		"ERROR"
		};

// struct packet_buf{

//	 uint32_t dest_addr;
//	 uint32_t src_addr;
//	 uint8_t pack_state;
//	
//	 uint8_t cmd;
//	 uint8_t var;
//   uint16_t val;
//	 uint16_t pack_ID;
//	 uint32_t live_time;
//struct packet_buf *next;
//};




void WL_Handler(void);
uint8_t WL_Send_Packet(uint16_t PID, uint8_t pack_state, uint32_t dest_addr, uint8_t cmd, uint8_t var, uint16_t val);
//uint16_t add_packet(WL_Packet* packet);
uint8_t WL_Check_Addr(uint8_t base_addr);
#endif