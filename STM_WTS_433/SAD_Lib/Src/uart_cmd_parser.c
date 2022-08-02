
#include "uart_cmd_parser.h"


void uartInit(void) {  

  print_to("\r\n");
  print_to("\r\n");
  print_to("             |||||||||||||||\r\n");
  print_to("         ||||||||||||||||||||||\r\n");
  print_to("       ||||||||||||||||||||||||||\r\n");
  print_to("      ||||     |||||||||      |||||\r\n");
  print_to("    ||||     ||||||||||||||    |||||\r\n");
  print_to("   |||||  ||||||||||||||||||||  |||||\r\n");
  print_to("   ||||| |||||||||     |||||||| ||||||\r\n");
  print_to("  ||||||||||||              |||||||||||\r\n");
  print_to("  ||||||||||                  |||||||||\r\n");
  print_to(" |||||||||                      |||||||\r\n");
  print_to(" ||||||||                        ||||||\r\n");
  print_to(" |||||||                          |||||\r\n");
  print_to("  ||||||                          |||||\r\n");
  print_to("  |||||                           |||||\r\n");
  print_to("  |||||                           ||||\r\n");
  print_to("   ||||                           ||||\r\n");
  print_to("    ||||          |||||||||       |||\r\n");
  print_to("     ||||    ||  |||||||||||||   |||\r\n");
  print_to("       ||||||||  |||||||||||||||||\r\n");
  print_to("        |||||||  |||||||||||||||\r\n");
  print_to("           |||||||||||||||||||\r\n");
  print_to("               |||||||||||\r\n");
  print_to("                                    ||||\r\n");
  print_to("       ||||||    ||||||        |||||||||\r\n");
  print_to("      |||||||  ||||||||||    |||||||||||\r\n");
  print_to("     |||||    ||||||||||||  ||||||||||||\r\n");
  print_to("     ||||     ||||    ||||| ||||    ||||\r\n");
  print_to("    |||||     ||||    ||||| ||||    ||||\r\n");
  print_to("||||||||       ||||||||||||  |||||||||||\r\n");
  print_to("|||||||         |||||||||||   ||||||||||\r\n");
  print_to(DEV_NAME);

}




#define UVARS_CNT 10
#define DEC_STR 10
#define HEX_STR 16

#define CMD_CNT 12
static const char* commands[CMD_CNT] = {							
		"help",         // 0                      
		"init",     		// 1
		"wlsend",      	// 2
		"set",       	  // 3
		"get",					// 4
		"debug",				// 5
		"wl clear irq",	// 6
		"rx handler",		// 7
		"state",				// 8
		"eeprom write",	// 9
		"eeprom restore", //10
		"reset"
											
	}; 
#define PRM_CNT 6
static const char* param[PRM_CNT] = {							
		"wl_addr",        // 0                      
		"wl_pwr",     		// 1
		"wl_bw",      	 	// 2
		"wl_sf",       	  // 3
		"wl_cr",					// 4
		"sv"							// 5
											
	}; 
