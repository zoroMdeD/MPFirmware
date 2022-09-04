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
extern bool handCTRL_flag;

extern uint16_t DutyCicle;

extern bool OpenBlink;
extern bool CloseBlink;

extern uint16_t Compare;

bool checkOne = false;
bool checkTwo = false;

//Функция запуска направления движения задвижки ОТКР/ЗАКР
void DirectionMove(void)
{
	//Открытие задвижки
	if(Forward)
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

		if(handCTRL_flag)
			handCLOSE_flag = true;
		else if(!handCTRL_flag)
			distCLOSE_flag = true;

		DirMove_OPENmcu = true;

		Compare = 9000;

		/*
		 * !!!
		 * Возможно нужно поставить задержку для исключения одновременной работы таймеров
		 * на открытие и закрытие
		 */
		HAL_Delay(20);

		//Запускаем таймеры на открытие
		if((GPIOA->IDR & OPENmcu_Pin) != 0)
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);	//Run timer four channel one	(CFWD)
		}
	}
	//Закрытие задвижки
	else if(Reverse)
	{
		//Останавливаем таймеры на открытие
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)

		DirMove_OPENmcu = false;
		Reverse = false;

		if(handCTRL_flag)
			handOPEN_flag = true;
		else if(!handCTRL_flag)
			distOPEN_flag = true;

		DirMove_CLOSEmcu = true;

		Compare = 9000;

		HAL_Delay(20);

		//Запускаем таймеры на закрытие
		if((GPIOA->IDR & CLOSEmcu_Pin) != 0)
		{
			 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	//Run timer two channel four	(AREV)
			 HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
			 HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	//Run timer four channel four	(CREV)
		}
	}
	//Останавливаем привод
	else if(Stop)
	{
		DirMove_OPENmcu = false;
		DirMove_CLOSEmcu = false;

		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)

		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)

		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)

		/*
		 * Если самоподхват не установлен и управление с метного пульта
		 */
		if(!SELF_CAPTURE_flag && handCTRL_flag)
		{
			handOPEN_flag = true;
			handCLOSE_flag = true;
		}
		/*
		 * Если управление с дистанционного пульта
		 */
		else if(!handCTRL_flag)
		{
			distOPEN_flag = true;
			distCLOSE_flag = true;
		}

//		OpenBlink = false;
//		CloseBlink = false;

		Stop = false;
	}
}
void ManagementProcess(void)
{
	/*
	 * Проверяем сигнал выбора управления местное/дистанционное
	 * Если в высоком уровне, то управление местное
	 * Если в низком уровне то управление дистанционное
	 *
	 * handCTRL(GPIOC3): 	Management:
	 *      High			  Local
	 *      Low			  	 Remote
	 */
	handCTRL_flag = GPIOC->IDR & handCTRL_Pin;
	if(handCTRL_flag && !checkOne)
	{
		checkOne = true;
		checkTwo = false;

		distOPEN_flag = false;
		distCLOSE_flag = false;
		distSTOP_flag = false;
//	  	distINT_flag = false;

		handOPEN_flag = true;
		handCLOSE_flag = true;
	}
	else if(!handCTRL_flag && !checkTwo)
	{
		checkTwo = true;
		checkOne = false;

		handOPEN_flag = false;
		handCLOSE_flag = false;

		distOPEN_flag = true;
		distCLOSE_flag = true;
		distSTOP_flag = true;
//	  	distINT_flag = true;
	}
}
void SelfCaptureProcess(void)
{
	/*
	 * Проверяем выбран ли режим работы эл.привода с самоподхватом,
	 * если режим работы с самоподхватом не выбран, то выставляем флаг, что самоподхват отсудствует,
	 * если выбран режим с самоподхватом то работаем в обычном режиме
	 */
	SELF_CAPTURE_flag = GPIOB->IDR & SELF_CAPTURE_Pin;
	/*
	 * Проверяем была ли нажата кнопка, и проверяем выключен ли режим самоподхвата,
	 * если выключен режим самоподхвата, то при отпускании кнопки "ОТКРЫТЬ"
	 * привод остановиться. Чтобы продолжить движение необходимо снова нажать кнопку "ОТКРЫТЬ"
	 */
	if(!SELF_CAPTURE_flag && handCTRL_flag)
	{
		if(!handOPEN_flag)
		{
			if((GPIOC->IDR & handOPEN_Pin) != 0)
			{
				Stop = true;
			}
		}
		else if(!handCLOSE_flag)
		{
			if((GPIOC->IDR & handCLOSE_Pin) != 0)
			{
				Stop = true;
			}
		}
		/*
		 * Предположительно с дистанционного пульта управления работать без самоподхвата запрещено
		 * нужно уточнить этот момент
		 */
	}
}
void DutyCycleProcess(void)
{
	/*
	 * Увеличиваем скважность до уставки,
	 * если флаг направления движения выставлен,
	 * и кулачковый концевик не замкнут
	 */
	if((Compare > DutyCicle) && DirMove_OPENmcu && ((GPIOA->IDR & OPENmcu_Pin) != 0))
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Compare);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Compare);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, Compare);
		Compare = Compare - 10;
		HAL_Delay(5);	//Вопрос нужна ли задержка, и какая узнать подробней !!!
	}
	else if((Compare > DutyCicle) && DirMove_CLOSEmcu && ((GPIOA->IDR & CLOSEmcu_Pin) != 0))
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, Compare);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Compare);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, Compare);
		Compare = Compare - 10;
		HAL_Delay(5);	//Вопрос нужна ли задержка, и какая узнать подробней !!!
	}
}
