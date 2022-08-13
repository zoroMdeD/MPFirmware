/*
 * com.c
 *
 *  Created on: 1 нояб. 2020 г.
 *      Author: moroz
 */
#include "com.h"

char DBG_buf[DBG_RX_BUFFER_SIZE] = {0,};
char DBG_str[DBG_RX_BUFFER_SIZE] = {0,};


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
		//---------------------------------------------Old function---------------------------------------------
		if(strstr(DBG_buf, "TEST") != NULL)
		{
			SEND_str("THE DEVICE IS WORKING\n");
		}
		//-------------------------------------------End Old function-------------------------------------------
		//------------------------------------------------SD_CARD-----------------------------------------------
		else if(strstr(DBG_buf, "READ_SD") != NULL)
		{
			my_read_file();
		}
		else if(strstr(DBG_buf, "WRITE_SD") != NULL)
		{
//			test_create_json();
			my_write_file_json("TestFile.txt", "text_testing");
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
		else	//тест для посылки строки через терминал
		{
			snprintf(DBG_str, DBG_RX_BUFFER_SIZE, "%s", DBG_buf);
			SEND_str(DBG_str);
			json_input(DBG_str);
		}
		if(fdbg)
		{
			snprintf(DBG_str, DBG_RX_BUFFER_SIZE, "%s\n", DBG_buf);
		}
	}
}
