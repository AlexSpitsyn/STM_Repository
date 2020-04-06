
#include "wireless_ctrl.h"
#include "i2c.h"


_Bool dbg_f=1;
_Bool rx_handler=1;
WL_Packet RX_packet;
char str[64]; 
uint16_t packet_handler_timer=0;
uint16_t ds_check_timer=0;

uint8_t nrf_init_state=0;
uint8_t ds_init_state=0;

struct packet_buf *pack_info =NULL;

WL_ADDRESS WL_ADDR = { .S = "WTS0" };



#define MAX_PACK_CNT 10
uint8_t pack_cnt=0;

//uint8_t WL_RX_ADDR[WL_ADR_WIDTH] = "WTS0"; //SELF ADDRESS S1

uint8_t WL_Check_Packet(void);

uint8_t WL_Check_Addr(uint8_t base_addr){
	
	//0 - addr free READY
	//1 - init fail
	//2 - addr busy
	//3 - timeout	
	
	uint8_t state=0xFF;
	uint32_t t;
	//base_addr &=0xF0;
	
	//If PCF8754 set addr
//	PCF8574_ReadReg();
//	base_addr |= PCF8574A_reg>>4;
	HAL_StatusTypeDef res;
	res = HAL_I2C_Master_Transmit(&hi2c2, 0x40, &state, 1, HAL_MAX_DELAY);
	res = HAL_I2C_Master_Receive(&hi2c2, 0x40, &base_addr, 1, HAL_MAX_DELAY);
	printf("I2C STATUS CODE: %d\r\n",res);
	printf("I2C ERROR CODE: %d\r\n",HAL_I2C_GetError(&hi2c2));
	//base_addr |= base_addr>>4;
	//If GPIO set addr	
//	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A1_Pin);
//	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A2_Pin)<<1;
//	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A3_Pin)<<2;
//	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A4_Pin)<<3;
	
	
	//base_addr+=48;
	
	
	WL_ADDR.S[WL_ADR_WIDTH-1]=base_addr;
	

state= WL_Send_Packet(0xDEAD, PS_NEW, WL_ADDR.Val, CMD_PRESENT, 0, 0);
	
	if(state == PS_SEND_OK){
		t=HAL_GetTick();
		while(1){
			if(WL_RECEIVE){
				WL_RECEIVE=0;	
				state= WL_Check_Packet();
			
				if(state==PS_ADDR_MATCH){				
					if(RX_packet.pack_ID == 0xDEAD){						
						sprintf(str,"SX1278 ADDR: 0x%02X - BUSY\r\nSET COMMON ADDR: 0x%02X\r\n", WL_ADDR.Val+base_addr, WL_ADDR.Val);
						putsUSART(str);
						return 2;
						
					}
				}	
		}
			
			
		if((HAL_GetTick()-t)>2000){
			
			putsUSART("ADDR FREE\r\n>");			
			WL_ADDR.Val+=base_addr;	
			sprintf(str,"SX1278 ADDR: 0x%02X \r\n", WL_ADDR.Val);
			putsUSART(str);
			return 3;
			}	
		}
	}


}

void print_pack_info(WL_Packet* packet){

			sprintf(str,"CMD: 0x%02X\r\n", packet->cmd);
			putsUSART(str);
			sprintf(str,"VAR: 0x%02X\r\n", packet->var);
			putsUSART(str);
			sprintf(str,"VAL: 0x%02X\r\n", packet->val);
			putsUSART(str);
			sprintf(str,"P_ID: 0x%02X\r\n", packet->pack_ID);
			putsUSART(str);	
			sprintf(str,"DEST ADDR: 0x%02X\r\n", packet->dest_addr);
			putsUSART(str);
			sprintf(str,"SRC ADDR: 0x%02X\r\n", packet->src_addr);
			putsUSART(str);	
//			sprintf(str,"STATE: 0x%02X\r\n", packet->state);
//			putsUSART(str);
			sprintf(str,"CRC: 0x%02X\r\n", packet->crc);
			putsUSART(str); 
}

