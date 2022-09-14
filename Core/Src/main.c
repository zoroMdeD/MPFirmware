/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "wwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//Скважность равна нулю при значении 9000, для увеличения скважности уменьшаем значение
uint16_t Compare = 0;
uint16_t DutyCicle = 7000;	//Уставка до какой скважности увеличивать

//Флаги срабатывания управляющих сигналов
//---------------------------------------
bool distHIGHP_flag = true;
bool handOPEN_flag = false;
bool handCLOSE_flag = false;
bool distOPEN_flag = false;
bool CLOSEmcu_flag =false;
bool OPENmcu_flag = false;
bool distCLOSE_flag = false;
bool distSTOP_flag = false;
bool distINT_flag = false;

bool handCTRL_flag = false;

//Флаги правильности подключения фаз
uint8_t A = 0;
uint8_t B = 3;
uint8_t C = 0;
bool PhCorrect = false;
bool PhUncorrect = false;
uint8_t BlinkFail = 0;
uint16_t BlinkQueue = 0;

bool InitFlag = true;

bool test_flag = true;

//bool CloseBlink = false;			//Флаг блинкера на закрытие
//bool OpenBlink = false;			//Флаг блинкера на открытие
//---------------------------------------

bool SELF_CAPTURE_flag = true;		//Флаг самоподхвата

char trans_str[64] = {0,};
bool Stop = false;					//Флаг остановки эл.привода
bool Forward = false;				//Флаг начала движения эл.привода вперед(открытие)
bool Reverse = false;				//Флаг начала движения эл.привода назад(закрытие)
bool Interface = false;				//Флаг передачи управления эл.приводом внешним интерфейсам(ProfiBus, ModBus, CurrentLoop, etc)
bool HighPriority = false;			//Флаг нажатия кнопки высшего приоритета на дистанционном пульту
//-----------------CMD-----------------------
bool flag_OPENmcu = false;			//Флаг прихода команды Открыть задвижку
bool flag_CLOSEmcu = false;			//Флаг прихода команды Закрыть задвижку
bool DirMove_OPENmcu = false;		//Флаг движения задвижки на открытие
bool DirMove_CLOSEmcu = false;		//Флаг движения задвижки на закрытие
//-------------------------------------------
//-----------------LCD-----------------------
bool info = true;					//Флаг отображения главного меню
uint8_t time = 0;					//Переменная задержки
uint8_t Blink = 0;					//Счетчик времени для индикации
uint16_t What_Time = 0;				//Счетчик времени на выполнение необходимых действий
bool display_Off = false;			//Флаг состояния дисплея
bool display_Sleep = false;			//Флаг того что дисплей в спящем режиме
double Current = 0.0;				//Переменная значения выставленного тока
//-------------------------------------------
//-----------------ADC-----------------------
double Amps[3] = {0,};				//Значение тока на фазах (среднее значение)
volatile uint32_t adc[3] = {0,}; 	//Массив для хранения данных АЦП
double adcValue[3] = {0,};			//Массив для хранения обработанных данных АЦП
double reserve_Current = 0.0;		//Переменная значения тока с запасом(уставка по току)
uint16_t cnt = 0;					//Счетчик кол-ва измеренных значений тока
bool run_Сomparison = false;
//-------------------------------------------
//----------------FATfs----------------------
FATFS FATFS_Obj;					//�?нициализация структуры описывающей инициализацию файловой системы
FRESULT result;						//�?нициализация структуры описывающей статусы работы карты памяти
FIL MyFile;							//�?нициализация структуры описывающей выбранный файл
uint32_t BytesToWrite = 0;			//Кол-во записанных байт
int firmwareBytesToWrite = 0;		//Кол-во байт которые нужно записать(размер файла)
int firmwareBytesCounter = 0;		//Счетчик полной почсылки (248 байт)

bool LogFileCreate = false;			//Флаг того что необходимо создать файл логов, false - необходимо создать
//-------------------------------------------
char DateTime[64];					//Переменная даты и времени
int LaunchNum = 0;					//Счетчик кол-ва запусков устройства
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void ReInitZeroDetectorPins(void)
//{
////	GPIO_InitTypeDef GPIO_InitStruct;
////
////	/*Configure GPIO pins : PBPin PBPin */
////	GPIO_InitStruct.Pin = A_ZeroCross_Pin|B_ZeroCross_Pin|C_ZeroCross_Pin;
////	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
////	GPIO_InitStruct.Pull = GPIO_NOPULL;
////	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//}
//void InitTimersZeroDetector(void)
//{
//	MX_TIM2_Init();
//	MX_TIM3_Init();
//	MX_TIM4_Init();
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
//  MX_RTC_Init();
//  MX_WWDG_Init();
  /* USER CODE BEGIN 2 */

	time = 200;

	EN_Interrupt();	//Для дебага/Конфигурации по USART1

	// Init lcd using one of the stm32HAL i2c typedefs
	if (ssd1306_Init(&hi2c2) != 0)
	{
		Error_Handler();
	}
	//----------------ADC-----------------------
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, 3);	//Стартуем АЦП
	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
	//------------------------------------------
	//---------------FATfs----------------------
