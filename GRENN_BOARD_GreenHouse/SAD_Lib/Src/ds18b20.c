/*
 * ds18b20.c
 *
 *  Created on: 30 ����. 2018 �.
 *      Author: a.spitsyn
 */

#include "ds18b20.h"


//Byte 0 Temperature LSB (50h)
//Byte 1 Temperature MSB (05h) 			
//Byte 2 TH Register or User Byte 1
//Byte 3 TL Register or User Byte 2
//Byte 4 Configuration Register
//Byte 5 Reserved (FFh)
//Byte 6 Reserved
//Byte 7 Reserved (10h)
//Byte 8 CRC* 

//uint8_t DS18B20_DBG_PRINT_F=1;

uint8_t SCRATCHPAD[9];
int8_t DS18B20_TEMP = 0;

uint8_t LastDeviceFlag;
uint8_t LastDiscrepancy;
uint8_t LastFamilyDiscrepancy;
uint8_t ROM_NO[8];
uint8_t Dev_ID[MAX_DEV_CNT][8];
uint8_t Dev_Cnt;
uint8_t mode;


//-----------------------------------------------
uint8_t ds18b20_SearhRom(uint8_t *Addr)
{
  uint8_t id_bit_number;
  uint8_t last_zero, rom_byte_number, search_result;
  uint8_t id_bit, cmp_id_bit;
  uint8_t rom_byte_mask, search_direction;
  //����������������� ����������
  id_bit_number = 1;
  last_zero = 0;
  rom_byte_number = 0;
  rom_byte_mask = 1;
  search_result = 0;
	if (!LastDeviceFlag)
	{
		ow_Reset();
		ow_WriteByte(0xF0);
	}
	do
  {
		id_bit = ow_ReadBit();
		cmp_id_bit = ow_ReadBit();
		if ((id_bit == 1) && (cmp_id_bit == 1))
			break;
		else
		{
			if (id_bit != cmp_id_bit)
				search_direction = id_bit; // bit write value for search
			else
			{
				if (id_bit_number < LastDiscrepancy)
					search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
				else
					search_direction = (id_bit_number == LastDiscrepancy);
				if (search_direction == 0)
				{
					last_zero = id_bit_number;
					if (last_zero < 9)
					LastFamilyDiscrepancy = last_zero;
				}
			}
			if (search_direction == 1)
				ROM_NO[rom_byte_number] |= rom_byte_mask;
			else
				ROM_NO[rom_byte_number] &= ~rom_byte_mask;
			ow_WriteBit(search_direction);
			id_bit_number++;
			rom_byte_mask <<= 1;
			if (rom_byte_mask == 0)
			{
				rom_byte_number++;
				rom_byte_mask = 1;
			}
		}
  } while(rom_byte_number < 8); // ��������� ����� � 0 �� 7 � �����
	if (!(id_bit_number < 65))
  {
	  // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
		LastDiscrepancy = last_zero;
		// check for last device
		if (LastDiscrepancy == 0)
			LastDeviceFlag = 1;
		search_result = 1;	
  }
	if (!search_result || !ROM_NO[0])
	{
		LastDiscrepancy = 0;
		LastDeviceFlag = 0;
		LastFamilyDiscrepancy = 0;
		search_result = 0;
	}
	else
  {
    for (int i = 0; i < 8; i++) Addr[i] = ROM_NO[i];
  }	
  return search_result;
}
//-----------------------------------------------
uint8_t ds18b20_Init(uint8_t device_cnt, uint8_t resolution)
{
	int i = 0, j=0;
  uint8_t dt[8];
	if (device_cnt>MAX_DEV_CNT){
		if(DS18B20_DBG_PRINT_F){
			dbg_print("DS18B20: MAX DEV_CNT");
		}
		return 1;
	}
	if (device_cnt==1){
		mode=SKIP_ROM;
	}else{
		mode=NO_SKIP_ROM;
	}
	
	
  if(mode==SKIP_ROM){
		if(ow_Reset()){ 			
			if(DS18B20_DBG_PRINT_F){
				dbg_print("DS18B20: NOT FOUND\r\n");
			}
			return 1;		//FAIL DS18B20 NOT FOUND
		}
		
		//SKIP ROM
		ow_WriteByte(0xCC);
		//WRITE SCRATCHPAD
		ow_WriteByte(0x4E);
		//TH REGISTER 100 ��������
		ow_WriteByte(0x64);
		//TL REGISTER - 30 ��������
		ow_WriteByte(0x9E);
		//Resolution 12 bit
		ow_WriteByte(resolution);
  }
	else{
		for(i=1;i<=device_cnt;i++){
			if(ds18b20_SearhRom(dt)){
				Dev_Cnt++;
				memcpy(Dev_ID[Dev_Cnt-1],dt,sizeof(dt));
			}
			else break;
		}
		if(Dev_Cnt!=device_cnt){
			if(DS18B20_DBG_PRINT_F){
				dbg_print("DS18B20: DEV_CNT BAD\r\n");
			}
			return 1;
		}
		
		for(i=1;i<=Dev_Cnt;i++){
			if(ow_Reset()) return 1;
			//Match Rom
			ow_WriteByte(0x55);
			for(j=0;j<=7;j++){
				ow_WriteByte(Dev_ID[i-1][j]);
			}
			//WRITE SCRATCHPAD
			ow_WriteByte(0x4E);
			//TH REGISTER 100 ��������
			ow_WriteByte(0x64);
			//TL REGISTER - 30 ��������
			ow_WriteByte(0x9E);
			//Resolution 
			ow_WriteByte(resolution);
		}
	}
  return 0;
}
//----------------------------------------------------------
void ds18b20_MeasureTemperCmd(uint8_t DevNum)
{
	int i = 0;
  ow_Reset();
  if(mode==SKIP_ROM){
    //SKIP ROM
    ow_WriteByte(0xCC);
  }
	else{
		//Match Rom
		ow_WriteByte(0x55);
		for(i=0;i<=7;i++)
		{
			ow_WriteByte(Dev_ID[DevNum-1][i]);
		}
	}
  //CONVERT T
  ow_WriteByte(0x44);
}
//----------------------------------------------------------
void ds18b20_ReadStratcpad(uint8_t DevNum)
{
  uint8_t i;
  ow_Reset();
  if(mode==SKIP_ROM){
    //SKIP ROM
    ow_WriteByte(0xCC);
  }
	else{
		//Match Rom
		ow_WriteByte(0x55);
		for(i=0;i<=7;i++){
			ow_WriteByte(Dev_ID[DevNum-1][i]);
		}
	}
  //READ SCRATCHPAD
  ow_WriteByte(0xBE);
  for(i=0;i<=8;i++)  {
    SCRATCHPAD[i] = ow_ReadByte();
  }
}

