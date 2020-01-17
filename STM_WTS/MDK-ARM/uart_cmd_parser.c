#include "uart_cmd_parser.h"

#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include "stdarg.h"

#include "wireless_ctrl.h"

#include "usart.h"
#include "nrf24.h"


void uartInit(void) {  

  putsUSART("\r\n");
  putsUSART("\r\n");
  putsUSART("             |||||||||||||||\r\n");
  putsUSART("         ||||||||||||||||||||||\r\n");
  putsUSART("       ||||||||||||||||||||||||||\r\n");
  putsUSART("      ||||     |||||||||      |||||\r\n");
  putsUSART("    ||||     ||||||||||||||    |||||\r\n");
  putsUSART("   |||||  ||||||||||||||||||||  |||||\r\n");
  putsUSART("   ||||| |||||||||     |||||||| ||||||\r\n");
  putsUSART("  ||||||||||||              |||||||||||\r\n");
  putsUSART("  ||||||||||                  |||||||||\r\n");
  putsUSART(" |||||||||                      |||||||\r\n");
  putsUSART(" ||||||||                        ||||||\r\n");
  putsUSART(" |||||||                          |||||\r\n");
  putsUSART("  ||||||                          |||||\r\n");
  putsUSART("  |||||                           |||||\r\n");
  putsUSART("  |||||                           ||||\r\n");
  putsUSART("   ||||                           ||||\r\n");
  putsUSART("    ||||          |||||||||       |||\r\n");
  putsUSART("     ||||    ||  |||||||||||||   |||\r\n");
  putsUSART("       ||||||||  |||||||||||||||||\r\n");
  putsUSART("        |||||||  |||||||||||||||\r\n");
  putsUSART("           |||||||||||||||||||\r\n");
  putsUSART("               |||||||||||\r\n");
  putsUSART("                                    ||||\r\n");
  putsUSART("       ||||||    ||||||        |||||||||\r\n");
  putsUSART("      |||||||  ||||||||||    |||||||||||\r\n");
  putsUSART("     |||||    ||||||||||||  ||||||||||||\r\n");
  putsUSART("     ||||     ||||    ||||| ||||    ||||\r\n");
  putsUSART("    |||||     ||||    ||||| ||||    ||||\r\n");
  putsUSART("||||||||       ||||||||||||  |||||||||||\r\n");
  putsUSART("|||||||         |||||||||||   ||||||||||\r\n");
  putsUSART("Wireless Temp Sensor v.1\r\n> ");

}

//Commands 
//  set [VAR] [VAL]
//  get [VAR] / all
//  err print
//  err clear
//  eeprom write [VAR] / all
//  debug print
//  help
//  var list
//Variables
//  unsigned char       tctrl                   RW
//  unsigned char       tset                    RW
//  unsigned char       temp                    RO
//  unsigned char       steps                   RW
//  unsigned int        position                RO
//  unsigned int        pdest                   RW
//  unsigned int        pmax                    RW
//  unsigned int        tctrl_t                 RW
//  unsigned int        tupd_t                  RW
//  unsigned int        t_hyst                  RW

# define UVARS_CNT 10

