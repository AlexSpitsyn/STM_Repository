
#include "wireless_ctrl.h"



WL_Packet TX_packet;
WL_Packet RX_packet;
char str[32];
uint16_t packet_handler_timer=0;
uint16_t ds_check_timer=0;

uint8_t nrf_init_state=0;
uint8_t ds_init_state=0;

struct packet_buf *pack_info =NULL;


#define MAX_PACK_CNT 10
uint8_t pack_cnt=0;



uint8_t WL_Check_Addr(uint8_t base_addr){
	//0 - addr free READY
	//1 - init fail
	//2 - addr busy
	//3 - timeout	
	
	uint8_t temp_addr=NRF_TX_ADDR[ADR_WIDTH-1];
	uint32_t t;

	base_addr &=0xF0;
	
	//If PCF8754 set addr
	//PCF8574_ReadReg();
	//base_addr |= PCF8574A_reg>>4;
	
	//If GPIO set addr	
	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A1_Pin);
	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A2_Pin)<<1;
	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A3_Pin)<<2;
	base_addr |= !HAL_GPIO_ReadPin(GPIOB,A4_Pin)<<3;
	base_addr+=48;
	
	//NRF_RX_ADDR[ADR_WIDTH-1]=base_addr;
	NRF_TX_ADDR[ADR_WIDTH-1]=base_addr;

//if(NRF_TX_ADDR[ADR_WIDTH-1]!=temp_addr){	
	NRF24_Send("HI");	
	//temp_addr=base_addr;
	t=HAL_GetTick();
	while(1){		
		if(nrf_tx_flag){
			nrf_tx_flag=0;
			putsUSART("NRF: ADDR BUSY\r\n>");
			return 2;
		}	
		if(nrf_tx_fail){
			nrf_tx_fail=0;
			NRF_RX_ADDR[ADR_WIDTH-1]=base_addr;
			NRF_TX_ADDR[ADR_WIDTH-1]=temp_addr;
			NRF24_Init();
			//sprintf(str,"NRF SET TX_ADDR[2]: 0x%02X\r\n", NRF_TX_ADDR[ADR_WIDTH-1]);
			//putsUSART(str);
			return 0;		
		}		
		if((HAL_GetTick()-t)>2000){
			putsUSART("NRF: ADDR TIMEOUT\r\n>");
			return 3;
		}			
	}
//}

//return 2;

}

void add_packet(uint16_t p_id, uint8_t p_state, uint8_t p_host_addr, uint8_t p_cmd, uint8_t p_var, uint8_t p_val){
  struct packet_buf *temp;
  if(pack_cnt==0){
		pack_info=(struct packet_buf*)malloc(sizeof(struct packet_buf));
		pack_info->next=pack_info;	
	}else{
		temp = (struct packet_buf*)malloc(sizeof(struct packet_buf));
		temp->next = pack_info->next; 
		pack_info->next=temp;
		pack_info=pack_info->next;		
	}
	
	pack_info->pack_state=p_state;
	pack_info->host_addr=p_host_addr;
	pack_info->cmd=p_cmd;
	pack_info->var=p_var;
	pack_info->val=p_val;		
	pack_info->time_stamp=HAL_GetTick();
	pack_info->pack_ID=p_id;
	pack_cnt++;
	
		if(NRF_DBG_PRINT_F){
		sprintf(str,"\r\nAdd Packet ID: 0x%02X\r\n", p_id);
		putsUSART(str);
		}			
	
	return;
}

uint8_t get_packet( uint16_t p_id){

	uint8_t c = pack_cnt;
	while(c--){
		if(pack_info->pack_ID==p_id){
			return 1;
		}
			
		pack_info=pack_info->next;
	}
	return 0;  
}

