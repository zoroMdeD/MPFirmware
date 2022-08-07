/*
 * input_JSON.c
 *
 *  Created on: 24 нояб. 2020 г.
 *      Author: MMorozov
 */
//#include "main.h"
//#include <stdlib.h>
//#include "../Inc/cJSON.h"
#include "../Inc/input_JSON.h"

//Settings firmware
struct SettingsFirmware
{
	char *NAME;			//Имя файла прошивки
	char *VERSION;		//Версия прошивки
	int SIZE;			//Размер файла прошивки в байтах
	bool check_UPD;		//Флаг готовности получения файла прошивки с сервера
}firmware;

char *INSTRUCTION;		//Инструкция
char *TYPE;				//Комманда

//Переменные уловных выражений
char *D_IN;					//Цифровой вход
char *D_OUT;				//Цифровой выход
char *VAR_IN;				//Переменная уровня цифрового входа
char *VAR_OUT;				//Переменная уровня цифрового выхода

char *NAME_FW;
char *VERSION_FW;
char *SIZE_FW;

char *NAME_FW;
char *VERSION_FW;
char *SIZE_FW;

char *TIME;				//Переменная JSON системного времени сервера, для сравнения актуальности данных (если время с сервера меньше чем время у клиента, то данные не актуальны и игнорируем их) Timestamp
int Time_Server = 0;	//Переменная системного времени сервера
int Time_Client = 0;	//Переменная системного времени клиента

//Функция разбора полученной строки JSON
void json_input(char *text)
{
	cJSON *json = cJSON_Parse(text);

	cJSON *stime = cJSON_GetObjectItem(json, "TIME");
	TIME = stime->valuestring;
	Time_Server = atoi(TIME);
	if(Time_Server > Time_Client)
	{
		cJSON *sInstruction = cJSON_GetObjectItem(json, "INSTRUCTION");
		INSTRUCTION = sInstruction->valuestring;

		if(strcmp(INSTRUCTION, "SET_PROGRAMM") == 0)
		{
			cJSON *sType = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "TYPE");
			TYPE = sType->valuestring;

			if(strcmp(TYPE, "SET_DIDO") == 0)	//Включить/выключить цифровой выход если цифровой вход = значение(уровень)
			{
				cJSON *s1 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "D_IN");
				cJSON *s2 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "VAR_IN");
				cJSON *s3 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "D_OUT");
				cJSON *s4 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "VAR_OUT");

				D_IN = s1->valuestring;
				VAR_IN = s2->valuestring;
				D_OUT = s3->valuestring;
				VAR_OUT = s4->valuestring;

				cJSON_Delete(json);
				free(stime);
				free(sInstruction);
				free(sType);
				free(s1);
				free(s2);
				free(s3);
				free(s4);
			}
		}
		else if(strcmp(INSTRUCTION, "UPDATE_FIRMWARE") == 0)
		{
			cJSON *sType = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "TYPE");
			TYPE = sType->valuestring;

			if(strcmp(TYPE, "FILE_DOWNLOAD") == 0)
			{
				cJSON *s1 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "NAME");
				cJSON *s2 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "VERSION");
				cJSON *s3 = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "COMMAND"), "SIZE");
				NAME_FW = s1->valuestring;
				VERSION_FW = s2->valuestring;
				SIZE_FW = s3->valuestring;

				firmware.NAME = NAME_FW;
				firmware.VERSION = VERSION_FW;
				firmware.SIZE = atoi(SIZE_FW);
				firmware.check_UPD = true;

				cJSON_Delete(json);
				free(stime);
				free(sInstruction);
				free(sType);
				free(s1);
				free(s2);
				free(s3);
			}
			else if(strcmp(TYPE, "RESET_MCU") == 0)
			{
				//Нужно сохранить все данные перед перезагрузкой!!!
				cJSON_Delete(json);
				free(stime);
				free(sInstruction);
				free(sType);

				HAL_NVIC_SystemReset();		//Перезапускаем контроллер
			}
			else
			{
				cJSON_Delete(json);
				free(stime);
				free(sInstruction);
				free(sType);
			}
		}
		else
		{

			free(stime);
			free(sInstruction);
		}
	}
	else
	{
		cJSON_Delete(json);
		free(stime);
	}
}
