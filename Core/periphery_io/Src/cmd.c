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

extern bool handOPEN_flag;
extern bool handCLOSE_flag;
extern bool distOPEN_flag;
extern bool distCLOSE_flag;
extern bool distSTOP_flag;

extern bool SELF_CAPTURE_flag;

extern uint16_t Compare;

//Функция запуска направления движения задвижки ОТКР/ЗАКР
void DirectionMove(void)
{
	if(Forward)	//Открытие задвижки
	{
		/*
		 * <<<ВАЖНО>>>
		 * Таймеры отвечающие за движение вала в противоположное направление необходимо остановить
		 * и только потом запускать таймеры отвечающие за движение в выбранную сторону, иначе произойдет наложение полуволн,
		 * что приведет к возникновению сквозных токов, вследствии чего сгорят симмисторы.
		 */

		//Останавливаем таймеры на закрытие
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)

		DirMove_CLOSEmcu = false;
		Forward = false;

		DirMove_OPENmcu = true;

		Compare = 9000;

		/*
		 * !!!
		 * Возможно нужно поставить задержку для исключения одновременной работы таймеров
		 * на открытие и закрытие
		 */
		HAL_Delay(20);

		//Запускаем таймеры на открытие
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);	//Run timer four channel one	(CFWD)
	}
	else if(Reverse)	//Закрытие задвижки
	{
		//Останавливаем таймеры на открытие
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)

		DirMove_OPENmcu = false;
		Reverse = false;

		DirMove_CLOSEmcu = true;

		Compare = 9000;

		HAL_Delay(20);

		//Запускаем таймеры на закрытие
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	//Run timer two channel four	(AREV)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	//Run timer four channel four	(CREV)
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
