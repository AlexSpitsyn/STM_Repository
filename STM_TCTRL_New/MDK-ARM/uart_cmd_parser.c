#include "uart_cmd_parser.h"

#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include "stdarg.h"

#include "system.h"

#include "usart.h"

#include "nrf24.h"
uint8_t strtodec(char * str, char ** end, uint16_t * val, uint16_t vmin, uint16_t vmax) {
  uint16_t conv_val;
  conv_val = strtol(str, end, 10);
  if ((conv_val >= vmin) && (conv_val <= vmax)) {
    * val = conv_val;
    return 0;

  } else {
    return 1;
  }
}

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
  putsUSART("Temp controller ver.4\r\n> ");

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

void parseCommand(void) {
  unsigned char i;
  char * pArg1 = NULL, * pArg2 = NULL, * pEnd = NULL;
  uint8_t l = 0;
  char answer[64] = "";
  int16_t tVal;
  uint8_t cmd;
  uint8_t
  var;
  uint16_t val;
  uint8_t addr;
	
	#define CMD_CNT 13

  char commands[CMD_CNT][16] = {
    "set", // 0
    "get", // 1
    "err print", // 2
    "err clear", // 3
    "eeprom write", // 4
    "debug print", // 5
    "var list", // 6
    "help", // 7
    "nrf init", // 8
    "nrf send", // 9
    "nrf set", // 10
    "nrf get state", //11
    "nrf debug" //12
  };

  for (i = 0; i < CMD_CNT; i++) {
    if (strncmp(uart_rx_buf, commands[i], strlen(commands[i])) == 0) {
      l = strlen(commands[i]);
      pArg1 = uart_rx_buf + l + 1;
      break;
    }
  }

  switch (i) {
  case 0: // set variable

    if ( * (pArg1) == 0) {
      putsUSART("wrong var");
    } else {
      pArg2 = strchr(pArg1, 0x20); // VAL
      if (pArg2 == NULL | * (pArg2 + 1) == 0) {
        putsUSART("wrong var value");
      } else {
        * pArg2 = 0;
        pArg2++;

        for (i = 0; i < UVARS_CNT; i++) {
          if (strcmp(pArg1, SV[i].name) == 0) {
            if (SV[i].we) {
              tVal = (uint16_t) atoi(pArg2);

              if ((SV[i].vmin <= tVal) && (tVal <= SV[i].vmax)) {
                * SV[i].pVal = tVal;
                if (i == vn_DRIVE_POS) {
                  drv_m1.dest_pos = drv_m1.position;
                }
                putsUSART("Set OK");
              } else {
                sprintf(answer, "wrong value\r\n %s min=%d, max=%d\r\n",
                  SV[i].name, SV[i].vmin, SV[i].vmax);
                putsUSART(answer);
              }
            } else {
              putsUSART("RO variable");
            }
            break;
          } else if (i == UVARS_CNT - 1) {
            putsUSART("var not found");
          }
        }
      }
    }
    break;

  case 1: // get variable
    if ( * (pArg1) == 0) {
      putsUSART("wrong var");
    } else {
      if (strcmp(pArg1, "all") == 0) {
        for (i = 0; i < UVARS_CNT; i++) {
          sprintf(answer, "%s = %d\r\n", SV[i].name, * SV[i].pVal);
          putsUSART(answer);
        }
      } else {
        for (i = 0; i < UVARS_CNT; i++) {
          if (strcmp(pArg1, SV[i].name) == 0) {
            sprintf(answer, "%s = %d\r\n", SV[i].name, * SV[i].pVal);
            putsUSART(answer);
            break;
          } else if (i == UVARS_CNT - 1)
            putsUSART("command not found");
        }
      }
    }
    break;

  case 2: // err print
    if (SysState.error_code & TEMP_SENSOR_READING_ERROR) {
      putsUSART("Temp sensor broken\r\n");
    }
    if (SysState.error_code & DRV_CLOSE_ERROR) {
      putsUSART("Cant close moto\r\n");
    }
    if (SysState.error_code & DRV_OPEN_ERROR) {
      putsUSART("Cant open moto\r\n");
    }
    break;

  case 3: // err clear
    SysState.error_code = 0;
    SysState.error_f = 0;
    putsUSART("Done\r\n");
    break;

  case 4: // eeprom write variable
    if ( * (pArg1) == 0) {
      putsUSART("wrong var");
    } else {
      if (strcmp(pArg1, "all") == 0) {
        for (i = 0; i < UVARS_CNT; i++) {
          if (SV[i].mem_addr != 0) {
            SysVarRW(WR, & SV[i]);
            sprintf(answer, "EEPROM %s <= %d OK\r\n", SV[i].name,
              * SV[i].pVal);
            putsUSART(answer);
            HAL_Delay(5);
          }
        }
      } else {
        for (i = 0; i < UVARS_CNT; i++) {
          if (strcmp(pArg1, SV[i].name) == 0) {
            if (SV[i].mem_addr != 0) {
              SysVarRW(WR, & SV[i]);

              sprintf(answer, "EEPROM %s <= %d OK\r\n", SV[i].name,
                * SV[i].pVal);
              putsUSART(answer);
              break;
            } else {
              putsUSART("var is RO");
            }
          } else if (i == UVARS_CNT) {
            putsUSART("var not found");
          }
        }
      }
    }
    break;

  case 5: // debug print
    SysState.uart_dbg_f = 1;
    SysState.uart_en_f = 0;
    SysState.ss_update_f = 1;
    break;

  case 6: // var list
    for (i = 0; i < UVARS_CNT; i++) {
      sprintf(answer, "%s\r\n", SV[i].name);
      putsUSART(answer);
    }
    break;

  case 7: // help
    putsUSART(
      "\r\n"
      "help\r\n"
      "set [VAR] [VAL]\r\n"
      "get [VAR] / all\r\n"
      "err print\r\n"
      "err clear\r\n"
      "eeprom write [VAR] / all\r\n"
      "debug print\r\n"
      "var list\r\n"
      "nrf init\r\n"
      "nrf send [tx_addr] [cmd] [var] [val]\r\n"
      "nrf set [rx_addr/tx_addr] [xx] [xx] [xx] \r\n"
      "nrf get state\r\n"
      "nrf debug\r\n");
    break;

  case 8: // nrf init
    SysState.nrf24_en_f = 1;
    NRF24_Init();

    break;

  case 9: // nrf send
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

  case 10: // nrf set 
    if ( * (pArg1) == 0) {
      putsUSART("wrong var");
    } else {
      if (strncmp(pArg1, "tx_addr", 7) == 0) {
        pArg2 = pArg1 + 8;
        if ( * (pArg2) == 0) {
          putsUSART("wrong addr");
        } else {

          if (strtodec(pArg2, & pEnd, (uint16_t * ) & NRF_TX_ADDR[0], 0, 255) | strtodec(pEnd, & pEnd, (uint16_t * ) & NRF_TX_ADDR[1], 0, 255) | strtodec(pEnd, & pEnd, (uint16_t * ) & NRF_TX_ADDR[2], 0, 255)) {
            putsUSART("wrong addr value. [0-255]");
          } else {
            sprintf(answer, "Set NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_TX_ADDR[0], NRF_TX_ADDR[1], NRF_TX_ADDR[2]);
            putsUSART(answer);
          }

        }
      } else if (strncmp(pArg1, "rx_addr", 7) == 0) {
        pArg2 = pArg1 + 8;
        if ( * (pArg2) == 0) {
          putsUSART("wrong addr");
        } else {
          if (strtodec(pArg2, & pEnd, (uint16_t * ) & NRF_RX_ADDR[0], 0, 255) | strtodec(pEnd, & pEnd, (uint16_t * ) & NRF_RX_ADDR[1], 0, 255) | strtodec(pEnd, & pEnd, (uint16_t * ) & NRF_RX_ADDR[2], 0, 255)) {
            putsUSART("wrong addr value. [0-255]");
          } else {
            sprintf(answer, "Set NRF_TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n",
              NRF_RX_ADDR[0], NRF_RX_ADDR[1], NRF_RX_ADDR[2]);
            putsUSART(answer);
          }
        }
      } else if (strncmp(pArg1, "rf_ch", 7) == 0) {
        pArg2 = pArg1 + 8;
        if ( * (pArg2) == 0) {
          putsUSART("wrong addr");
        } else {

          if (strtodec(pArg2, & pEnd, (uint16_t * ) & NRF_RF_CH, 0, 127)) {
            putsUSART("wrong RF_CH value. [0-127]");
          } else {
            NRF24_WriteReg(RF_CH, 76);
            printf(answer, "Set NRF_RF_CH: 0x%02X\r\n", NRF_RF_CH);
            putsUSART(answer);
          }
        }
      } else {
        putsUSART("wrong addr type");
      }
    }
    break;

  case 11: // nrf get state
    NRF24_Get_State();
    break;

  case 12: // nrf debug
    NRF_DBG_PRINT_F ^= 1;
    if (NRF_DBG_PRINT_F) {
      putsUSART("NRF DEBUG SET");

    } else {
      putsUSART("NRF DEBUG RESET");
    }
    break;

  default:
    putsUSART("command unknown");
    break;
  }

  putsUSART("\r\n>");
}