void parseCommand(char *buf) {
  uint8_t i=0, l=0;
  char *pArg1 = NULL, *pArg2 = NULL, *pEnd = NULL;
  
  char answer[64] = "";

	uint32_t cmd, var, val, addr, tmp;

	
	
	// remove spaces

	remove_spaces(buf);

	i=strlen(buf);
	if(buf[i-1] == ' ')
			buf[i-1]  = '\0';
		if(buf[0] == ' ')
			buf  = buf+1;
		

	
	//search cmd
  for (i = 0; i < CMD_CNT; i++) {
    if (strncmp(buf, commands[i], strlen(commands[i])) == 0) {
      l = strlen(commands[i]);
      pArg1 = buf + l + 1;
      break;
    }
	}

  switch (i) {//swith command
    
//==============================HELP==================================
    case 0:  // help
		if (*(pArg1-1) == 0) {
    
			print_to("help [cmd]\r\n");
			 
			for (i = 1; i < CMD_CNT; i++) {
				print_to(commands[i]);
				print_to("\r\n");	
			}
    }else{
			for (i = 1; i < CMD_CNT; i++) {
				if (strncmp(pArg1, commands[i], strlen(commands[i])) == 0) {					
					break;
				}				
			}
			switch (i){//swith command for help
//		"help",         // 0                      
//		"init",     		// 1
//		"wlsend",      	// 2
//		"set",       	  // 3
//		"get",					// 4
//		"debug",				// 5
//		"clear irq",		// 6
//		"rx handler",		// 7
//		"state"					// 8
				case 2:  // wlsend
					print_to(
						"send addr(0x00000000-0xFFFFFFFF) cmd(0-3) var val\r\n"
						"cmd: 0 - check addr\r\n"
						"\t 1 - get\r\n"
						"\t 2 - set\r\n"
						"\t 3 - eeprom write\r\n"
						//"var 0 - temp\r\n"
					);
				break;
				
				case 3:  // set
					print_to(
					"set [var] [val]\r\n"
					"vars: "
					);
					for (i = 0; i < PRM_CNT; i++) {
						print_to("\t");
						print_to(param[i]);
						print_to("\r\n");
					}	
					for (i = 0; i < SYS_VAR_CNT; i++){
						print_to("\t");
						print_to(SV[i].name);
						print_to("\r\n");
					}
					break;
					
				case 4:  // get
					print_to(
					"get [var]\r\n"
					"vars: "
					);
					for (i = 0; i < PRM_CNT; i++) {
						print_to("\t");
						print_to(param[i]);
						print_to("\r\n");
					}	
					for (i = 0; i < SYS_VAR_CNT; i++) {
						print_to("\t");
						print_to(SV[i].name);
						print_to("\r\n");
					}	
					break;
					
				case 5://debug
					print_to("debug [sx / wl]\r\n");
				break;
				
				case 7://rx handler 
					print_to("rx handler [on / off]\r\n");
				break;
				default:					
					print_to("no help for command\r\n");
				break;
				
			}//end swith command for help
		}
		break;
//========================================================================    
//==============================  INIT  ==================================
//========================================================================   
    case 1:  
			
      SX1278_defaultConfig();
			WL_Init();
			print_to("\r\nSX1278 init: OK\r\n");
      break;
//========================================================================		
//============================  WL SEND  =================================
//========================================================================   
    case 2:  // wlsend				
      if (*(pArg1) == 0) {
        print_to("wrong var");
      } else {
				if(str_to_int32( pArg1,&pEnd,HEX_STR,&addr,0x00000000, 0xFFFFFFFF) | str_to_int32(pEnd,&pEnd,DEC_STR,&cmd,0, 0xFF) | str_to_int32(pEnd,&pEnd,DEC_STR,&var,0, 0xFF) | str_to_int32(pEnd,&pEnd,DEC_STR,&val,0, 0xFFFF)){
					print_to("wrong parameters");		
				}else{									
					sprintf(answer, "SEND CODE: %s",PACK_STATE_STR[WL_Send_Packet(0, PS_NEW, addr, (uint8_t)cmd,  (uint8_t)var,  (uint16_t)val, 0)]);		
					print_to(answer);	
				}	

      }
      
			break;
//========================================================================   
//============================== SET VAR =================================
//========================================================================   
    case 3:  // set 
			if (*(pArg1) != 0) {  
				
				for (i = 0; i < PRM_CNT; i++) {
					if (strncmp(pArg1, param[i], strlen(param[i])) == 0) {
						l = strlen(param[i]);
						pArg2 = pArg1 + l + 1;
						break;
					}
				}
				if (pArg2 != 0) {
					if(*(pArg2) == 0) {
            print_to("value not set");
						break;
					} 					
					switch (i){
						//addr [0x00000000-0xFFFFFFFF] / pwr [0-3] / bw [0-9] / sf [6-12] / cr [0-4]\r\n");
						case 0://addr
							if(str_to_int32(pArg2,&pEnd,HEX_STR,&tmp,0x00000000, 0xFFFFFFFF)){
								print_to("Wrong WL ADDR value [0x00000000-0xFFFFFFFF]");		
							}else{
								WL_ADDR.Val = tmp;
								sprintf(answer, "Set WL ADDR: 0x%08X / %s", WL_ADDR.Val, WL_ADDR.S);
								print_to(answer);
							}								
							break;
							
						case 1://power
							if(str_to_int32(pArg2,&pEnd,DEC_STR,&tmp,0, 3)){
								print_to("Wrong POWER value [0-3]");		
							}else{
								SX1278.POWER = (uint8_t)tmp;
								sprintf(answer, "Set SX1278 POWER: %s dBm", SX1278_Power_s[(uint8_t)tmp]);
								print_to(answer);
							}
							SX1278_defaultConfig();
							SX1278_init(SX1278_POWER_17DBM, SX1278_LORA_SF_8, SX1278_LORA_BW_20_8KHZ, SX1278_CR_4_5 , WL_PLOAD_WIDTH);
							SX1278_LoRaEntryRx( WL_PLOAD_WIDTH, 2000); 
							break;
							
						case 2://bw
							if(str_to_int32(pArg2,&pEnd,DEC_STR,&tmp,0, 9)){
								print_to("Wrong Bandwidth value [0-9]");		
							}else{
								SX1278.BW = (uint8_t)tmp;
								sprintf(answer, "Set SX1278 Bandwidth: %s kHz", SX1278_LoRaBandwidth_s[SX1278.BW]);
								print_to(answer);
							}
							//SX1278_defaultConfig();
							SX1278_init(SX1278_POWER_17DBM, SX1278_LORA_SF_8, SX1278.BW, SX1278_CR_4_5 , WL_PLOAD_WIDTH);
							SX1278_LoRaEntryRx( WL_PLOAD_WIDTH, 2000); 	
							break;
							
						case 3://sf
							
							if(str_to_int32(pArg2,&pEnd,DEC_STR,&tmp,6, 12)){
								print_to("Wrong SF value [6-12]");		
							}else{
								SX1278.SF = (uint8_t)tmp-6;
								sprintf(answer, "Set SX1278 SF: %d", SX1278_SpreadFactor[SX1278.SF]);
								print_to(answer);
							}	
							//SX1278_defaultConfig();
							SX1278_init(SX1278_POWER_17DBM, SX1278.SF, SX1278_LORA_BW_20_8KHZ, SX1278_CR_4_5 , WL_PLOAD_WIDTH);
							SX1278_LoRaEntryRx( WL_PLOAD_WIDTH, 2000); 					
							break;
							
						case 4://cr
							if(str_to_int32(pArg2,&pEnd,DEC_STR,&tmp,0, 3)){
								print_to("Wrong Coding Rate value [0-3]");		
							}else{
								SX1278.CR = (uint8_t)tmp+1;
								sprintf(answer, "Set SX1278 Coding Rate: %s", SX1278_CR_s[SX1278.CR]);
								print_to(answer);
							}
							//SX1278_defaultConfig();
							SX1278_init(SX1278_POWER_17DBM, SX1278_LORA_SF_8, SX1278_LORA_BW_20_8KHZ, SX1278.CR , WL_PLOAD_WIDTH);
							SX1278_LoRaEntryRx( WL_PLOAD_WIDTH, 2000); 
							break;		
							
						default:
							print_to("wrong param type");
							break;
						
					}	
					print_to("\r\n");	
					
				
				}else {
					for (i = 0; i < SYS_VAR_CNT; i++) {
							if (strncmp(pArg1, SV[i].name, strlen(SV[i].name)) == 0) {
								l = strlen(SV[i].name);
								pArg2 = pArg1 + l + 1;
								break;
							}
						}
						if (pArg2 != 0) {
							if(str_to_int32(pArg2,&pEnd, DEC_STR, &val, SV[i].vmin, SV[i].vmax)){
								sprintf(answer, "Wrong %s value [%d-%d]",  SV[i].name, SV[i].vmin, SV[i].vmax);
								print_to(answer);
								
							}else{
								if(SV[i].we){									
										*SV[i].pVal = (uint16_t)val;
										sprintf(answer, "Set %s = %d OK", SV[i].name, *SV[i].pVal);
										print_to(answer);										
								}else{
									print_to("Sys var write protect");	
								}									
							}							
						}else{
							print_to("var not found");
						}
				}
			    
			} else {
				print_to("var not set");
			}
			break;       
//===========================================================================			
//==============================  GET VAR  ==================================
//===========================================================================   
			case 4://get
				if (*(pArg1) != 0){
					l=0;
					for (i = 0; i < PRM_CNT; i++) {
						if (strncmp(pArg1, param[i], strlen(param[i])) == 0) {
							l = 1;
							break;
						}
					}						
					if(l){
						switch (i){							
							case 0://addr		
									sprintf(answer, "WL ADDR: 0x%08X / %s", WL_ADDR.Val, WL_ADDR.S);
									print_to(answer);								
								break;
								
							case 1://power
									sprintf(answer, "SX1278 POWER: %s dBm", SX1278_Power_s[SX1278.POWER]);
									print_to(answer);								
								break;
								
							case 2://bw
									sprintf(answer, "Set SX1278 Bandwidth: %s kHz", SX1278_LoRaBandwidth_s[SX1278.BW]);
									print_to(answer);
								break;
								
							case 3://sf	
									sprintf(answer, "SX1278 SF: %d", SX1278_SpreadFactor[SX1278.SF]);
									print_to(answer);								
								break;
								
							case 4://cr									
									sprintf(answer, "SX1278 Coding Rate: %s", SX1278_CR_s[SX1278.CR]);
									print_to(answer);							
								break;
							
							case 5://all sv
								for (i = 0; i < SYS_VAR_CNT; i++) {
									sprintf(answer, "%s = %d\r\n", SV[i].name, *SV[i].pVal);
									print_to(answer);	
								}
							break;
								
							default:
								print_to("wrong param type");
							break;					
						}		
					}else {						
						for (i = 0; i < SYS_VAR_CNT; i++) {
							if (strncmp(pArg1, SV[i].name, strlen(SV[i].name)) == 0) {
								l = 1;
								break;
							}
						}
						if(l){
							sprintf(answer, "%s = %d", SV[i].name, *SV[i].pVal);
						print_to(answer);	
						}else{
							sprintf(answer, "Wrong var: %s",  pArg1);
							print_to(answer);	
						}
					}  	
				}else{
					print_to("var not set");
				}
				
			break;
//===========================================================================			
//==============================  DEBUG    ==================================	
//===========================================================================			
    	case 5:// debug
				if (*(pArg1) == 0) {
					print_to("wrong var");
				} else {
					if (strncmp(pArg1,"wl", 2) == 0) {
						WL_DEBUG_PRINT^=1;
						if(WL_DEBUG_PRINT){
							print_to("DEBUG SET");
						}else{
							print_to("DEBUG RESET");
						}							
					}else if(strncmp(pArg1,"sx", 2)==0) {
						SX1278_DBG_PRINT_F^=1;
						if(SX1278_DBG_PRINT_F){
							print_to("SX1278 DEBUG SET");
						}else{
							print_to("SX1278 DEBUG RESET");
						}
					}else{
						print_to("wrong command");						
					}
      }
				break;
//===========================================================================			
//==============================  CLEAR IRQ  ================================
//===========================================================================			
			case 6: //clear IRQ
				SX1278_clearIrq();
				print_to("IRQ Clear: OK");
				break;				
//===========================================================================			
//==============================  RX HANDLER ON/OFF =========================
//===========================================================================			
			case 7://rx handler
				if (*(pArg1) == 0) {
					print_to("wrong cmd param");
				} else {
					if (strncmp(pArg1,"on", 2) == 0) {
						rx_handler=1;
						if(WL_DEBUG_PRINT){
							print_to("rx handler SET");
						}						
					}else if(strncmp(pArg1,"off", 3)==0) {
						rx_handler=0;
						if(WL_DEBUG_PRINT){
							print_to("rx handler RESET");
						}
					}else{
						print_to("wrong command");						
					}
      }
				break;
//===========================================================================			
//==============================  STATE    ==================================
//===========================================================================			
			case 8:  //state		
				sprintf(answer, "ERROR CODE: 0x%08X\r\n", SysState.error_code);
				print_to(answer);	
				print_to("\r\n-----  WL STATE  ----\r\n");	
				sprintf(answer, "WL ADDR: 0x%08X / %s", WL_ADDR.Val, WL_ADDR.S);
				print_to(answer);
				print_to("\r\n-----  SX1278 STATE  ----\r\n");	
				SX1278_Get_State();
				print_to("\r\n-----  DS STATE  ----\r\n");
				if(ow_Reset()){
					print_to("STATE: FAIL");
				}else{
					ds18b20_GetTemp(0);
					sprintf(answer, "STATE: OK\r\nTEMP: %d'C", DS18B20_TEMP);
					print_to(answer);
				}	
					
				
			break;
//===========================================================================			
//==============================  WRITE EEPROM  =============================
//===========================================================================				
			case 9:  //eeprom			
				tmp=0;
				for (i = 0; i < SYS_VAR_CNT; i++) {	
					if (SV[i].mem_addr != 0) {						
						if(SysVarRW(WR, &SV[i])){
							tmp=1;
							break;
						}
						HAL_Delay(50);
					}
				}
				if(tmp){
					print_to("SV EEPROM WRITE: FAIL\r\n");
				}else{
					print_to("SV EEPROM WRITE: DONE\r\n");
				}
				HAL_Delay(100);
				
				if(EEPROM_Write(EEPROM_WL_ADDR, WL_ADR_WIDTH, (uint8_t*)WL_ADDR.S)){
					print_to("WL_ADDR EEPROM WRITE: FAIL\r\n");
				}else{
					print_to("WL_ADDR EEPROM WRITE: DONE\r\n");
				}
			
			break;
//===========================================================================			
//==============================  WRITE RESTORE  =============================
//===========================================================================				
			case 10:  //eeprom	
				if (EEPROM_Write(0, strlen((char*)eeprom_dump), eeprom_dump)) {						
					print_to("EEPROM RESTORE: FAIL\r\n");
				}else{
					print_to("EEPROM RESTORE: DONE\r\n");
				}
			break;		

//===========================================================================			
//==============================  WRITE RESTORE  =============================
//===========================================================================				
			case 11:  //reset
				print_to("SYSTEM RESET\r\n");
				HAL_NVIC_SystemReset();

			break;		
    
			default:
				print_to("command unknown");
				break;
  }
	print_to("\r\n>");
	
}

uint8_t str_to_int32(char* str, char** end, uint8_t str_format, uint32_t* val, uint32_t vmin, uint32_t vmax){
	uint32_t conv_val;			
	conv_val=strtol(str, end, str_format);
	if((conv_val>=vmin)&&(conv_val<=vmax)){
		*val=conv_val;
		return 0;
		
	}else{
		return 1;
	}
}

void remove_spaces(char *str){	
    char *dest = str;  
 
    while (*str != '\0'){  
			while (*str == ' ' && *(str + 1) == ' ') {
				str++;  
			}
			*dest++ = *str++;
    }    
    *dest = '\0';		
}