//uint16_t add_packet(WL_Packet* packet){
//  struct packet_buf *temp;
//  if(pack_cnt==0){
//		pack_info=(struct packet_buf*)malloc(sizeof(struct packet_buf));
//		pack_info->next=pack_info;	
//	}else{
//		temp = (struct packet_buf*)malloc(sizeof(struct packet_buf));
//		temp->next = pack_info->next; 
//		pack_info->next=temp;
//		pack_info=pack_info->next;		
//	}
//	
//	pack_info->pack_ID=packet->pack_ID;
//	
//	pack_info->pack_state=packet->state;
//	pack_info->dest_addr=packet->dest_addr;
//	pack_info->src_addr=packet->src_addr;
//	pack_info->cmd=packet->cmd;
//	pack_info->var=packet->var;
//	pack_info->val=packet->val;		
//	pack_info->live_time=WL_PACKET_LIVE_TIME;
//	
//	pack_cnt++;
//	
//		
//	if(WL_DEBUG_PRINT){
//		sprintf(str,"\r\nAdd Packet ID: 0x%02X\r\n", pack_info->pack_ID);
//		putsUSART(str);
//	}			
//	
//	return pack_info->pack_ID;
//}

//uint8_t get_packet( uint16_t p_id){

//	uint8_t c = pack_cnt;
//	while(c--){
//		if(pack_info->pack_ID==p_id){
//			return 0; //FOUND
//		}
//			
//		pack_info=pack_info->next;
//	}
//	return 1;  // NOT FOUND
//}

//uint8_t remove_packet( uint16_t p_id){
//  struct packet_buf *temp;
//	uint8_t c = pack_cnt;
//	temp=pack_info;
//  if(get_packet(p_id)){
//		if(pack_cnt==1){
//			free(pack_info);
//			pack_info=NULL;
//			pack_cnt--;
//					
//			if(WL_DEBUG_PRINT){
//				sprintf(str,"\r\nRemove Packet ID: 0x%02X\r\n", p_id);
//				putsUSART(str);
//			}	
//			return 1;
//		}else{
//			while (c--){ 
//				if(temp->next==pack_info){
//					temp->next = pack_info->next;
//					free(pack_info);
//					pack_info=temp;
//					pack_cnt--;
//					if(WL_DEBUG_PRINT){
//						sprintf(str,"\r\nRemove Packet ID: 0x%02X\r\n", p_id);
//						putsUSART(str);
//					}
//					return 1;
//				}
//				temp = temp->next;
//			}
//		}		
//		return 0;		
//	}else{
//		return 0;
//	}
//	
//	
//}



uint8_t WL_Get_Var(uint8_t var, uint16_t* val){
uint8_t state;
	switch (var){
				case VAR_TEMP: 
					if(ds_init_state){
						state=PS_ERROR;
					}else{
						ds18b20_GetTemp();
						*val=DS_TEMP;				
						state=PS_DONE;
					}

				break;
//				case VAR_T_CTRL: 
//				TX_packet.val=SysState.temp_ctrl_f;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_SET: 
//				TX_packet.val=SysState.set_temp;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_CTRL_TIME: 
//				TX_packet.val=SysState.t_ctrl_time;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_UPDT_TIME: 
//				TX_packet.val=SysState.t_updt_time;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_HYST: 
//     		TX_packet.val=SysState.t_hyst;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_POS: 
//				TX_packet.val=drv_m1.position;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_POS_MAX: 
//				TX_packet.val=drv_m1.max_pos;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_POS_DEST: 
//				TX_packet.val=drv_m1.dest_pos;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_STEPS: 
//				TX_packet.val=drv_m1.step;				
//				TX_packet.pack_state=RSP_OK;
//				break;
				
				default:				
				val=0;				
				state=PS_VAR_NOT_SUPPORTED;
				break;
			}
	return state;
}