//	MyInitCard();
//	SendStr("Init sd card -> success\n");
	//------------------------------------------

//	HAL_GPIO_WritePin(GPIOC, mcuREADY_Pin, SET);	//Статус, МК работает нормально
//	HAL_GPIO_WritePin(GPIOA, mcuFAIL_Pin, RESET);	//Сбрасываем ошибку МК

	//Считываем значение с пина управления
	handCTRL_flag = GPIOC->IDR & handCTRL_Pin;
	if(handCTRL_flag)
	{
		distOPEN_flag = false;
		distCLOSE_flag = false;
		distSTOP_flag = false;
	//	  	distINT_flag = false;

		handOPEN_flag = true;
		handCLOSE_flag = true;
		#if DEBUG_USART
			SendStr("[1] - Control is local\n");
		#endif
	}
	else if(!handCTRL_flag)
	{
		distOPEN_flag = true;
		distCLOSE_flag = true;
		distSTOP_flag = true;
	//	  	distINT_flag = false;

		handOPEN_flag = false;
		handCLOSE_flag = false;
		#if DEBUG_USART
			SendStr("[2] - Control is remote\n");
		#endif
	}
	//Считываем значение с пина самоподхвата
	SELF_CAPTURE_flag = GPIOB->IDR & SELF_CAPTURE_Pin;
	#if DEBUG_USART
		if(SELF_CAPTURE_flag)
			SendStr("[3] - Self capture is set\n");
		else if(!SELF_CAPTURE_flag)
			SendStr("[4] - Self capture is reset\n");
	#endif


	#if REINIT
		MX_GPIO_Init_Interrupt();
	#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  DisplayInfo();		  //Придумать как заблокировать режим работы с дисплеем

	  if(PhCorrect)	//Проверяем правильность включение фаз
	  {
		  if(InitFlag)
		  {
			  #if DEBUG_USART
			  	  SendStr("InitFlag - off\n");
			  #endif

			  InitFlag = false;
			  #undef REINIT
			  #define REINIT	0

			  MX_GPIO_Init();
			  MX_TIM2_Init();
			  MX_TIM3_Init();
			  MX_TIM4_Init();

			  #if DEBUG_USART
			  	  SendStr("Timers init\n");
			  #endif
			  	HAL_GPIO_WritePin(GPIOC, mcuREADY_Pin, SET);	//Статус, МК работает нормально
		  }
		  ManagementProcess();
		  SelfCaptureProcess();
		  DutyCycleProcess();

		  DirectionMove();
//		  СurrentСomparison();

//		  adcValue[0] += ConversionADC((uint16_t)adc[0]);
//		  adcValue[1] += ConversionADC((uint16_t)adc[1]);
//		  adcValue[2] += ConversionADC((uint16_t)adc[2]);
	  }
	  DebugMain();

	  //-------------------------------------------------------------------------------------------------
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//Пришла команда "Высшего приоритета" с дистанционного пульта управления (distHIGHP)

	//Уточнить работает ли высший приоритет когда работаем с местного пульта управления!!!

	if ((GPIO_Pin == GPIO_PIN_0) && distHIGHP_flag)
	{
		#if DEBUG_USART
			SendStr("[101] - distHIGHP is action\n");
		#endif
		distHIGHP_flag = false;
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL)
		if((GPIOC->IDR & GPIO_PIN_3) == 0)
		{
			if(((GPIOB->IDR & GPIO_PIN_15) != 0) && ((GPIOB->IDR & GPIO_PIN_14) == 0))	//HIGHP_OPENmcu = 1; HIGHP_CLOSEmcu = 0;
			{
				Forward = true;
			}
			else if(((GPIOB->IDR & GPIO_PIN_15) == 0) && ((GPIOB->IDR & GPIO_PIN_14) != 0))	//HIGHP_OPENmcu = 0; HIGHP_CLOSEmcu = 1;
			{
				Reverse = true;
			}
		}
		HighPriority = true;
	}
	//Пришла команда "Открыть" с местного пульта управления (handOPEN)
	else if ((GPIO_Pin == GPIO_PIN_1) && handOPEN_flag)
	{
		#if DEBUG_USART
			SendStr("[102] - handOPEN is action\n");
		#endif
		handOPEN_flag = false;

//		CloseBlink = false;
//		OpenBlink = true;

//		if((GPIOA->IDR & OPENmcu_Pin) != 0)
//		{
			Forward = true;

			HAL_GPIO_WritePin(GPIOC, mcuCLOSE_Pin, RESET);	//Убираем флаг "mcuCLOSE"
			HAL_GPIO_WritePin(GPIOC, mcuOPEN_Pin, SET);		//Выставляем флаг "mcuOPEN"
//		}
//		else
//			handOPEN_flag = true;
	}
	//Пришла команда "Закрыть" с местного пульта управления (handCLOSE)
	else if ((GPIO_Pin == GPIO_PIN_2) && handCLOSE_flag)
	{
		#if DEBUG_USART
			SendStr("[103] - handCLOSE is action\n");
		#endif
		handCLOSE_flag = false;

//		OpenBlink = false;
//		CloseBlink = true;

//		if((GPIOA->IDR & CLOSEmcu_Pin) != 0)
//		{
			Reverse = true;

			HAL_GPIO_WritePin(GPIOC, mcuOPEN_Pin, RESET);	//Убираем флаг "mcuOPEN"
			HAL_GPIO_WritePin(GPIOC, mcuCLOSE_Pin, SET);	//Выставляем флаг "mcuCLOSE"
//		}
//		else
//			handCLOSE_flag = true;
	}


	//--------------------------------------РАБОТА ФАЗ--------------------------------------


	//Переход через ноль на фазе "А"
	else if (GPIO_Pin == GPIO_PIN_3)
	{
		if(/*((GPIOB->IDR & GPIO_PIN_5) == 0) && */((GPIOB->IDR & GPIO_PIN_7) == 0) && A == 0)
		{
			SendStr("[301] - A\n");
			A = 1;
			cnt = 0;
		}
		if(DirMove_OPENmcu && ((GPIOA->IDR & OPENmcu_Pin) != 0))
		{
			#if DEBUG_USART
				SendStr("[104] - AFWD\n");
			#endif
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
		}
		else if(DirMove_CLOSEmcu && ((GPIOA->IDR & CLOSEmcu_Pin) != 0))
		{
			#if DEBUG_USART
				SendStr("[105] - AREV\n");
			#endif
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	//Run timer two channel four	(AREV)
		}
	}
	 //Переход через ноль на фазе "B"
	else if (GPIO_Pin == GPIO_PIN_5)
	{
		if((cnt == 2) &&  B == 0)
		{
			B = 3;
			SendStr("[303] - B\n");
		}
		if(DirMove_OPENmcu && ((GPIOA->IDR & OPENmcu_Pin) != 0))
		{
			#if DEBUG_USART
				SendStr("[106] - BFWD\n");
			#endif
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		}
		else if(DirMove_CLOSEmcu && ((GPIOA->IDR & CLOSEmcu_Pin) != 0))
		{
			#if DEBUG_USART
				SendStr("[107] - BFWD\n");
			#endif
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		}
	}
	//Переход через ноль на фазе "C"
	else if (GPIO_Pin == GPIO_PIN_7)
	{
		if((cnt == 1) && C == 0)
		{
			C = 2;
			SendStr("[302] - C\n");
		}
		if(DirMove_OPENmcu && ((GPIOA->IDR & OPENmcu_Pin) != 0))
		{
			#if DEBUG_USART
				SendStr("[108] - CFWD\n");
			#endif
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);	//Run timer four channel one	(CFWD)
		}
		else if(DirMove_CLOSEmcu && ((GPIOA->IDR & CLOSEmcu_Pin) != 0))
		{
			#if DEBUG_USART
				SendStr("[109] - CREV\n");
			#endif
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	//Run timer four channel four	(CREV)
		}
	}


	//--------------------------------------------------------------------------------------


	//Пришла команда "Открыть" с дистанционного пульта управления (distOPEN)
	else if ((GPIO_Pin == GPIO_PIN_8) && distOPEN_flag)
	{
		#if DEBUG_USART
			SendStr("[110] - distOPEN\n");
		#endif
		distOPEN_flag = false;

//		if((GPIOA->IDR & OPENmcu_Pin) != 0)
//		{
			Forward = true;

			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, RESET);	//Убираем флаг "mcuCLOSE"
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, SET);		//Выставляем флаг "mcuOPEN"
//		}
//		else
//			distOPEN_flag = true;
	}
	//Флаг того что привод дошел до конца "CLOSEmcu"
	else if (GPIO_Pin == GPIO_PIN_11)
	{
		#if DEBUG_USART
			SendStr("[111] - CLOSEmcu\n");
		#endif
		Stop = true;
//		CLOSEmcu_flag = true;
	}
	//Флаг того что привод дошел до начала "OPENmcu"
	else if (GPIO_Pin == GPIO_PIN_12)
	{
		#if DEBUG_USART
			SendStr("[112] - OPENmcu\n");
		#endif
		Stop = true;
//		OPENmcu_flag = true;
	}
	//Пришла команда "Закрыть" с дистанционного пульта управления (distCLOSE)
	else if ((GPIO_Pin == GPIO_PIN_13) && distCLOSE_flag)
	{
		#if DEBUG_USART
			SendStr("[113] - distCLOSE\n");
		#endif
		distCLOSE_flag = false;

//		if((GPIOA->IDR & CLOSEmcu_Pin) != 0)
//		{
			Reverse = true;

			//Здесь наверное нужно моргать
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, RESET);	//Убираем флаг "mcuOPEN"
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, SET);		//Выставляем флаг "mcuCLOSE"
//		}
//		else
//			distCLOSE_flag = true;
	}
	//Пришла команда "Остановить" с дистанционного пульта управления (distSTOP)
	else if ((GPIO_Pin == GPIO_PIN_14) && distSTOP_flag)
	{
		#if DEBUG_USART
			SendStr("[114] - distSTOP\n");
		#endif
		distSTOP_flag = false;
		Stop = true;
	}
