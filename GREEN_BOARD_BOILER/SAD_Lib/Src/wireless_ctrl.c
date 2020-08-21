
#include "wireless_ctrl.h"


_Bool WL_DEBUG_PRINT=0;

_Bool rx_handler=1;
WL_Packet RX_packet;

uint16_t packet_handler_timer=0;


uint8_t wl_init_state=0;


struct packet_buf *pack_info =NULL;

WL_ADDRESS WL_ADDR;// = { .S = "WTS0" };

uint8_t pack_cnt=0;

uint8_t WL_Init(void){
	uint8_t res;
	SX1278_init(SX1278_POWER_17DBM, SX1278_LORA_SF_8, SX1278_LORA_BW_20_8KHZ, SX1278_CR_4_5 , WL_PLOAD_WIDTH);

	HAL_Delay(200);
	
	res = EEPROM_Read(EEPROM_WL_ADDR, WL_ADR_WIDTH, (uint8_t*)WL_ADDR.S);
	
	SX1278_LoRaEntryRx( WL_PLOAD_WIDTH, 2000); 
	return res;	
	
}
void print_pack_info(WL_Packet* packet){
	
			sprintf(dbg_str,"STATE: 0x%02X\r\n", packet->state);
			print_to(dbg_str);
			sprintf(dbg_str,"CMD: 0x%02X\r\n", packet->cmd);
			print_to(dbg_str);
			sprintf(dbg_str,"VAR: 0x%02X\r\n", packet->var);
			print_to(dbg_str);
			sprintf(dbg_str,"VAL: 0x%04X\r\n", packet->val);
			print_to(dbg_str);
			sprintf(dbg_str,"PID: 0x%04X\r\n", packet->pack_ID);
			print_to(dbg_str);	
			sprintf(dbg_str,"DEST ADDR: 0x%08X\r\n", packet->dest_addr);
			print_to(dbg_str);
			sprintf(dbg_str,"SRC ADDR: 0x%08X\r\n", packet->src_addr);
			print_to(dbg_str);	
//			sprintf(dbg_str,"STATE: 0x%02X\r\n", packet->state);
//			print_to(dbg_str);
			sprintf(dbg_str,"DESC: %s\r\n", packet->desc);
			print_to(dbg_str);
			sprintf(dbg_str,"CRC: 0x%08X\r\n", packet->crc);
			print_to(dbg_str); 
}


uint8_t WL_Get_Var(uint8_t var, int16_t* val, uint8_t* desc){
uint8_t state;
	
	if(var>= SYS_VAR_CNT){
		*val=0;				
		state=PS_VAR_NOT_SUPPORTED;
	}else{
		*val=(int16_t)*SV[var].pVal;
		strcpy((char*)desc, SV[var].name);	
		state=PS_DONE;
					
	}
	

	return state;
}

uint8_t WL_Set_Var(uint8_t var, uint16_t val, uint8_t* desc){
uint8_t state;	
	
	if(var>= SYS_VAR_CNT){		
		state=PS_VAR_NOT_SUPPORTED;
	}else{
		if(SV[var].we){
			if(val<=SV[var].vmax && val>=SV[var].vmin){
				*SV[var].pVal = (uint16_t)val;	
				strcpy((char*)desc, SV[var].name);	
				state=PS_DONE;				
			}else{
				state=PS_VAL_NOT_SUPPORTED;
			}
			
		}else{
			state=PS_CMD_NOT_SUPPORTED;
		}
	}
				

	return state;
}


uint8_t WL_Write_EEPROM(){
	for(int i; i<SYS_VAR_CNT; i++){		
		if (SV[i].mem_addr != 0){
			SysVarRW(WR, &SV[i]);
		}
	}
	return 0;
}

uint8_t WL_Send_Packet(uint16_t PID, uint8_t pack_state, uint32_t dest_addr, uint8_t cmd, uint8_t var, uint16_t val, uint8_t* desc){	
	
	WL_Packet TX_packet;
	uint8_t buf[WL_PLOAD_WIDTH+1]; 
	
	TX_packet.src_addr =WL_ADDR.Val;
	TX_packet.dest_addr=dest_addr;
	
	TX_packet.cmd=cmd;
	TX_packet.var=var;
	TX_packet.val=val;
	strcpy((char*)TX_packet.desc,(char*)desc);
	
	TX_packet.state=pack_state;
	if(PID){
		TX_packet.pack_ID=PID;	
	}else{
		TX_packet.pack_ID=(uint16_t)HAL_GetTick();	
	}
	
	TX_packet.crc=HW_CRC32((uint8_t*)&TX_packet, WL_PLOAD_WIDTH-4, 0xFFFFFFFF);
			
	if(WL_DEBUG_PRINT){
			sprintf(dbg_str,"\r\nTry Send Packet to: 0x%08X\r\n", dest_addr );
			dbg_print(dbg_str);		
			dbg_print("TX Packet Info\r\n");	
			print_pack_info(&TX_packet);
	}
		
	memcpy(buf,&TX_packet, WL_PLOAD_WIDTH);
	
	if(WL_SEND_BUF(buf)){
		if(WL_DEBUG_PRINT){				
				dbg_print("\r\nPacket Send: OK\r\n");				
		}			

		SX1278_LoRaEntryRx(WL_PLOAD_WIDTH,3000);

		return PS_SEND_OK;
	}else{
		if(WL_DEBUG_PRINT){				
				dbg_print("\r\nPacket Send: Timeout\r\n");				
		}			
		return PS_TIMEOUT;
	}	
}





