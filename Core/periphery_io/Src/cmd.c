/*
 * cmd.c
 *
 *  Created on: Aug 10, 2022
 *      Author: moroz
 */
#include "../periphery_io/Inc/cmd.h"

extern bool Forward;
extern bool Reverse;
extern bool Stop;
extern bool DirMove_OPENmcu;
extern bool DirMove_CLOSEmcu;

extern uint16_t Compare_1;
extern uint16_t Compare_2;
extern uint16_t Compare_3;

//Функция запуска направления движения задвижки ОТКР/ЗАКР
void Direction_Move(void)
{
	if(Forward)	//Открытие задвижки
	{
		DirMove_CLOSEmcu = false;
		Forward = false;

		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)

		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);	//Run timer four channel one	(CFWD)

		DirMove_OPENmcu = true;

		Compare_1 = 9000;
		Compare_2 = 9000;
		Compare_3 = 9000;
	}
	else if(Reverse)	//Закрытие задвижки
	{
		DirMove_OPENmcu = false;
		Reverse = false;

		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)

		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	//Run timer two channel four	(AREV)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	//Run timer four channel four	(CREV)

		DirMove_CLOSEmcu = true;

		Compare_1 = 9000;
		Compare_2 = 9000;
		Compare_3 = 9000;
	}
	else if(Stop)	//Останавливаем привод
	{
		DirMove_OPENmcu = false;
		DirMove_CLOSEmcu = false;

		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)

		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)

		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)

		Stop = false;
	}
}