//	//Пришла команда "Передать управление плате расширения" с дистанционного пульта управления (distINT)
//	else if ((GPIO_Pin == GPIO_PIN_15) && distINT_flag)
//	{
//		distINT_flag = false;
//		Interface = true;
//
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, SET);	//Выставляем флаг "mcuINT", управление передано внешним интерфейсам
//	}
	else
	{
		__NOP();
	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)	//Убрать все из колбека
    {
    	What_Time++;
    	BlinkFail++;

    	if(InitFlag)
    		BlinkQueue++;

    	if(test_flag)
    		cnt++;
//    	adcValue[0] += ConversionADC((uint16_t)adc[0]);
//        adcValue[1] += ConversionADC((uint16_t)adc[1]);
//        adcValue[2] += ConversionADC((uint16_t)adc[2]);

//        if(Blink == 50)	//Раз в 0.5 секунд мигаем
//        {
//        	if(OpenBlink)
//        		HAL_GPIO_TogglePin(GPIOC, mcuOPEN_Pin);		//Статус, задвижка открывается(мигание)
//        	else if(CloseBlink)
//        	    HAL_GPIO_TogglePin(GPIOC, mcuCLOSE_Pin);	//Статус, задвижка закрывается(мигание)
//        }
        if(BlinkFail == 250 && PhUncorrect)	//Раз в 0.75 секунды индикация ошибки(Чередование фаз не прямое)
        {
        	HAL_GPIO_TogglePin(GPIOA, mcuFAIL_Pin);	//Чередования фаз не прямое
        	BlinkFail = 0;
        }
        if(BlinkQueue == 1000)	//Раз в 3 секунды проверяет правильность расключения фаз
        {
        	test_flag = false;
        	if(A < C && B > C)
        	{
				#if DEBUG_USART
        			SendStr("PhOK\n");
				#endif
//        		HAL_GPIO_WritePin(GPIOA, mcuFAIL_Pin, RESET);	//Чередования фаз прямое
//        		HAL_GPIO_WritePin(GPIOC, mcuREADY_Pin, SET);	//Статус, МК работает нормально
        		PhCorrect = true;
        		PhUncorrect = false;
        	}
        	else
        	{
				#if DEBUG_USART
        			SendStr("PhNO\n");
				#endif
//        		HAL_GPIO_WritePin(GPIOC, mcuREADY_Pin, RESET);	//Статус выключаем, МК работает с ошибкой
        		PhCorrect = false;
        		PhUncorrect = true;
        	}
        	BlinkQueue = 0;
        }
        if(What_Time == 12000)	//Через 2 минуты отключаем дисплей
        {
			#if DEBUG_USART
				SendStr("[21] - Display is off\n");
			#endif
        	display_Off = true;
        	What_Time = 0;
        }
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