uint8_t WL_Set_Var(uint8_t var, uint16_t val){
uint8_t state;	
	switch (var){
				
				case VAR_TEMP: 		
				state=PS_VAR_NOT_SUPPORTED;
				break;
//				case VAR_T_CTRL: 
//				SysState.temp_ctrl_f=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_SET: 
//				TX_packet.val=SysState.set_temp=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_CTRL_TIME: 
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_UPDT_TIME: 
//				SysState.t_updt_time=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_T_HYST: 
//     		SysState.t_hyst=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_POS: 
//				drv_m1.position=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_POS_MAX: 
//				drv_m1.max_pos=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_POS_DEST: 
//				drv_m1.dest_pos=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
//				case VAR_DRV_STEPS: 
//				drv_m1.step=RX_packet.val;				
//				TX_packet.pack_state=RSP_OK;
//				break;
				
				default:				
							
				state=PS_VAR_NOT_SUPPORTED;
				break;
			}
	return state;
}


uint8_t WL_Send_Packet(uint16_t PID, uint8_t pack_state, uint32_t dest_addr, uint8_t cmd, uint8_t var, uint16_t val){	
	
	uint32_t t;	
	WL_Packet TX_packet;
	uint8_t buf[WL_PLOAD_WIDTH+1]; 
	
	TX_packet.src_addr =WL_ADDR.Val;
	TX_packet.dest_addr=dest_addr;
	
	TX_packet.cmd=cmd;
	TX_packet.var=var;
	TX_packet.val=val;
	
	TX_packet.state=pack_state;
	if(PID){
		TX_packet.pack_ID=PID;	
	}else{
		TX_packet.pack_ID=(uint16_t)HAL_GetTick();	
	}
	
	
	//add_packet(&TX_packet);	
	

	TX_packet.crc=HW_CRC32((uint8_t*)&TX_packet, WL_PLOAD_WIDTH-4, 0xFFFFFFFF);
	//TX_packet.crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)&TX_packet,2);
		
	if(WL_DEBUG_PRINT){
			sprintf(str,"\r\nTry Send Packet to: 0x%08X\r\n", dest_addr );
			putsUSART(str);		
			putsUSART("TX Packet Info\r\n");	
			print_pack_info(&TX_packet);
	}
		
	memcpy(buf,&TX_packet, WL_PLOAD_WIDTH);
	
	if(WL_SEND_BUF(buf)){
		if(WL_DEBUG_PRINT){				
				putsUSART("\r\nPacket Send: OK\r\n");				
		}			

		SX1278_RX_Mode();

		return PS_SEND_OK;
	}else{
		if(WL_DEBUG_PRINT){				
				putsUSART("\r\nPacket Send: Timeout\r\n");				
		}			
		return PS_TIMEOUT;
	}





	
//	t=HAL_GetTick();
//	while(1){
//		if(WL_TX_COMPL){
//			WL_TX_COMPL=0;	
//			
//			if(WL_DEBUG_PRINT){				
//				putsUSART("\r\nPacket Send: OK\r\n");				
//			}			
//			return PS_SEND_OK;
//		}
//		if(WL_TIMEOUT){
//			WL_TIMEOUT=0;
//			
//			if(WL_DEBUG_PRINT){				
//				putsUSART("\r\nPacket Send: Timeout\r\n");	
//			}			
//			return PS_TIMEOUT;
//		}

//		if((HAL_GetTick()-t)>WL_SEND_TIMEOUT){
//			if(WL_DEBUG_PRINT){				
//				putsUSART("\r\nSX1278 ERROR\r\n");	
//			}	
//			return PS_ERROR ;
//		}
//	}
	
}