uint8_t WL_Check_Packet(void){
	
	uint32_t crc;
	
	
	memcpy(	&RX_packet, &WL_PACKET_BUF ,WL_PLOAD_WIDTH);
	
	crc=HW_CRC32((uint8_t*)&RX_packet, WL_PLOAD_WIDTH-4 , 0xFFFFFFFF);
	//crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)	&RX_packet,2);	

	if(WL_DEBUG_PRINT){		
			dbg_print("RX Packet Info\r\n");	
			print_pack_info(&RX_packet);
			sprintf(dbg_str,"CRC CALK: 0x%08X\r\n",crc);
			dbg_print(dbg_str);
	}
	
	if(crc!=RX_packet.crc){
		if(WL_DEBUG_PRINT){		
			sprintf(dbg_str,"CRC check: %d / %s\r\n",PS_CRC_BAD, PACK_STATE_STR[PS_CRC_BAD]);
			dbg_print(dbg_str);
		}		
		return PS_CRC_BAD;
		
	}else{
		
		if(WL_DEBUG_PRINT){		
			sprintf(dbg_str,"CRC check: %d / %s\r\n",PS_CRC_BAD, PACK_STATE_STR[PS_CRC_OK]);
			dbg_print(dbg_str);
		}	
		
		if(RX_packet.dest_addr == WL_ADDR.Val){
			if(WL_DEBUG_PRINT){		
				sprintf(dbg_str,"ADDR check: %d / %s\r\n",PS_ADDR_MATCH, PACK_STATE_STR[PS_ADDR_MATCH]);
				dbg_print(dbg_str);
			}
			return PS_ADDR_MATCH;	
		}	else{
			if(WL_DEBUG_PRINT){		
				sprintf(dbg_str,"ADDR check: %d / %s\r\n",PS_ADDR_MISMATCH, PACK_STATE_STR[PS_ADDR_MISMATCH]);
				dbg_print(dbg_str);
			}
			return PS_ADDR_MISMATCH;	
		}
		
	}	
}

uint8_t WL_Run_CMD(uint8_t cmd){
	uint8_t state;
	switch (cmd){
				
		case CMD_PRESENT:
			state = PS_DONE;
		break;
				
		case CMD_GET: 
				state = WL_Get_Var(RX_packet.var, &RX_packet.val, RX_packet.desc);
		break;
				
		case CMD_SET: 
				state = WL_Set_Var(RX_packet.var, RX_packet.val, RX_packet.desc);
		break;
				
		case CMD_EEPROM_WR: 	
				state = WL_Write_EEPROM();
		break;
				
		case CMD_ERR_CLR: 	
				state = PS_CMD_NOT_SUPPORTED;			
		break;
				
		default:
			state = PS_CMD_NOT_SUPPORTED;
		break;		
			
	}
	return state;
}



void WL_Handler(void){
	
	uint8_t state=0;
	
		if(WL_RECEIVE){
			WL_RECEIVE=0;	
			if(WL_DEBUG_PRINT){		
				dbg_print("\r\n   Packet Received\r\n");
			}
			state= WL_Check_Packet();
			
			if((state==PS_ADDR_MATCH) && rx_handler){		
				HAL_Delay(500);
				
				state = WL_Run_CMD(RX_packet.cmd);
				if(WL_DEBUG_PRINT){		
						sprintf(dbg_str,"RUN CMD STATE: %s\r\n",PACK_STATE_STR[state]);
						dbg_print(dbg_str);
				}
				state = WL_Send_Packet(RX_packet.pack_ID, state ,RX_packet.src_addr, RX_packet.cmd, RX_packet.var, RX_packet.val, RX_packet.desc);
				if(WL_DEBUG_PRINT){		
					sprintf(dbg_str,"SEND PACKET STATE: %s\r\n",PACK_STATE_STR[state]);
					dbg_print(dbg_str);
				}
			}	
		}		
}



