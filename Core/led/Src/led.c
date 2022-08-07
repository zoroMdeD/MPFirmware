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

void display_info(void)
{
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
	if(LEFT_NUM_DOWN)
	{
	  	if(Current >= 1)
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
	  	LEFT_NUM_DOWN = false;
	}
	else if(LEFT_NUM_UP)
	{
	  	ssd1306_SetCursor(60, 23);
	  	snprintf(trans_str, 63, "%.2fA", Current += 1);
	  	ssd1306_WriteString(trans_str, Font_11x18, White);
	  	ssd1306_UpdateScreen(&hi2c2);
	  	HAL_Delay(time);
	  	LEFT_NUM_UP = false;
 	}
	else if(RIGHT_NUM)
	{
	  	HAL_Delay(time);
	  	if((GPIOB->IDR & GPIO_PIN_2) == 0)
	  	{
		  	HAL_Delay(2000);
		  	if((GPIOB->IDR & GPIO_PIN_2) == 0)
		  	{
			  	ssd1306_Fill(Black);
			  	ssd1306_UpdateScreen(&hi2c2);

			  	ssd1306_SetCursor(42, 23);
			  	ssd1306_WriteString("SAVE", Font_11x18, White);
			  	ssd1306_UpdateScreen(&hi2c2);
			  	HAL_Delay(1500);

				info = true;
		  	}
	  	}
	  	else
	  	{
		  	ssd1306_SetCursor(60, 23);
		  	snprintf(trans_str, 63, "%.2fA", Current += 0.1);
		  	ssd1306_WriteString(trans_str, Font_11x18, White);
		  	ssd1306_UpdateScreen(&hi2c2);
	  	}
	  	RIGHT_NUM = false;
	}
}
