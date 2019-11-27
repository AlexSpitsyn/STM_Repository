
#include "wireless_ctrl.h"



WL_Packet TX_packet;
WL_Packet RX_packet;
char str[32];
uint16_t packet_handler_timer=0;

struct packet_buf *pack_info =NULL;

#define MAX_PACK_CNT 10
uint8_t pack_cnt=0;

uint8_t WL_Check_Addr(uint8_t base_addr){
	//0 - addr free
	//2 - addr busy
	//3 - timeout			 
	static uint8_t temp_addr=0;
	uint32_t t;	
	base_addr &=0xF0;
	
	//If PCF8754 set addr			
	PCF8574_ReadReg();
	base_addr |= PCF8574A_reg>>4;

	//If GPIO set addr
//	base_addr |= PCF8574A_reg>>4;	
//	base_addr |= HAL_GPIO_ReadPin(GPIOB,A1_Pin);
//	base_addr |= HAL_GPIO_ReadPin(GPIOB,A2_Pin)<<1;
//	base_addr |= HAL_GPIO_ReadPin(GPIOB,A3_Pin)<<2;
//	base_addr |= HAL_GPIO_ReadPin(GPIOB,A4_Pin)<<3;	
	
	NRF_RX_ADDR[0]=base_addr;
	NRF_TX_ADDR[0]=base_addr;
	
if(NRF_TX_ADDR[0]!=temp_addr){	
	NRF24_Send("Hi");	
	temp_addr=base_addr;
	t=HAL_GetTick();
	while(1){		
		if(nrf_tx_flag){
			nrf_tx_flag=0;
			return 2;
		}	
		if(nrf_tx_fail){
			nrf_tx_fail=0;
			return 0;		
		}		
		if((HAL_GetTick()-t)>2000){
			return 3;
		}			
	}
}	 
return 2;
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
				ds18b20_GetTemp();
				pack_info->val=DS_TEMP;
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_T_CTRL: 
				pack_info->val=SysState.temp_ctrl_f;
				pack_info->pack_state=PS_DONE;
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_T_SET: 
				pack_info->val=SysState.set_temp;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_T_CTRL_TIME: 
				pack_info->val=SysState.t_ctrl_time;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_T_UPDT_TIME: 
				pack_info->val=SysState.t_updt_time;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_T_HYST: 
				pack_info->val=SysState.t_hyst;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_DRV_POS: 
				pack_info->val=drv_m1.position;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_DRV_POS_MAX: 
				pack_info->val=drv_m1.max_pos;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_DRV_POS_DEST: 
				pack_info->val=drv_m1.dest_pos;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_DRV_STEPS: 
				pack_info->val=drv_m1.steps;				
				pack_info->pack_state=PS_DONE;
				break;
				
				case VAR_ERR_CODE: 
				pack_info->val=SysState.error_code;				
				pack_info->pack_state=PS_DONE;
				break;
				
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
				
				case VAR_T_CTRL: 
				if ((SV[vn_T_CTRL_F].vmin <= pack_info->val) && (pack_info->val <= SV[vn_T_CTRL_F].vmax)) {
					SysState.temp_ctrl_f=pack_info->val;
					if(SysState.temp_ctrl_f){					
						SysVarRW(WR, &SV[vn_T_CTRL_F]);
						SysCnt.temp_update = 0;
						SysCnt.temp_ctrl = 0;
						ds18b20_GetTemp();
						SS_SHOW(TEMP_VIEW);
						SysState.view_mode_f = TEMP_VIEW; 
						SysState.ss_update_f = 1; 
						SysCnt.view_mode = 0;
					}else{						            
						SysVarRW(WR, &SV[vn_T_CTRL_F]);
						SS_PRINT_T_NONE();
						SS_SHOW(TEMP_VIEW);
					}
					pack_info->pack_state=PS_DONE;	
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				
				break;
				
				case VAR_T_SET:				
				if ((SV[vn_T_SET].vmin <= pack_info->val) && (pack_info->val <= SV[vn_T_SET].vmax)) {
					SysState.set_temp=pack_info->val;
					SysVarRW(WR, &SV[vn_T_SET]);
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_T_CTRL_TIME:
				if ((SV[vn_T_CTRL_TIME].vmin <= pack_info->val) && (pack_info->val <= SV[vn_T_CTRL_TIME].vmax)) {
					SysState.t_ctrl_time=pack_info->val;
					SysVarRW(WR, &SV[vn_T_CTRL_TIME]);
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_T_UPDT_TIME:
				if ((SV[vn_T_UPDT_TIME].vmin <= pack_info->val) && (pack_info->val <= SV[vn_T_UPDT_TIME].vmax)) {				
					SysState.t_updt_time=pack_info->val;
					SysVarRW(WR, &SV[vn_T_UPDT_TIME]);
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_T_HYST:
				if ((SV[vn_T_HYST].vmin <= pack_info->val) && (pack_info->val <= SV[vn_T_HYST].vmax)) {
					SysState.t_hyst=pack_info->val;	
					SysVarRW(WR, &SV[vn_T_HYST]);				
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_DRV_POS: 
				if ((SV[vn_DRIVE_POS].vmin <= pack_info->val) && (pack_info->val <= SV[vn_DRIVE_POS].vmax)) {
					drv_m1.position=pack_info->val;
					drv_m1.dest_pos=drv_m1.position;
					SysVarRW(WR, &SV[vn_DRIVE_POS]);
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_DRV_POS_MAX: 
				if ((SV[vn_DRIVE_MAX_POS].vmin <= pack_info->val) && (pack_info->val <= SV[vn_DRIVE_MAX_POS].vmax)) {
					drv_m1.max_pos=pack_info->val;
					SysVarRW(WR, &SV[vn_DRIVE_MAX_POS]);				
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_DRV_POS_DEST: 
				if ((SV[vn_DRIVE_POS].vmin <= pack_info->val) && (pack_info->val <= SV[vn_DRIVE_POS].vmax)) {
					drv_m1.dest_pos=pack_info->val;	
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				
				case VAR_DRV_STEPS: 
				if ((SV[vn_DRIVE_STEPS].vmin <= pack_info->val) && (pack_info->val <= SV[vn_DRIVE_STEPS].vmax)) {
					drv_m1.steps=pack_info->val;
					SysVarRW(WR, &SV[vn_DRIVE_STEPS]);				
					pack_info->pack_state=PS_DONE;
				}else{
					pack_info->pack_state=PS_VAL_NOT_SUPPORTED;
				}
				break;
				

				
				default:
				pack_info->pack_state=PS_VAR_NOT_SUPPORTED;
				break;
			}
}


uint8_t WL_Send_Packet(void){

	uint32_t t;

	TX_packet.cmd=pack_info->cmd;
	TX_packet.var=pack_info->var;
	TX_packet.val=pack_info->val;
	TX_packet.state=pack_info->pack_state;
	TX_packet.pack_ID=pack_info->pack_ID;
	
	
	TX_packet.host_addr=NRF_RX_ADDR[0];
	NRF_TX_ADDR[0]=pack_info->host_addr;
	
	
	
	TX_packet.crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)&TX_packet,2);

		if(NRF_DBG_PRINT_F){
		sprintf(str,"\r\nTX Packet CMD: 0x%02X\r\n", TX_packet.cmd);
		putsUSART(str);
		sprintf(str,"\r\nTX Packet VAR: 0x%02X\r\n", TX_packet.var);
		putsUSART(str);
		sprintf(str,"\r\nTX Packet VAL: 0x%02X\r\n", TX_packet.val);
		putsUSART(str);
		sprintf(str,"\r\nTX Packet P_ID: 0x%02X\r\n", TX_packet.pack_ID);
		putsUSART(str);		
		sprintf(str,"\r\nTX Packet HOST ADDR: 0x%02X\r\n", TX_packet.host_addr);
		putsUSART(str);
		sprintf(str,"\r\nTX Packet STATE: 0x%02X\r\n", TX_packet.state);
		putsUSART(str);
		sprintf(str,"\r\nTX Packet CRC: 0x%02X\r\n", TX_packet.crc);
		putsUSART(str);

	}
	NRF24_Send((uint8_t*)&TX_packet);
	
	t=HAL_GetTick();
	while(1){
		if(nrf_tx_flag){
			nrf_tx_flag=0;			
			if(NRF_DBG_PRINT_F){
				sprintf(str,"\r\nPacket Send CRC: 0x%02X\r\n", TX_packet.crc);
				putsUSART(str);
			}
			pack_info->pack_state=PS_SEND_OK;
			break;			
		}
		if(nrf_tx_fail){
			nrf_tx_fail=0;
			pack_info->pack_state = PS_NO_ACK;
			break;
		}
		if(HAL_GetTick()-t>1000){
			return PS_ERROR;
		}
	}
	return pack_info->pack_state;
}
uint8_t WL_Get_Packet(){

	uint32_t crc;

	memcpy(	&RX_packet,NRF_RX_BUF,12);
	crc=HAL_CRC_Calculate(&hcrc,(uint32_t*)	&RX_packet,2);	
	

	if(NRF_DBG_PRINT_F){
		sprintf(str,"\r\nRecieve Packet CMD: 0x%02X\r\n", RX_packet.cmd);
		putsUSART(str);
		sprintf(str,"\r\nRecieve Packet VAR: 0x%02X\r\n", RX_packet.var);
		putsUSART(str);
		sprintf(str,"\r\nRecieve Packet VAL: 0x%02X\r\n", RX_packet.val);
		putsUSART(str);
		sprintf(str,"\r\nRecieve Packet P_ID: 0x%02X\r\n", RX_packet.pack_ID);
		putsUSART(str);		
		sprintf(str,"\r\nRecieve Packet HOST ADDR: 0x%02X\r\n", RX_packet.host_addr);
		putsUSART(str);
		sprintf(str,"\r\nRecieve Packet STATE: 0x%02X\r\n", RX_packet.state);
		putsUSART(str);
		sprintf(str,"\r\nRecieve Packet CRC: 0x%02X\r\n", RX_packet.crc);
		putsUSART(str);
		sprintf(str,"\r\nCalc CRC: 0x%02X\r\n", crc);
		putsUSART(str);
	}
	
	if(crc!=RX_packet.crc){	
		if(NRF_DBG_PRINT_F){
			putsUSART("\r\nCRC: BAD\r\n");
		}
		return PS_CRC_BAD;
	}else{
		if(NRF_DBG_PRINT_F){
			putsUSART("\r\nCRC: OK\r\n");
		}
	
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
			}else{
				return PS_NOT_FOUND;
			}
		}
		return PS_CRC_OK;
	}	
}

void WL_Packet_Handler(void){
	int8_t i=pack_cnt;

	if(packet_handler_timer>200){
		packet_handler_timer=0;
		while(i--){

			if(pack_info->pack_state==PS_NO_ACK){
				remove_packet(pack_info->pack_ID);
			}
			if(pack_info->pack_state==PS_SEND_OK){				
					remove_packet(pack_info->pack_ID);				
			}		
			if(NRF_DBG_PRINT_F){
				sprintf(str,"\r\nPack cnt: 0x%02X\r\n", i);
				putsUSART(str);
				sprintf(str,"P_ID: 0x%02X\r\n", RX_packet.pack_ID);
				putsUSART(str);		
				sprintf(str,"STATE: 0x%02X\r\n", RX_packet.state);
				putsUSART(str);

	
			}
			pack_info=pack_info->next;
		}
		if(pack_cnt>0){
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
		}else{
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
		}		 
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
							putsUSART("\r\nCMD NOT SUPPORTED\r\n");
							break;
						}								
							
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
						remove_packet(pack_info->pack_ID);	
						putsUSART("\r\nRET: Error\r\n");						
						break;
						
						case PS_DONE: 
						remove_packet(pack_info->pack_ID);	
						putsUSART("\r\nRET: Done\r\n");						
						break;

						

					}
				//}else{
				//send_req_f = 0;

				
				
				
			}else if(state==PS_CRC_BAD) {				
				//BAD CRC					
				
				
			}else if(state==PS_ERROR) {	
				putsUSART("\r\nRecieve Packet: ERROR\r\n");
			}				
		}
		WL_Packet_Handler();
		
}
