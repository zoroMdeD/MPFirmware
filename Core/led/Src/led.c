/*
 * led.c
 *
 *  Created on: 5 авг. 2022 г.
 *      Author: moroz
 */

#include "main.h"
#include "../led/Inc/ssd1306.h"
#include "../led/Inc/fonts.h"
#include "../led/Inc/led.h"

extern char trans_str[];
extern bool info;						//Флаг главного экрана
extern uint8_t time;					//Переменная задержки
extern double Current;
extern double reserve_Current;
extern uint8_t What_Time;
extern bool display_Off;

extern bool display_Sleep;

bool short_state1 = 0;
bool short_state2 = 0;
bool short_state3 = 0;
bool long_state1 = 0;
bool long_state2 = 0;
bool long_state3 = 0;
uint32_t time_key1 = 0;
uint32_t time_key2 = 0;
uint32_t time_key3 = 0;

void DisplayInfo(void)
{
	uint32_t ms = HAL_GetTick();
	bool key1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	bool key2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	bool key3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);

	if(info || display_Off)
	{
		if(display_Off)	//Выключаем дисплей
		{
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen(&hi2c2);
			display_Sleep = true;
			display_Off = false;
		}
		if(info)
		{
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen(&hi2c2);

			ssd1306_SetCursor(1, 23);
			ssd1306_WriteString("ISet:", Font_11x18, White);

			ssd1306_SetCursor(60, 23);
			snprintf(trans_str, 63, "%.2fA", Current);
			ssd1306_WriteString(trans_str, Font_11x18, White);
			ssd1306_UpdateScreen(&hi2c2);
			info = false;
		}
	}
	if(key1_state == false && !short_state1 && (ms - time_key1) > 100)
	{
	    short_state1 = true;
	    long_state1 = false;
	    time_key1 = ms;
	}
	if(key2_state == false && !short_state2 && (ms - time_key2) > 100)
	{
	    short_state2 = true;
	    long_state2 = false;
	    time_key2 = ms;
	}
	if(key3_state == false && !short_state3 && (ms - time_key3) > 100)
	{
	    short_state3 = true;
	    long_state3 = false;
	    time_key3 = ms;
	}
	else if(key1_state == false && !long_state1 && (ms - time_key1) > 2000)
	{
	    long_state1 = true;
	    // действие на длинное нажатие
//	    SendStr("LONG_PRESS_BT1\n");
	}
	else if(key2_state == false && !long_state2 && (ms - time_key2) > 2000)
	{
	    long_state2 = true;

		info = true;
		display_Sleep = false;
	  	What_Time = 0;

//	    SendStr("LONG_PRESS_BT2\n");
	}
	else if(key3_state == false && !long_state3 && (ms - time_key3) > 2000)
	{
	    long_state3 = true;

	  	ssd1306_Fill(Black);
	  	ssd1306_UpdateScreen(&hi2c2);

	  	reserve_Current = Current * 1.25;	//Уставка по току плюс 25%
	  	//Вызвать функцию сохранения уставки по току!!!

//	  	SendStr("-------------------------\n");
//	  	SendStr("| ");
	  	SendStr("SETPOINT: ");
	  	snprintf(trans_str, 63, "%.2fA\n", reserve_Current);
	  	SendStr(trans_str);
//	  	SendStr(" |\n");
//	  	SendStr("-------------------------\n");

	  	ssd1306_SetCursor(42, 23);
	  	ssd1306_WriteString("SAVE", Font_11x18, White);
	  	ssd1306_UpdateScreen(&hi2c2);
	  	HAL_Delay(1500);

		info = true;

	  	What_Time = 0;

//	    SendStr("LONG_PRESS_BT3\n");
	}
	else if(key1_state == true && short_state1 && (ms - time_key1) > 100)
	{
	    short_state1 = false;
	    time_key1 = ms;

	    if(!long_state1)
	    {
		  	if(Current >= 1 && !display_Sleep)
		  	{
			  	if(Current >= 10 && Current < 11)
			  	{
			  		ssd1306_SetCursor(60, 23);
				    ssd1306_WriteString("      ", Font_11x18, White);
				  	ssd1306_SetCursor(60, 23);
				  	snprintf(trans_str, 63, "%.2fA", Current -= 1);
				  	ssd1306_WriteString(trans_str, Font_11x18, White);
				  	ssd1306_UpdateScreen(&hi2c2);
			  	}
			  	else
			  	{
				  	ssd1306_SetCursor(60, 23);
				  	snprintf(trans_str, 63, "%.2fA", Current -= 1);
				  	ssd1306_WriteString(trans_str, Font_11x18, White);
				  	ssd1306_UpdateScreen(&hi2c2);
			  	}
		  	}
		  	What_Time = 0;
//	    	SendStr("SHORT_PRESS_BT1\n");
	    }
	}
	else if(key2_state == true && short_state2 && (ms - time_key2) > 100)
	{
	    short_state2 = false;
	    time_key2 = ms;

	    if(!long_state2)
	    {
		  	if(!display_Sleep)
		  	{
		  		ssd1306_SetCursor(60, 23);
		  		snprintf(trans_str, 63, "%.2fA", Current += 1);
		  		ssd1306_WriteString(trans_str, Font_11x18, White);
		  		ssd1306_UpdateScreen(&hi2c2);
		  	}
		  	What_Time = 0;
//	    	SendStr("SHORT_PRESS_BT2\n");
	    }
	}
	else if(key3_state == true && short_state3 && (ms - time_key3) > 100)
	{
	    short_state3 = false;
	    time_key3 = ms;

	    if(!long_state3)
	    {
		  	if(!display_Sleep)
		  	{
			  	ssd1306_SetCursor(60, 23);
			  	snprintf(trans_str, 63, "%.2fA", Current += 0.1);
			  	ssd1306_WriteString(trans_str, Font_11x18, White);
			  	ssd1306_UpdateScreen(&hi2c2);
		  	}
		  	What_Time = 0;
//	    	SendStr("SHORT_PRESS_BT3\n");
	    }
	}
