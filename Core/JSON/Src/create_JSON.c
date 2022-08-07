/*
 * create_JSON.c
 *
 *  Created on: 12 мая 2022 г.
 *      Author: MMorozov
 */
//#include "main.h"
#include "../Inc/create_JSON.h"

//Функция создания JSON строки
void test_create_json(void)
{
	char *out;

	cJSON *root, *command;

	root = cJSON_CreateObject();
	command = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "INSTRUCTION", cJSON_CreateString("SET_PROGRAMM"));
	cJSON_AddItemToObject(root, "COMMAND", command);

	cJSON_AddItemToObject(command, "TYPE", cJSON_CreateString("SET_VAIDO"));
	cJSON_AddItemToObject(command, "A_IN", cJSON_CreateString("VHOD1"));
	cJSON_AddItemToObject(command, "RANGE_LOW", cJSON_CreateString("1.5"));
	cJSON_AddItemToObject(command, "RANGE_HIGH", cJSON_CreateString("2.5"));
	cJSON_AddItemToObject(command, "D_OUT", cJSON_CreateString("VIHOD4"));
	cJSON_AddItemToObject(command, "VAR_OUT", cJSON_CreateString("1"));

	cJSON_AddItemToObject(root, "TIME", cJSON_CreateString("1122334455"));

	out = cJSON_Print(root);

	SEND_str(out);
	my_write_file_json("JSON_voltage.json", out);

	free(out);

	cJSON_Delete(root);
}
