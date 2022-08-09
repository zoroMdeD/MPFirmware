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
extern bool LEFT_NUM_UP;				//Флаг нажатия кнопки "Целые+"
extern bool LEFT_NUM_DOWN;			//Флаг нажатия кнопки "Целые-"
extern bool RIGHT_NUM;				//Флаг нажатия кнопки "Десятые+"
extern bool info;					//Флаг главного экрана
extern uint8_t time;					//Переменная задержки
extern double Current;
extern double reserve_Current;
extern uint8_t What_Time;
extern bool display_Off;

bool display_Sleep = false;

void Display_info(void)
{
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
	if(LEFT_NUM_DOWN)
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
	  	HAL_Delay(time);
	  	What_Time = 0;
	  	LEFT_NUM_DOWN = false;
	}
	else if(LEFT_NUM_UP)
	{
	  	HAL_Delay(time);
	  	if((GPIOB->IDR & GPIO_PIN_1) == 0)	//Включаем дисплей
	  	{
		  	HAL_Delay(2000);
		  	if((GPIOB->IDR & GPIO_PIN_1) == 0)
		  	{
				info = true;
				display_Sleep = false;
		  	}
	  	}
	  	else if(!display_Sleep)
	  	{
	  		ssd1306_SetCursor(60, 23);
	  		snprintf(trans_str, 63, "%.2fA", Current += 1);
	  		ssd1306_WriteString(trans_str, Font_11x18, White);
	  		ssd1306_UpdateScreen(&hi2c2);
	  	}
	  	What_Time = 0;
	  	LEFT_NUM_UP = false;
 	}
	else if(RIGHT_NUM)
	{
	  	HAL_Delay(time);
	  	if((GPIOB->IDR & GPIO_PIN_2) == 0  && !display_Sleep)
	  	{
		  	HAL_Delay(2000);
		  	if((GPIOB->IDR & GPIO_PIN_2) == 0)
		  	{
			  	ssd1306_Fill(Black);
			  	ssd1306_UpdateScreen(&hi2c2);

			  	reserve_Current = Current * 1.20;	//Уставка по току плюс 20%
			  	//Вызвать функцию сохранения уставки по току!!!

			  	SEND_str("-------------------------\n");
			  	SEND_str("| ");
			  	 SEND_str("SETPOINT: ");
		    	snprintf(trans_str, 63, "%.2fA", reserve_Current);
		    	SEND_str(trans_str);
		    	SEND_str(" |\n");
		    	SEND_str("-------------------------\n");

			  	ssd1306_SetCursor(42, 23);
			  	ssd1306_WriteString("SAVE", Font_11x18, White);
			  	ssd1306_UpdateScreen(&hi2c2);
			  	HAL_Delay(1500);

				info = true;
		  	}
	  	}
	  	else if(!display_Sleep)
	  	{
		  	ssd1306_SetCursor(60, 23);
		  	snprintf(trans_str, 63, "%.2fA", Current += 0.1);
		  	ssd1306_WriteString(trans_str, Font_11x18, White);
		  	ssd1306_UpdateScreen(&hi2c2);
	  	}
	  	What_Time = 0;
	  	RIGHT_NUM = false;
	}
}