void parseCommand(char *buf) {
  unsigned char i;
  char *pArg1 = NULL, *pArg2 = NULL, *pEnd = NULL;
  uint8_t l = 0;
  char answer[64] = "";
  
	uint8_t cmd;
	uint8_t var;
	uint16_t val;
	uint8_t addr;
	uint8_t tmp;

#define CMD_CNT 6

  char commands[CMD_CNT][16] = {							
		"help",         // 0                      
		"nrf init",     // 1
		"send",      	  // 2
		"set",       	  // 3
		"get state",		// 4
		"debug"					// 5
											
	};  

  for (i = 0; i < CMD_CNT; i++) {
    if (strncmp(buf, commands[i], strlen(commands[i])) == 0) {
      l = strlen(commands[i]);
      pArg1 = buf + l + 1;
      break;
    }
  }

  switch (i) {
    

    case 0:  // help
			
//		enum CMD{
//	CMD_PRESENT,
//	CMD_GET,
//	CMD_SET,
//	CMD_EEPROM_WR,
//	CMD_ERR_CLR
//};

//enum VAR{
//	VAR_TEMP,
//	VAR_T_CTRL,
//	VAR_T_SET,
//	VAR_T_CTRL_TIME,
//	VAR_T_UPDT_TIME,
//	VAR_T_HYST,
//	VAR_DRV_POS,
//	VAR_DRV_POS_MAX,
//	VAR_DRV_POS_DEST,
//	VAR_DRV_STEPS

//};
putsUSART(
          "\r\n"
          "help\r\n"          
          "init\r\n"	
					"send addr[0-255] cmd[0-1] var[0-1] val\r\n"					
#if (ADR_WIDTH==3)
					"set [rx_addr/tx_add] [0-255] [0-255] [0-255], [rf_ch] [0-127], [data_rate] [0-2]\r\n");
#endif	
#if (ADR_WIDTH==4)
					"set [rx_addr/tx_add] [0-255] [0-255] [0-255] [0-255], [rf_ch] [0-127], [data_rate] [0-2]\r\n");
#endif	
#if (ADR_WIDTH==5)
					"set [rx_addr/tx_add] [0-255] [0-255] [0-255] [0-255] [0-255], [rf_ch] [0-127], [data_rate] [0-2]\r\n");
#endif	          					
          
