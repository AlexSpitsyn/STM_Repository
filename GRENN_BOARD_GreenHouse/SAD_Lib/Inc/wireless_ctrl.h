#ifndef WIRELESS_CTRL_H_
#define WIRELESS_CTRL_H_

#include "sys_config.h"
#include "system.h"





extern _Bool dbg_f;
extern _Bool rx_handler;
extern _Bool WL_DEBUG_PRINT;

#define	WL_PACKET_BUF					SX1278.rxBuffer
#define WL_ADR_WIDTH 					4

#define WL_RECEIVE						SX1278.receive_f
#define WL_PLOAD_WIDTH 				36 //bite in packet
#define WL_SEND_BUF(buf) 			SX1278_transmit(buf, WL_PLOAD_WIDTH, SX1278_DEFAULT_TIMEOUT)



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

uint8_t dev_error_code;	
uint8_t state;
uint8_t cmd;
uint8_t var;
	
int16_t val;	
uint16_t pack_ID;
	
uint8_t desc[16];	
	
uint32_t crc;		
}WL_Packet;

//extern WL_Packet TX_packet;
//extern WL_Packet RX_packet;
extern uint16_t packet_handler_timer;


extern uint8_t wl_init_state;

enum CMD_STATE{
	CMD_PRESENT,
	CMD_GET,
	CMD_SET,
	CMD_EEPROM_WR,
	CMD_ERR_CLR,
	CMD_RESET
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
	PS_SEND_OK,							//1
	PS_ADDR_MATCH,					//2
	PS_ADDR_MISMATCH,				//3	
	PS_CRC_BAD,							//4
	PS_CRC_OK,							//5
	PS_TIMEOUT,							//6
	PS_ERROR								//7
};

enum WL_CMD_STATE{								
	CMD_DONE,							//0
	CMD_NOT_SUPPORTED,		//1
	VAR_NOT_SUPPORTED,		//2
	VAL_NOT_SUPPORTED,		//3	
	CMD_ERROR							//4
};

#define PACK_STATE_STR_CNT	8
static const char* PACK_STATE_STR[PACK_STATE_STR_CNT] = { 
		"NEW",		
		"SEND OK",
		"ADDR MATCH",
		"ADDR MISMATCH",		
		"CRC BAD",	
		"CRC OK",	
		"TIMEOUT",
		"ERROR"
		};
#define CMD_STATE_STR_CNT	5
static const char* CMD_STATE_STR[PACK_STATE_STR_CNT] = { 		
		"DONE",		
		"CMD NOT SUPPORTED",
		"VAR NOT SUPPORTED",
		"VAL NOT SUPPORTED",		
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




uint8_t WL_Init(void);
void WL_Handler(void);
uint8_t WL_Send_Packet(uint16_t PID, uint8_t pack_state, uint32_t dest_addr, uint8_t cmd, uint8_t var, uint16_t val, uint8_t* desc);
		


#endif
