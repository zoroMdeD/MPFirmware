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
uint16_t Compare_1 = 0;
uint16_t Compare_2 = 0;
uint16_t Compare_3 = 0;
uint16_t DutyCicle = 4500;	//Уставка до какой скважности увеличивать

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
bool LEFT_NUM_UP = false;			//Флаг нажатия кнопки "Целые+"
bool LEFT_NUM_DOWN = false;			//Флаг нажатия кнопки "Целые-"
bool RIGHT_NUM = false;				//Флаг нажатия кнопки "Десятые+"
bool info = true;					//Флаг отображения главного меню
uint8_t time = 0;					//Переменная задержки
uint16_t What_Time = 0;
bool display_Off = false;
bool display_Sleep = false;
double Current = 0.0;
//-------------------------------------------
//-----------------ADC-----------------------
double Amps[3] = {0,};				//Значение тока на фазах (среднее значение)
volatile uint32_t adc[3] = {0,}; 	//Массив для хранения данных АЦП
double adcValue[3] = {0,};			//Массив для хранения обработанных данных АЦП
double reserve_Current = 0.0;
uint8_t cnt = 0;					//Счетчик кол-ва измеренных значений тока
bool run_Сomparison = false;
//-------------------------------------------
//----------------FATfs----------------------
FATFS FATFS_Obj;					//�?нициализация структуры описывающей инициализацию файловой системы
FRESULT result;						//�?нициализация структуры описывающей статусы работы карты памяти
FIL MyFile;							//�?нициализация структуры описывающей выбранный файл
uint32_t BytesToWrite = 0;			//Кол-во записанных байт
int firmwareBytesToWrite = 0;		//Кол-во байт которые нужно записать(размер файла)
int firmwareBytesCounter = 0;		//Счетчик полной почсылки (248 байт)
//-------------------------------------------
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_RTC_Init();
  MX_WWDG_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	time = 200;

	EN_Interrupt();		//Для дебага/Конфигурации по USART1

	// Init lcd using one of the stm32HAL i2c typedefs
	if (ssd1306_Init(&hi2c2) != 0)
	{
		Error_Handler();
	}
	//----------------ADC-----------------------
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, 3);		//Стартуем АЦП
	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
	//------------------------------------------
	//---------------FATfs----------------------
	my_init_card();
	SEND_str("Init sd card -> success\n");
	//------------------------------------------

	//--------------------------------Test--------------------------------
//	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
//	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000);
//	uint16_t compare = 0;
	//------------------------------End Test------------------------------

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Direction_Move();
	  //Придумать как заблокировать режим работы с дисплеем
	  Display_info();
	  Сurrent_Сomparison();
	  DEBUG_main();
	  //--------------------------------Test--------------------------------
//	  while(compare < 9000)
//	  {
//		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, compare);
//	  	  compare = compare + 10;
//	  	  HAL_Delay(10);
//	  }
//	  compare = 9000;
//	  while(compare > 100)
//	  {
//		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, compare);
//	  	  compare = compare - 10;
//	  	  HAL_Delay(10);
//	  }
	  //------------------------------End Test------------------------------
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
	//--------------------------------Test--------------------------------
