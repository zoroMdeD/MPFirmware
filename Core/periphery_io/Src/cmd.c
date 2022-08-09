/*
 * cmd.c
 *
 *  Created on: Aug 10, 2022
 *      Author: moroz
 */
#include "../periphery_io/Inc/cmd.h"

extern bool flag_OPENmcu;
extern bool flag_CLOSEmcu;
extern bool DirMove_OPENmcu;
extern bool DirMove_CLOSEmcu;

//Функция запуска направления движения задвижки ОТКР/ЗАКР
void Direction_Move(void)
{
	if(flag_OPENmcu)
	{
		DirMove_CLOSEmcu = false;

		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)

		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);	//Run timer four channel one	(CFWD)

		DirMove_OPENmcu = true;
		flag_OPENmcu = false;
	}
	else if(flag_CLOSEmcu)
	{
		DirMove_OPENmcu = false;

		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)

		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	//Run timer two channel four	(AREV)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	//Run timer four channel four	(CREV)

		DirMove_CLOSEmcu = true;
		flag_CLOSEmcu = false;
	}
}