uint8_t WL_Check_Packet(void){
	
	uint32_t crc;
	//WL_Packet RX_packet;
	memcpy(	&RX_packet, &WL_PACKET_BUF,WL_PLOAD_WIDTH);
	crc=HW_CRC32((uint8_t*)&RX_packet, WL_PLOAD_WIDTH-4 , 0xFFFFFFFF);
	//crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)	&RX_packet,2);	

	if(WL_DEBUG_PRINT){		
			putsUSART("RX Packet Info\r\n");	
			print_pack_info(&RX_packet);
			sprintf(str,"CRC CALK: 0x%08X\r\n",crc);
			putsUSART(str);
	}
	
	if(crc!=RX_packet.crc){
		if(WL_DEBUG_PRINT){		
			sprintf(str,"CRC check: %d / %s\r\n",PS_CRC_BAD, PACK_STATE_STR[PS_CRC_BAD]);
			putsUSART(str);
		}		
		return PS_CRC_BAD;
		
	}else{
		
		if(WL_DEBUG_PRINT){		
			sprintf(str,"CRC check: %d / %s\r\n",PS_CRC_BAD, PACK_STATE_STR[PS_CRC_OK]);
			putsUSART(str);
		}	
		
		if(RX_packet.dest_addr == WL_ADDR.Val){
			if(WL_DEBUG_PRINT){		
				sprintf(str,"ADDR check: %d / %s\r\n",PS_ADDR_MATCH, PACK_STATE_STR[PS_ADDR_MATCH]);
				putsUSART(str);
			}
			return PS_ADDR_MATCH;	
		}	else{
			if(WL_DEBUG_PRINT){		
				sprintf(str,"ADDR check: %d / %s\r\n",PS_ADDR_MISMATCH, PACK_STATE_STR[PS_ADDR_MISMATCH]);
				putsUSART(str);
			}
			return PS_ADDR_MISMATCH;	
		}
		
	}	
}

uint8_t WL_Run_CMD(uint8_t cmd){
	switch (cmd){
				
		case CMD_PRESENT:
			return PS_DONE;
		break;
				
		case CMD_GET: 
				return WL_Get_Var(RX_packet.var, &RX_packet.val);
		break;
				
		case CMD_SET: 
				return WL_Set_Var(RX_packet.var, RX_packet.val);
		break;
				
		case CMD_EEPROM_WR: 	
				return PS_CMD_NOT_SUPPORTED;
		break;
				
		case CMD_ERR_CLR: 	
				return PS_CMD_NOT_SUPPORTED;			
		break;
				
		default:
			return PS_CMD_NOT_SUPPORTED;
		break;
			
			
	}
}

//void WL_Packet_Handler(void){
//	int8_t i=pack_cnt;

//	if(packet_handler_timer>200){
//		packet_handler_timer=0;		
//	
//		//printf("pack_cnt: %d\r\n",pack_cnt);
//		while(i--){
//			if(WL_DEBUG_PRINT){				
//				sprintf(str,"\r\nPack Num: %d\r\n", i+1);
//				putsUSART(str);
//				sprintf(str,"P_ID: 0x%02X\r\n", RX_packet.pack_ID);
//				putsUSART(str);		
//				sprintf(str,"STATE: 0x%02X\r\n", RX_packet.state);
//				putsUSART(str);

//	
//			}
//			
////			if(pack_info->pack_state==PS_DONE){
////				remove_packet(pack_info->pack_ID);
////			}
//			if(pack_info->pack_state==PS_NO_ACK){
//				remove_packet(pack_info->pack_ID);
//			}
//			if(pack_info->pack_state==PS_SEND_OK){
//				if(HAL_GetTick()-pack_info->time_stamp>10000){
//					remove_packet(pack_info->pack_ID);
//				}
//			}		

//			pack_info=pack_info->next;
//		}
////		if(pack_cnt>0){
////			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
////		}else{
////			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
////		}
//			
//	}

//	
//}

void WL_Handler(void){
	
	uint8_t state=0;
	
		if(WL_RECEIVE){
			WL_RECEIVE=0;	
			
			state= WL_Check_Packet();
			
			if(state==PS_ADDR_MATCH && rx_handler){		
				HAL_Delay(1000);
				
				state = WL_Run_CMD(RX_packet.cmd);
				if(WL_DEBUG_PRINT){		
						sprintf(str,"RUN CMD STATE: %s\r\n",PACK_STATE_STR[state]);
						putsUSART(str);
				}
				state = WL_Send_Packet(RX_packet.pack_ID, state ,RX_packet.src_addr, RX_packet.cmd, RX_packet.var, RX_packet.val);
				if(WL_DEBUG_PRINT){		
					sprintf(str,"SEND PACKET STATE: %s\r\n",PACK_STATE_STR[state]);
					putsUSART(str);
				}
			}	
		}		
}