//	if(LEFT_NUM_DOWN)
//	{
//	  	HAL_Delay(time);
//	  	if(Current >= 1 && !display_Sleep)
//	  	{
//		  	if(Current >= 10 && Current < 11)
//		  	{
//		  		ssd1306_SetCursor(60, 23);
//			    ssd1306_WriteString("      ", Font_11x18, White);
//			  	ssd1306_SetCursor(60, 23);
//			  	snprintf(trans_str, 63, "%.2fA", Current -= 1);
//			  	ssd1306_WriteString(trans_str, Font_11x18, White);
//			  	ssd1306_UpdateScreen(&hi2c2);
//		  	}
//		  	else
//		  	{
//			  	ssd1306_SetCursor(60, 23);
//			  	snprintf(trans_str, 63, "%.2fA", Current -= 1);
//			  	ssd1306_WriteString(trans_str, Font_11x18, White);
//			  	ssd1306_UpdateScreen(&hi2c2);
//		  	}
//	  	}
//	  	What_Time = 0;
//	  	LEFT_NUM_DOWN = false;
//	}
//	else if(LEFT_NUM_UP)
//	{
//	  	HAL_Delay(time);
//	  	if((GPIOB->IDR & GPIO_PIN_1) == 0)	//Включаем дисплей
//	  	{
//		  	HAL_Delay(2000);
//		  	if((GPIOB->IDR & GPIO_PIN_1) == 0)
//		  	{
//				info = true;
//				display_Sleep = false;
//		  	}
//	  	}
//	  	else if(!display_Sleep)
//	  	{
//	  		ssd1306_SetCursor(60, 23);
//	  		snprintf(trans_str, 63, "%.2fA", Current += 1);
//	  		ssd1306_WriteString(trans_str, Font_11x18, White);
//	  		ssd1306_UpdateScreen(&hi2c2);
//	  	}
//	  	What_Time = 0;
//	  	LEFT_NUM_UP = false;
// 	}
//	else if(RIGHT_NUM)
//	{
//	  	HAL_Delay(time);
//	  	if((GPIOB->IDR & GPIO_PIN_2) == 0  && !display_Sleep)
//	  	{
//		  	HAL_Delay(2000);
//		  	if((GPIOB->IDR & GPIO_PIN_2) == 0)
//		  	{
//			  	ssd1306_Fill(Black);
//			  	ssd1306_UpdateScreen(&hi2c2);
//
//			  	reserve_Current = Current * 1.25;	//Уставка по току плюс 25%
//			  	//Вызвать функцию сохранения уставки по току!!!
//
//			  	SendStr("-------------------------\n");
//			  	SendStr("| ");
//			  	 SendStr("SETPOINT: ");
//		    	snprintf(trans_str, 63, "%.2fA", reserve_Current);
//		    	SendStr(trans_str);
//		    	SendStr(" |\n");
//		    	SendStr("-------------------------\n");
//
//			  	ssd1306_SetCursor(42, 23);
//			  	ssd1306_WriteString("SAVE", Font_11x18, White);
//			  	ssd1306_UpdateScreen(&hi2c2);
//			  	HAL_Delay(1500);
//
//				info = true;
//		  	}
//	  	}
//	  	else if(!display_Sleep)
//	  	{
//		  	ssd1306_SetCursor(60, 23);
//		  	snprintf(trans_str, 63, "%.2fA", Current += 0.1);
//		  	ssd1306_WriteString(trans_str, Font_11x18, White);
//		  	ssd1306_UpdateScreen(&hi2c2);
//	  	}
//	  	What_Time = 0;
//	  	RIGHT_NUM = false;
//	}
}
