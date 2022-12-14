/*
 * sd_cmd.c
 *
 *  Created on: 6 мая 2022 г.
 *      Author: MMorozov
 */

#include "../../Core/fatfs/Inc/sd_cmd.h"

extern FATFS FATFS_Obj;
extern FRESULT result;
extern FIL MyFile;

extern uint32_t BytesToWrite;
extern uint32_t firmwareBytesToWrite;
extern uint32_t firmwareBytesCounter;

extern bool check_init;
extern char DateTime[];
extern int LaunchNum;

uint8_t readBuffer[512];	//Буфер для хранения прочитанных с карты данных
uint8_t WriteBuffer[248];
uint32_t BytesToRead = 0;	//Буфер для хранения размера файла
uint32_t BytesCounter = 0;	//Счетчик кол-ва прочитанных данных итерируемый пачками readBuffer[512]
UINT readBytes = 0;			//Счетчик кол-ва прочитанных данных
UINT WriteBytes = 0;		//Счетчик кол-ва записанных данных

extern bool LogFileCreate;

//Функция инициализации карты памяти
void MyInitCard(void)
{
//	SD_PowerOn();
	sd_ini();
}
//Функция чтения файла с карты памяти
void MyReadFile(void)
{
	if (f_mount(0, &FATFS_Obj) == FR_OK)	//Монтируем модуль FatFs
	{
		SendStr("f_mount -> success\n");

		uint8_t path[18]="JSON_voltage.json";
		path[17] = '\0';

		result = f_open(&MyFile, (char*)path, FA_READ);

		if(result == FR_OK)
		{
			SendStr("f_open -> success\n");

			BytesToRead = MyFile.fsize;

			char str1[60];
			sprintf(str1, "file_Size: %d Byte\n", BytesToRead);
			SendStr(str1);

			BytesCounter = 0;
			while ((BytesToRead - BytesCounter) >= 512)
		    {
		       	f_read(&MyFile, readBuffer, 512, &readBytes);
		       	BytesCounter += 512;

		       	HAL_UART_Transmit(&huart3, (uint8_t*)readBuffer, strlen(readBuffer), 0x1000);
		    }
		    if (BytesToRead != BytesCounter)
		    {
	        	f_read(&MyFile, readBuffer, (BytesToRead - BytesCounter), &readBytes);

	        	HAL_UART_Transmit(&huart3, (uint8_t*)readBuffer, BytesToRead - BytesCounter, 0x1000);

	        	BytesCounter = BytesToRead;
	        }
//			result = f_read(&test, readBuffer, sizeof(readBuffer), &readBytes);
//			if(result == FR_OK)
//			{
//				SendStr("f_read -> success\n");
//				SendStr(readBuffer);
//				SendStr("\n");
//				sprintf(str1,"BytesToRead: %d\n",readBytes);
//				SendStr(str1);
//			}
		    f_close(&MyFile);
//		    f_unlink((char*)path);
		}
	}
}
//Функция записи файла на карту памяти
//Принимает "path" - указатель на имя файла
//Принимает "text" - указатель на строку JSON, которую нужно сохранить
void MyWriteFileJson(char *path, char *text)
{
	if (f_mount(0, &FATFS_Obj) == FR_OK)
	{
		SendStr("f_mount -> success\n");

		result = f_open(&MyFile, path + '\0', FA_CREATE_ALWAYS|FA_WRITE);

		if(result == FR_OK)
		{
			SendStr("f_open -> success\n");

			result = f_write(&MyFile, text, strlen(text), &WriteBytes);
			if(result == FR_OK)
			{
				SendStr("f_write -> success\n");

				char str1[60];
				sprintf(str1, "write_bytes: %d Byte\n", WriteBytes);
				SendStr(str1);
			}
		    f_close(&MyFile);
		}
	}
}
//Создание файла логов если такового не на карте памяти
//Функция принимает имя файла логов
//Default name: LogFile.txt
void CreateLogFile(char *name)
{
	char text[4096];
	sprintf(text, "<< ManagePower version firmware %s log-file %s >>\n"
				  "------------------------------------------------------------------"
				  "%s	--->	ManagePower launch number %d\n"
				  "%s	--->	Configuration:\n"
				  "\t\t\t\tSETPOINT: %.2f,\n"
				  "\t\t\t\tSEQUENCE: %s,\n"
				  "\t\t\t\tCOUPWORK: %s,\n"
				  "\t\t\t\tHIGHPRIORITY: %s,\n"
				  "\t\t\t\tHANDCTRL: %s,\n"
				  "\t\t\t\tDUTYCYCLE: %s\n", Config.VERSIONFIRMWARE, DateTime,
				  	  	  	  	  	  	  	 DateTime, LaunchNum,
											 DateTime,
											 Config.SETPOINT,
											 Config.SEQUENCE,
											 Config.COUPWORK,
											 Config.HIGHPRIORITY,
											 Config.HANDCTRL,
											 Config.DUTYCYCLE);

	if(!LogFileCreate)
	{
		if (f_mount(0, &FATFS_Obj) == FR_OK)
		{
			result = f_open(&MyFile, name + '\0', FA_CREATE_ALWAYS | FA_WRITE);
			if(result == FR_OK)
			{
				result = f_write(&MyFile, text, 256, &WriteBytes);

				LogFileCreate = true;
			}
		}
	}
	FlClose();
}
//Функция добавления логов в созданный ранее файл
//Функция принимает имя файла куда дописать
//Принимает текст который надо дописать в файл
int AddToLogFile(char *name, char *text)
{
	if (f_mount(0, &FATFS_Obj) == FR_OK)
	{
		result = f_open(&MyFile, name + '\0', FA_OPEN_ALWAYS | FA_WRITE);
		if(result == FR_OK)
		{
			firmwareBytesCounter = 0;
			uint16_t size = strlen(text);
			result = f_lseek(&MyFile, MyFile.fsize);	//Поиск конца файла
			if(result == FR_OK)
			{
				if((size - firmwareBytesCounter) >= 256)	//default: 248
				{
					result = f_write(&MyFile, text, 256, &WriteBytes);
					firmwareBytesCounter += 256;
					if(firmwareBytesCounter == size)
					{
						f_close(&MyFile);
						Config.LOGFILE = false;
						return SUCCESS;
					}
				}
				else if (size != firmwareBytesCounter)
				{
					result = f_write(&MyFile, text, (size - firmwareBytesCounter) , &WriteBytes);
					firmwareBytesCounter = size;
					f_close(&MyFile);
					Config.LOGFILE = false;
					return SUCCESS;
				}
			}
		}
	}
	return ERROR;
}
//Функция записи файла прошивки .bin на карту памяти
//Принимает "path" - указатель на имя файла
//Принимает "data_bytes" - указатель на буффер данных, которые нужно сохранить
//Принимает "crc32" - контрольную сумму принимаемого пакеда данных
//Возвращает статус контроля целостности данных
//char *MyWriteFileFirmware(char *name, char *data_bytes, uint32_t crc32)
//{
//	if(!check_init)
//	{
//		if (f_mount(0, &FATFS_Obj) == FR_OK)
//		{
//			result = f_open(&MyFile, name + '\0', FA_CREATE_ALWAYS | FA_WRITE);
//			if(result == FR_OK)
//			{
//				check_init = true;
//				firmwareBytesCounter = 0;
//			}
//		}
//	}
//	if(check_init)
//	{
//		result = f_lseek(&MyFile, MyFile.fsize);	//Поиск конца файла
//		if(result == FR_OK)
//		{
//			uint32_t crc32_t = HAL_CRC_Calculate(&hcrc, (uint32_t *)(data_bytes), 256);	//3-й параметр - указываем количество полных слов(0xFFFFFF)
//			if(crc32 == crc32_t)
//			{
//				if((firmware.SIZE - firmwareBytesCounter) >= 1024)	//default: 248
//				{
//					result = f_write(&MyFile, data_bytes, 1024, &WriteBytes);
//					firmwareBytesCounter += 1024;
//					if(firmwareBytesCounter == firmware.SIZE)
//					{
//					    f_close(&MyFile);
//						firmware.check_UPD = false;
//						return FW_COMPLETE;
//					}
//				}
//				else if (firmware.SIZE != firmwareBytesCounter)
//				{
//					result = f_write(&MyFile, data_bytes, (firmware.SIZE - firmwareBytesCounter) , &WriteBytes);
//					firmwareBytesCounter = firmware.SIZE;
//				    f_close(&MyFile);
//					firmware.check_UPD = false;
//					return FW_COMPLETE;
//				}
//				return FW_CRC32_OK;
//			}
//			return FW_CRC32_ERR;
//		}
//	}
//	return FW_UPD_ERROR;
//}
//Функция закрытия файла
void FlClose(void)
{
    f_close(&MyFile);
}
//Функция преобразования строки в строку шестнадцетиричных значений
//Принимает "input" - указатель на преобразуемую строку
//Принимает "output" - указатель на преобразованную строку
void str2hexStr(char *input, char *output)
{
	int loop = 0, i = 0;

	while(input[loop] != '\0')
	{
		sprintf((char*)(output + i),"%02X", input[loop]);
		loop+=1;
		i+=2;
	}
	output[i++] = '\0';
}
//Функция сохраниения конфигурационных данных (Включить/выключить цифровой выход(Открытый коллектор) если цифровой вход = значение(уровень))
//Принимает "D_IN" - строку с номером цифрового входа
//Принимает "text" - указатель на строку JSON
void save_dido(char *D_IN, char *text)
{
	char name_FIL[32];

	SendStr(text);
	sprintf(name_FIL,"%s%s.json", D_IN, "(DiDo)");
	SendStr(name_FIL);
	MyWriteFileJson(name_FIL, text);
}