//----------------------------------------------------------
void ds18b20_Convert_int8(void) {

	DS18B20_TEMP = (SCRATCHPAD[1] << 4) | (SCRATCHPAD[0] >> 4);

}
//----------------------------------------------------------

uint8_t ds18b20_Check_CRC(void)
{
  uint8_t crc = 0;
	
  for (uint8_t j = 0; j<8; j++) {
    uint8_t inbyte = SCRATCHPAD[j];
    for (uint8_t i = 0; i<8; i++)    {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
	if(crc==SCRATCHPAD[8])
		return 0;
	else
		return 1;
}

uint8_t ds18b20_GetTemp(uint8_t DevNum) {

	ds18b20_MeasureTemperCmd(DevNum);
	

//	HAL_Delay(150);// 9bit resolution
//	HAL_Delay(250);// 10bit resolution
	HAL_Delay(425);// 11bit resolution
	//HAL_Delay(800);// 12bit resolution

	ds18b20_ReadStratcpad(DevNum);		
	
	uint32_t ds_crc;

	if(DS18B20_DBG_PRINT_F){
		sprintf(dbg_str,"STRATHPAD: %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n CRC:  %02X\r\n",
      SCRATCHPAD[0], SCRATCHPAD[1], SCRATCHPAD[2], SCRATCHPAD[3], SCRATCHPAD[4],
		SCRATCHPAD[5], SCRATCHPAD[6], SCRATCHPAD[7],SCRATCHPAD[8], ds_crc);
				dbg_print(dbg_str);
	}
	
	
	
	if(ds18b20_Check_CRC()){
		if(DS18B20_DBG_PRINT_F){
			dbg_print("DS18B20: CRC BAD\r\n");
		}
		return 1;
	}else{		
		ds18b20_Convert_int8();
		if(DS18B20_DBG_PRINT_F){			
			sprintf(dbg_str, "DS18B20 TEMP: %d\r\n", DS18B20_TEMP);
			dbg_print(dbg_str);			
		}
		return 0;
	}		
	
}