/*
 * com.c
 *
 *  Created on: 1 нояб. 2020 г.
 *      Author: moroz
 */
#include "com.h"
//#include "usart_ring.h"
//#include <stdlib.h>
//
//#include "../JSON/Inc/cJSON.h"
//#include "../JSON/Inc/input_JSON.h"

char DBG_buf[DBG_RX_BUFFER_SIZE] = {0,};
char DBG_str[DBG_RX_BUFFER_SIZE] = {0,};

//extern bool check_UPD_FW;
//extern uint32_t firmwareBytesToWrite;
//extern uint32_t size_of_data;

//Функция удаления символов \r и \n из строки
void clear_string(char *src)
{
	char *dst = NULL;
	if(!src) return;
	uint8_t i = 0;

	for(dst = src; *src; src++)
	{
		if(i < 2 && (*src == '\n' || *src == '\r'))
		{
			i++;
			continue;
		}
		else if(*src == '\n' || *src == '\r') *src = ' ';

		*dst++ = *src;
	}

	*dst = 0;
}
//Функция передачи байта по USART3
//Принимает байт
void USART_Tx(unsigned char Data)
{
	while(!(USART1->SR & USART_SR_TC));
	USART1->DR = Data;
}
//Функция отправки сткроки
//Принимает строку для отправки
void SEND_str(char * string)
{
	uint8_t i = 0;
	while(string[i])
	{
		USART_Tx(string[i]);
		i++;
	}
}
//Функция для отладки через COM порт
void DEBUG_main(void)
{
	if(dbg_available())
	{
		uint16_t i = 0;
		uint8_t fdbg = 1;
		memset(DBG_buf, 0, DBG_RX_BUFFER_SIZE);
		HAL_Delay(50);

		while(dbg_available())
		{
			DBG_buf[i++] = dbg_read();
			if(i > DBG_RX_BUFFER_SIZE - 1)
				break;
			HAL_Delay(1);
		}
		clear_string(DBG_buf);
		//-------------------------------------------UPD Firmware to USB----------------------------------------
//На время отладки закоментировано
//		if(firmware.check_UPD)
//			SEND_str(UPD_firmware(DBG_buf));
//		else
//			json_input(DBG_buf);		//здесь принимаем посылку и отправляем парситься
		//-----------------------------------------END UPD Firmware to USB--------------------------------------
		//---------------------------------------------Old function---------------------------------------------
		if(strstr(DBG_buf, "test") != NULL)
		{
			SEND_str("respone\n");
		}
		//-------------------------------------------End Old function-------------------------------------------
		//------------------------------------------------SD_CARD-----------------------------------------------
		else if(strstr(DBG_buf, "READ_SD") != NULL)
		{
			my_read_file();
		}
		else if(strstr(DBG_buf, "WRITE_SD") != NULL)
		{
			test_create_json();
//			my_write_file_json("TestFile.txt", "text_testing");
		}
		//------------------------------------------------------------------------------------------------------
		//--------------------------------------------UPDATE_FIRMWARE-------------------------------------------
		else if(strstr(DBG_buf, "UPDATE_FIRMWARE") != NULL)
		{
			json_input("{\"INSTRUCTION\":\"UPDATE_FIRMWARE\",\"COMMAND\":{\"TYPE\":\"FILE_DOWNLOAD\",\"NAME\":\"blink_gpio\",\"VERSION\":\"v.0.0.1\",\"SIZE\":\"248\"},\"TIME\":\"1122334455\"}");
		}
		else if(strstr(DBG_buf, "RESET_MCU") != NULL)
		{
			json_input("{\"INSTRUCTION\":\"UPDATE_FIRMWARE\",\"COMMAND\":{\"TYPE\":\"RESET_MCU\"},\"TIME\":\"1122334455\"}");
		}
		//------------------------------------------------------------------------------------------------------

//		else if(strcmp(DBG_buf,"BeginUPD") == 0)
//		{
//			json_input("{\"INSTRUCTION\":\"UPDATE_FIRMWARE\",\"COMMAND\":{\"TYPE\":\"FILE_DOWNLOAD\",\"NAME\":\"test_firmware\",\"VERSION\":\"v.0.0.1\",\"SIZE\":\"1272\"},\"TIME\":\"1122334455\"}");
//			firmware.check_UPD = true;
//			SEND_str("start\n");
//		}
//		else if(strcmp(DBG_buf,"EndUPD") == 0)
//		{
//			firmware.check_UPD = false;
//			fl_close();
//			SEND_str("end\n");
//
//			//Нужно сохранить все данные перед перезагрузкой!!!
//
//			HAL_NVIC_SystemReset();		//Перезапускаем контроллер
//		}
//		else if(firmware.check_UPD)
//		{
//			//Запись посылки но 1024 байт, прикрепляем к концу посылки еще 4 байта контрольной суммы, итого 1028 байт в посылки каждый раз.
//			//Буфер можно попробовать увеличить
//			//Следующую посылку посылать с сервера только после получения обратного сообщения что контрольная сумма сошлась
//			//Если посылка последнего пакета байт получается не кратная 4 то ее необходимо дополнить системными единицами памяти(FFh), до 1024 байт + 4 байта CRC = 1028 байт
//			char tmp_crc32[9];
//			sprintf(tmp_crc32, "%02X%02X%02X%02X", DBG_buf[1024], DBG_buf[1025], DBG_buf[1026] ,DBG_buf[1027]);	//Вытаскиваем последние 4 байта(CRC16)
//			char *pEnd;
//			uint32_t crc32 = (uint32_t)(strtol(tmp_crc32, &pEnd, 16));
//			SEND_str(my_write_file_firmware(firmware.NAME, DBG_buf, crc32));		//atoi(tmp_crc16)
//		}
//		else	//тест для посылки строки через терминал
//		{
//			snprintf(DBG_str, DBG_RX_BUFFER_SIZE, "%s", DBG_buf);
//			SEND_str(DBG_str);
//			json_input(DBG_str);
//		}
		if(fdbg)
		{
			snprintf(DBG_str, DBG_RX_BUFFER_SIZE, "%s\n", DBG_buf);

			//json_input("{\"INSTRUCTION\":\"SET_PERIPHERALS\",\"COMMAND\":{\"TYPE\":\"DIGITAL\",\"SET\":\"[0,1,1,0,0,0,0,1]\"},\"TIME\":\"1122334455\"}");
		}
	}
}