putsUSART("get state\r\n"
					"cmd 0 - present addr\r\n"
					"cmd 1 - get\r\n"
					"var 0 - temp\r\n");
      break;

    case 1:  // nrf init
			
      NRF24_Init();
      putsUSART("\r\nOK\r\n");

      break;

    case 2:  // nrf send
      if (*(pArg1) == 0) {
        putsUSART("wrong var");
      } else {
				if(strtodec(pArg1,&pEnd,(uint16_t*)&addr,0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&cmd,0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&var,0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&val,0, 0xFFFF)){
					putsUSART("wrong parameters");		
				}else{
					
					add_packet((uint16_t) HAL_GetTick(),PS_NEW,addr,cmd,var,val);
					sprintf(answer, "CMD: %d, Var: %d, Val: %d\r\n", cmd, var, val);					
					putsUSART(answer);					
					TX_packet.state=PS_NEW;
					sprintf(answer, "SEND to %d CODE: %d\r\n",addr,WL_Send_Packet());		
					putsUSART(answer);	
				}	

      }
      break;

    case 3:  // nrf set 
      if (*(pArg1) == 0) {
        putsUSART("wrong var");
      } else {
        if (strncmp(pArg1, "tx_addr", 7) == 0) {
          pArg2 = pArg1 + 8;
          if (*(pArg2) == 0) {
            putsUSART("wrong addr");
          } else {
					#if (ADR_WIDTH==3)
						if(strtodec(pArg2,&pEnd,(uint16_t*)&NRF_TX_ADDR[0],0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[1],0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[2],0, 255)){
													putsUSART("wrong addr value. [0-255]");		
						}else{
							sprintf(answer, "Set NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2]);
							putsUSART(answer);
						}
					#endif	
					#if (ADR_WIDTH==4)
						if(strtodec(pArg2,&pEnd,(uint16_t*)&NRF_TX_ADDR[0],0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[1],0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[2],0, 255)
							| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[3],0, 255)){
													putsUSART("wrong addr value. [0-255]");		
						}else{
							sprintf(answer, "Set NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X,\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2], NRF_TX_ADDR[3]);
							putsUSART(answer);
						}
					#endif	
					#if (ADR_WIDTH==5)
						if(strtodec(pArg2,&pEnd,(uint16_t*)&NRF_TX_ADDR[0],0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[1],0, 255) | strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[2],0, 255)
								| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[3],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_TX_ADDR[4],0, 255)){
												putsUSART("wrong addr value. [0-255]");		
						}else{
							sprintf(answer, "Set NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2], NRF_TX_ADDR[3], NRF_TX_ADDR[4]);
							putsUSART(answer);
						}
						#endif							
            
          }
        } else if (strncmp(pArg1, "rx_addr", 7) == 0) {
          pArg2 = pArg1 + 8;
          if (*(pArg2) == 0) {
            putsUSART("wrong addr");
          } else {
						#if (ADR_WIDTH==3)
            if(strtodec(pArg2,&pEnd,(uint16_t*)&NRF_RX_ADDR[0],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[1],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[2],0, 255)){
							putsUSART("wrong addr value. [0-255]");		
						}else{
							sprintf(answer, "Set NRF_RX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2]);
							putsUSART(answer);
						}
						#endif
						#if (ADR_WIDTH==4)
            if(strtodec(pArg2,&pEnd,(uint16_t*)&NRF_RX_ADDR[0],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[1],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[2],0, 255)
							| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[3],0, 255)){						
							putsUSART("wrong addr value. [0-255]");		
						}else{
							sprintf(answer, "Set NRF_RX_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2], NRF_TX_ADDR[3]);
							putsUSART(answer);
						}
						#endif
						#if (ADR_WIDTH==5)
            if(strtodec(pArg2,&pEnd,(uint16_t*)&NRF_RX_ADDR[0],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[1],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[2],0, 255)
								| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[3],0, 255)| strtodec(pEnd,&pEnd,(uint16_t*)&NRF_RX_ADDR[4],0, 255)){
								putsUSART("wrong addr value. [0-255]");		
						}else{
							sprintf(answer, "Set NRF_RX_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2], NRF_TX_ADDR[3], NRF_TX_ADDR[4]);
							putsUSART(answer);
						}
						#endif
				
          }
        } else if (strncmp(pArg1, "rf_ch", 5) == 0) {
          pArg2 = pArg1 + 6;
          if (*(pArg2) == 0) {
            putsUSART("wrong var");
          } else {
						
						if(strtodec(pArg2,&pEnd,(uint16_t*)&tmp,0, 127)){
							putsUSART("wrong rf_ch value. [0-127]");		
						}else{
							NRF24_WriteReg(RF_CH, tmp);
							printf(answer, "Set NRF_RF_CH: 0x%02X\r\n", tmp);
							putsUSART(answer);
						}	
          }
        }else if (strncmp(pArg1, "data_rate", 9) == 0) {
          pArg2 = pArg1 + 10;
          if (*(pArg2) == 0) {
            putsUSART("wrong var");
          } else {
						
						if(strtodec(pArg2,&pEnd,(uint16_t*)&tmp,0, 2)){
							putsUSART("wrong data_rate value. [0-2]");		
						}else{
							var=NRF24_ReadReg(RF_SETUP);
							putsUSART("Set NRF_DR: ");
							if (tmp==0){
								var|=0x20;
								var&=0xF7;
								NRF24_WriteReg(RF_SETUP, var );
								putsUSART("250KBps\r\n");
							}
							if (tmp==1){								
								var&=0xD7;
								NRF24_WriteReg(RF_SETUP, var );
								putsUSART("1MBps\r\n");
							}
							if (tmp==2){
								var|=0x08;
								var&=0xDF;
								NRF24_WriteReg(RF_SETUP, var );
								putsUSART("2MBps\r\n");
							}
						}	
          }
        }				else {
          putsUSART("wrong addr type");
        }
      }
      break;

			case 4:  // nrf get state
			putsUSART("\r\n-----  NRF STATE  ----\r\n");	
      NRF24_Get_State();
			putsUSART("\r\n-----  DS STATE  ----\r\n");
				if(ds18b20_Reset()){
					putsUSART("STATE: FAIL\r\n");
				}else{
					ds18b20_GetTemp();
					sprintf(answer, "STATE: OK\r\nTEMP: %d'C\r\n", DS_TEMP);
					putsUSART(answer);
				}
			
      break;

    	case 5:// nrf debug
    	NRF_DBG_PRINT_F^=1;
			if(NRF_DBG_PRINT_F){
				putsUSART("NRF DEBUG SET");

			}else{
				putsUSART("NRF DEBUG RESET");

			}
    	break;

    default:
      putsUSART("command unknown");
      break;
  }

  putsUSART("\r\n>");
}

uint8_t strtodec(char* str, char** end, uint16_t* val, uint16_t vmin, uint16_t vmax){
	uint16_t conv_val;			
	conv_val=strtol(str, end, 10);
	if((conv_val>=vmin)&&(conv_val<=vmax)){
		*val=conv_val;
		return 0;
		
	}else{
		return 1;
	}
}