//		if (GPIO_Pin == GPIO_PIN_3) //Phase A ZeroCrossing
//		{
//			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
//			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
//		}
//		else if (GPIO_Pin == GPIO_PIN_5) //Phase B ZeroCrossing
//		{
//			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
//			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
//		}
	//------------------------------End Test------------------------------

	if (GPIO_Pin == GPIO_PIN_0)	//Пришла команда "Высшего приоритета" с дистанционного пульта управления (distHIGHP)
	{
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
	else if (GPIO_Pin == GPIO_PIN_1)	//Пришла команда "Открыть" с местного пульта управления (handOPEN)
	{
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL); Проверяем статус управления с вн.интерфейсов
		if(((GPIOC->IDR & GPIO_PIN_3) != 0) && !Interface)
		{
			Forward = true;
			/*
			 * Когда эл.привод докрутит до начала, замкнет концевик и загориться индикация что задвижка открыта,
			 * так же на МК прийдет флаг "OPENmcu", что будет означать что необходимо прекратить крутить эл.привод.
			 */
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, RESET);	//Убираем флаг "mcuCLOSE"
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, SET);		//Выставляем флаг "mcuOPEN"
		}
	}
	else if (GPIO_Pin == GPIO_PIN_2)	//Пришла команда "Закрыть" с местного пульта управления (handCLOSE)
	{
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL); Проверяем статус управления с вн.интерфейсов
		if(((GPIOC->IDR & GPIO_PIN_3) != 0) && !Interface)
		{
			Reverse = true;
			/*
			 * Когда эл.привод докрутит до конца, замкнет концевик и загориться индикация что задвижка закрыта,
			 * так же на МК прийдет флаг "CLOSEmcu", что будет означать что необходимо прекратить крутить эл.привод.
			 */
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, RESET);	//Убираем флаг "mcuOPEN"
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, SET);		//Выставляем флаг "mcuCLOSE"
		}
	}
	else if (GPIO_Pin == GPIO_PIN_3) //Переход через ноль на фазе "А"
	{
		if(DirMove_OPENmcu)
		{
			if(Compare_1 > DutyCicle)	//Увеличиваем скважность до уставки
			{
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Compare_1);
				Compare_1 = Compare_1 - 100;
			}
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);	//Stop timer two channel one	(AFWD)
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	//Run timer two channel one		(AFWD)
		}
		else if(DirMove_CLOSEmcu)
		{
			if(Compare_1 > DutyCicle)	//Увеличиваем скважность до уставки
			{
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, Compare_1);
				Compare_1 = Compare_1 - 100;
			}
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);	//Stop timer two channel four	(AREV)
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	//Run timer two channel four	(AREV)
		}
	}
	else if (GPIO_Pin == GPIO_PIN_5) //Переход через ноль на фазе "B"
	{
		if(DirMove_OPENmcu || DirMove_CLOSEmcu)
		{
			if(Compare_2 > DutyCicle)	//Увеличиваем скважность до уставки
			{
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Compare_2);
				Compare_2 = Compare_2 - 100;
			}
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);	//Stop timer three channel one	(BFWD)
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);	//Run timer three channel one	(BFWD)
		}
	}
	else if (GPIO_Pin == GPIO_PIN_7) //Переход через ноль на фазе "C"
	{
		if(DirMove_OPENmcu)
		{
			if(Compare_3 > DutyCicle)	//Увеличиваем скважность до уставки
			{
				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, Compare_3);
				Compare_3 = Compare_3 - 100;
			}
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);	//Stop timer Four channel one	(CFWD)
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);	//Run timer four channel one	(CFWD)
		}
		else if(DirMove_CLOSEmcu)
		{
			if(Compare_3 > DutyCicle)	//Увеличиваем скважность до уставки
			{
				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, Compare_3);
				Compare_3 = Compare_3 - 100;
			}
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);	//Stop timer four channel four	(CREV)
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	//Run timer four channel four	(CREV)
		}
	}
	else if (GPIO_Pin == GPIO_PIN_8)	//Пришла команда "Открыть" с дистанционного пульта управления (distOPEN)
	{
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL); Проверяем статус управления с вн.интерфейсов
		if(((GPIOC->IDR & GPIO_PIN_3) == 0) && !Interface)
		{
			Forward = true;
			/*
			 * Когда эл.привод докрутит до начала, замкнет концевик и загориться индикация что задвижка открыта,
			 * так же на МК прийдет флаг "OPENmcu", что будет означать что необходимо прекратить крутить эл.привод.
			 */
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, RESET);	//Убираем флаг "mcuCLOSE"
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, SET);		//Выставляем флаг "mcuOPEN"
		}
	}
	else if (GPIO_Pin == GPIO_PIN_11)	//Флаг того что привод дошел до конца "CLOSEmcu"
	{
		Stop = true;
	}
	else if (GPIO_Pin == GPIO_PIN_12)	//Флаг того что привод дошел до начала "OPENmcu"
	{
		Stop = true;
	}
	else if (GPIO_Pin == GPIO_PIN_13)	//Пришла команда "Закрыть" с дистанционного пульта управления (distCLOSE)
	{
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL); Проверяем статус управления с вн.интерфейсов
		if(((GPIOC->IDR & GPIO_PIN_3) == 0) && !Interface)
		{
			Reverse = true;
			/*
			 * Когда эл.привод докрутит до конца, замкнет концевик и загориться индикация что задвижка закрыта,
			 * так же на МК прийдет флаг "CLOSEmcu", что будет означать что необходимо прекратить крутить эл.привод.
			 */
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, RESET);	//Убираем флаг "mcuOPEN"
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, SET);		//Выставляем флаг "mcuCLOSE"
		}
	}

	//ОБРАТ�?ТЬ ВН�?МАН�?Е НА ЭТОТ С�?ГНАЛ!!!

	else if (GPIO_Pin == GPIO_PIN_14) 	//Пришла команда "Остановить" с дистанционного пульта управления (distSTOP)
	{
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL); Проверяем статус управления с вн.интерфейсов
		if(((GPIOC->IDR & GPIO_PIN_3) == 0) && !Interface)
			Stop = true;
	}

	//ОБДУМАТЬ КАК ОБРАБАТЫВАТЬ ЭТОТ С�?ГНАЛ!!!

	else if (GPIO_Pin == GPIO_PIN_15)	//Пришла команда "Передать управление плате расширения" с дистанционного пульта управления (distINT)
	{
		/*
		 * handCTRL(GPIOC3): 	Management:
		 *     	 High			  Local
		 *     	 Low			  Remote
		 */
		//Проверяем статус, с какого пульта идет управление (handCTRL)
		if((GPIOC->IDR & GPIO_PIN_3) == 0)
		{
			Interface = true;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, SET);	//Выставляем флаг "mcuINT", управление передано внешним интерфейсам
		}
	}
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
    	cnt++;
    	adcValue[0] += Conversion_ADC1((uint16_t)adc[0]);
        adcValue[1] += Conversion_ADC1((uint16_t)adc[1]);
        adcValue[2] += Conversion_ADC1((uint16_t)adc[2]);

        if(What_Time == 12000)	//Через 2 минуты отключаем дисплей
        {
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