uint8_t remove_packet( uint16_t p_id){
  struct packet_buf *temp;
	uint8_t c = pack_cnt;
	temp=pack_info;
  if(get_packet(p_id)){
		if(pack_cnt==1){
			free(pack_info);
			pack_info=NULL;
			pack_cnt--;
					
			if(NRF_DBG_PRINT_F){
				sprintf(str,"\r\nRemove Packet ID: 0x%02X\r\n", p_id);
				putsUSART(str);
			}	
			return 1;
		}else{
			while (c--){ 
				if(temp->next==pack_info){
					temp->next = pack_info->next;
					free(pack_info);
					pack_info=temp;
					pack_cnt--;
					if(NRF_DBG_PRINT_F){
						sprintf(str,"\r\nRemove Packet ID: 0x%02X\r\n", p_id);
						putsUSART(str);
					}
					return 1;
				}
				temp = temp->next;
			}
		}		
		return 0;		
	}else{
		return 0;
	}
	
	
}



void WL_Get_Var(void){
	
	switch (pack_info->var){
				case VAR_TEMP: 
					if(ds_init_state){
						pack_info->pack_state=PS_ERROR;
					}else{
						ds18b20_GetTemp();
						pack_info->val=DS_TEMP;				
						pack_info->pack_state=PS_DONE;
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
				pack_info->val=0;				
				pack_info->pack_state=PS_VAR_NOT_SUPPORTED;
				break;
			}
}

void WL_Set_Var(void){
	
	switch (pack_info->var){
				
				case VAR_TEMP: 		
				pack_info->pack_state=PS_VAR_NOT_SUPPORTED;
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
							
				pack_info->pack_state=PS_VAR_NOT_SUPPORTED;
				break;
			}
}
//Retutned
//	PS_NEW,									//0
//	PS_DONE,								//1
//	PS_CMD_NOT_SUPPORTED,		//2
//	PS_VAR_NOT_SUPPORTED,		//3
//	PS_VAL_NOT_SUPPORTED,		//4
//	PS_SEND_OK,							//5
//	PS_NO_ACK,							//6
//	PS_NOT_FOUND,						//7
//	PS_CRC_OK,							//8
//	PS_CRC_BAD,							//9
//	PS_ERROR		

uint8_t WL_Send_Packet(void){	
	uint32_t t;

	TX_packet.cmd=pack_info->cmd;
	TX_packet.var=pack_info->var;
	TX_packet.val=pack_info->val;
	TX_packet.state=pack_info->pack_state;
	TX_packet.pack_ID=pack_info->pack_ID;
	
	
	TX_packet.host_addr=NRF_RX_ADDR[ADR_WIDTH-1];
	NRF_TX_ADDR[ADR_WIDTH-1]=pack_info->host_addr;
	

	TX_packet.crc=HW_CRC32((uint8_t*)&TX_packet, 8, 0xFFFFFFFF);
	//TX_packet.crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)&TX_packet,2);

		if(NRF_DBG_PRINT_F){
		putsUSART("\r\nPacket Send to:");		
				#if (ADR_WIDTH==3)
			sprintf(str,"0x%02X%02X%02X\r\n", NRF_TX_ADDR[0],NRF_TX_ADDR[1],NRF_TX_ADDR[2]);
		#endif
		#if (ADR_WIDTH==4)
			sprintf(str,"0x%02X%02X%02X%02X\r\n", NRF_TX_ADDR[0],NRF_TX_ADDR[1],NRF_TX_ADDR[2],NRF_TX_ADDR[3]);
		#endif
		#if (ADR_WIDTH==5)
			sprintf(str,"0x%02X%02X%02X%02X%02X\r\n", NRF_TX_ADDR[0],NRF_TX_ADDR[1],NRF_TX_ADDR[2],NRF_TX_ADDR[3],NRF_TX_ADDR[4]);
		#endif
		putsUSART(str);	
			
		sprintf(str,"TX Packet CMD: 0x%02X\r\n", TX_packet.cmd);
		putsUSART(str);
		sprintf(str,"TX Packet VAR: 0x%02X\r\n", TX_packet.var);
		putsUSART(str);
		sprintf(str,"TX Packet VAL: 0x%02X\r\n", TX_packet.val);
		putsUSART(str);
		sprintf(str,"TX Packet P_ID: 0x%02X\r\n", TX_packet.pack_ID);
		putsUSART(str);	
		sprintf(str,"TX Packet HOST ADDR: 0x%02X\r\n", TX_packet.host_addr);
		putsUSART(str);	
		sprintf(str,"TX Packet STATE: 0x%02X\r\n", TX_packet.state);
		putsUSART(str);
		sprintf(str,"TX Packet CRC: 0x%02X\r\n", TX_packet.crc);
		putsUSART(str);

	}
	NRF24_Send((uint8_t*)&TX_packet);
	t=HAL_GetTick();
	while(1){
		if(nrf_tx_flag){
			nrf_tx_flag=0;		
			pack_info->pack_state=PS_SEND_OK;
			if(NRF_DBG_PRINT_F){				
				putsUSART("\r\nPacket State: PS_SEND_OK\r\n");				
			}			
			break;
		}
		if(nrf_tx_fail){
			nrf_tx_fail=0;
			pack_info->pack_state = PS_NO_ACK;
			if(NRF_DBG_PRINT_F){				
				putsUSART("\r\nPacket State: PS_NO_ACK\r\n");	
			}			
			break;
		}

		if((HAL_GetTick()-t)>1000){
			if(NRF_DBG_PRINT_F){				
				putsUSART("\r\nSend timeout! Packet State: PS_ERROR\r\n");	
			}	
			return PS_ERROR;
		}
	}
	return pack_info->pack_state;
}





uint8_t WL_Get_Packet(){

	uint32_t crc;

	memcpy(	&RX_packet,NRF_RX_BUF,12);
	crc=HW_CRC32((uint8_t*)&RX_packet, 8, 0xFFFFFFFF);
	//crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)	&RX_packet,2);	
	

	if(NRF_DBG_PRINT_F){
		sprintf(str,"Recieve Packet CMD: 0x%02X\r\n", RX_packet.cmd);
		putsUSART(str);
		sprintf(str,"Recieve Packet VAR: 0x%02X\r\n", RX_packet.var);
		putsUSART(str);
		sprintf(str,"Recieve Packet VAL: 0x%02X\r\n", RX_packet.val);
		putsUSART(str);
		sprintf(str,"Recieve Packet P_ID: 0x%02X\r\n", RX_packet.pack_ID);
		putsUSART(str);		
		sprintf(str,"Recieve Packet HOST ADDR: 0x%02X\r\n", RX_packet.host_addr);
		putsUSART(str);
		sprintf(str,"Recieve Packet STATE: 0x%02X\r\n", RX_packet.state);
		putsUSART(str);
		sprintf(str,"Recieve Packet CRC: 0x%02X\r\n", RX_packet.crc);
		putsUSART(str);
		sprintf(str,"Calc CRC: 0x%02X\r\n", crc);
		putsUSART(str);
	}
	
	if(crc!=RX_packet.crc){	
		putsUSART("CRC: BAD\r\n");
		return PS_CRC_BAD;
	}else{
		putsUSART("CRC: OK\r\n");
		//pack_info->pack_state= PS_CRC_OK;
		
		
			
		if(RX_packet.state==PS_NEW){					
			if(pack_cnt<MAX_PACK_CNT){
				add_packet(RX_packet.pack_ID,RX_packet.state,RX_packet.host_addr,RX_packet.cmd,RX_packet.var,RX_packet.val);
				return PS_CRC_OK;
			}else{
				return PS_ERROR;
			}
				
		}else{
			if(get_packet(RX_packet.pack_ID)){
				pack_info->pack_state=RX_packet.state;
				
				return PS_CRC_OK;
			}else{
				return PS_NOT_FOUND;
			}
		}
	}	
}

void WL_Packet_Handler(void){
	int8_t i=pack_cnt;

	if(packet_handler_timer>200){
		packet_handler_timer=0;		
	
		//printf("pack_cnt: %d\r\n",pack_cnt);
		while(i--){
			if(NRF_DBG_PRINT_F){				
				sprintf(str,"\r\nPack Num: %d\r\n", i+1);
				putsUSART(str);
				sprintf(str,"P_ID: 0x%02X\r\n", RX_packet.pack_ID);
				putsUSART(str);		
				sprintf(str,"STATE: 0x%02X\r\n", RX_packet.state);
				putsUSART(str);

	
			}
			
//			if(pack_info->pack_state==PS_DONE){
//				remove_packet(pack_info->pack_ID);
//			}
			if(pack_info->pack_state==PS_NO_ACK){
				remove_packet(pack_info->pack_ID);
			}
			if(pack_info->pack_state==PS_SEND_OK){
				if(HAL_GetTick()-pack_info->time_stamp>10000){
					remove_packet(pack_info->pack_ID);
				}
			}		

			pack_info=pack_info->next;
		}
//		if(pack_cnt>0){
//			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
//		}else{
//			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
//		}
			
	}

	
}

void WL_Handler(void){
	
	uint8_t state=0;
	
		if(nrf_rx_flag){
			nrf_rx_flag=0;	
			state= WL_Get_Packet();
			
			if(state==PS_CRC_OK){
					
					switch (pack_info->pack_state){						
						
						case PS_NEW:
						putsUSART("\r\nRSP: OK\r\n");
						sprintf(str,"CMD:%d, VAR:%d, VAL: %d",pack_info->cmd, pack_info->var, pack_info->val);
						putsUSART(str);
											
						switch (pack_info->cmd){	
							case CMD_GET: 
							WL_Get_Var();
							break;
										
							case CMD_SET: 
							WL_Set_Var();
							break;
							
							case CMD_PRESENT:								
							pack_info->pack_state=PS_DONE;
							break;
														

							default:
							pack_info->pack_state=PS_CMD_NOT_SUPPORTED;
							break;
						}							
						sprintf(str,"\r\nSend answer state: %d\r\n",WL_Send_Packet());						
						putsUSART(str);
				
						
						break;
						
						case PS_VAL_NOT_SUPPORTED: 
						putsUSART("\r\nRET: Val not supported\r\n");
						pack_info->pack_state=PS_DONE;
						WL_Send_Packet();
						break;
						
						case PS_VAR_NOT_SUPPORTED: 
						putsUSART("\r\nRET: Var not supported\r\n");
						pack_info->pack_state=PS_DONE;
						WL_Send_Packet();
						break;
						
						case PS_CMD_NOT_SUPPORTED: 
						putsUSART("\r\nRET: Cmd not supported\r\n");
						pack_info->pack_state=PS_DONE;
						WL_Send_Packet();
						break;
						
						case PS_ERROR: 
						//pack_info->pack_state=PS_DONE;
						//WL_Send_Packet();
						remove_packet(pack_info->pack_ID);	
						putsUSART("\r\nRET: Error\r\n");						
						break;						

						
						case PS_DONE: 
						//pack_info->pack_state=PS_DONE;
						//WL_Send_Packet();
						remove_packet(pack_info->pack_ID);	
						putsUSART("\r\nRET: Done\r\n");						
						break;

						

					}
				//}else{
				//send_req_f = 0;

				
				
				
			}else if(state==PS_CRC_BAD) {					
				//BAD CRC					
				putsUSART("\r\nRecieve Packet: CRC BAD\r\n");
				
			}else if(state==PS_ERROR) {	
				putsUSART("\r\nRecieve Packet: ERROR\r\n");
			}				
		}
		WL_Packet_Handler();
		
}



