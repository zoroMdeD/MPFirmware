/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "com.h"
#include "usart_ring.h"
#include "../periphery_io/Inc/data_process.h"
#include "../periphery_io/Inc/cmd.h"
#include "../JSON/Inc/cJSON.h"
#include "../JSON/Inc/input_JSON.h"
#include "../JSON/Inc/create_JSON.h"
#include "../led/Inc/ssd1306.h"
#include "../led/Inc/fonts.h"
#include "../led/Inc/led.h"
//----------------------------FATfs---------------------------
#include "../../Core/fatfs/Inc/sd_cmd.h"
#include "../../Core/fatfs/Inc/spi_sd.h"
#include <stdarg.h>
#include "../fatfs/Inc/sd_main.h"
//------------------------------------------------------------
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

	/*
	 * <<Phase(A)>>
	 * htim2 -> TIM_CHANNEL_1 = Phase Impulse Modulation phase A forward	-	(AFWD) 			-	PA15
	 * htim2 -> TIM_CHANNEL_4 = Phase Impulse Modulation phase A reverse	-	(AREV)			-	PA3
	 * Trigger Source = Zero crossing phase A 								-	(A_ZeroCross)	-	PB3
	 *
	 * <<Phase(B)>>
	 * htim3 -> TIM_CHANNEL_1 = Phase Impulse Modulation phase B forward	-	(BFWD) 			-	PB4
	 * Trigger Source = Zero crossing phase B 								-	(B_ZeroCross)	-	PB5
	 *
	 * <<Phase(C)>>
	 * htim4 -> TIM_CHANNEL_1 = Phase Impulse Modulation phase C forward	-	(CFWD) 			-	PB6
	 * htim4 -> TIM_CHANNEL_4 = Phase Impulse Modulation phase C reverse	-	(CREV)			-	PB9
	 * Trigger Source = Zero crossing phase C 								-	(C_ZeroCross)	-	PB7
	 */

#define REINIT	1
//Settings firmware
extern struct ConfigurationSystem
{
	double SETPOINT;		//Уставка по току по превышению которой останавливаем работу эл.привода
	char *SEQUENCE;			//Флаг правильности чередования фаз(правильно ли расключили колодки)
	char *COUPWORK;			//Флаг работаем по муфте или нет
	char *VERSIONFIRMWARE;	//Версия прошивки
	char *HIGHPRIORITY;
	char *HANDCTRL;
	char *DUTYCYCLE;
	bool LOGFILE;			//Флаг необходимости дополнить логфайл
}Config;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define distCLOSE_Pin GPIO_PIN_13
#define distCLOSE_GPIO_Port GPIOC
#define distCLOSE_EXTI_IRQn EXTI15_10_IRQn
#define distSTOP_Pin GPIO_PIN_14
#define distSTOP_GPIO_Port GPIOC
#define distSTOP_EXTI_IRQn EXTI15_10_IRQn
#define distINT_Pin GPIO_PIN_15
#define distINT_GPIO_Port GPIOC
#define distINT_EXTI_IRQn EXTI15_10_IRQn
#define distHIGHP_Pin GPIO_PIN_0
#define distHIGHP_GPIO_Port GPIOC
#define distHIGHP_EXTI_IRQn EXTI0_IRQn
#define handOPEN_Pin GPIO_PIN_1
#define handOPEN_GPIO_Port GPIOC
#define handOPEN_EXTI_IRQn EXTI1_IRQn
#define handCLOSE_Pin GPIO_PIN_2
#define handCLOSE_GPIO_Port GPIOC
#define handCLOSE_EXTI_IRQn EXTI2_IRQn
#define handCTRL_Pin GPIO_PIN_3
#define handCTRL_GPIO_Port GPIOC
#define A_Current_Pin GPIO_PIN_0
#define A_Current_GPIO_Port GPIOA
#define B_Current_Pin GPIO_PIN_1
#define B_Current_GPIO_Port GPIOA
#define C_Current_Pin GPIO_PIN_2
#define C_Current_GPIO_Port GPIOA
#define AREV_Pin GPIO_PIN_3
#define AREV_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOA
#define MISO_Pin GPIO_PIN_6
#define MISO_GPIO_Port GPIOA
#define MOSI_Pin GPIO_PIN_7
#define MOSI_GPIO_Port GPIOA
#define CD_Pin GPIO_PIN_4
#define CD_GPIO_Port GPIOC
#define BT1_Pin GPIO_PIN_0
#define BT1_GPIO_Port GPIOB
#define BT2_Pin GPIO_PIN_1
#define BT2_GPIO_Port GPIOB
#define BT3_Pin GPIO_PIN_2
#define BT3_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_10
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_11
#define SDA_GPIO_Port GPIOB
#define SELF_CAPTURE_Pin GPIO_PIN_12
#define SELF_CAPTURE_GPIO_Port GPIOB
#define COUPLING_WORK_Pin GPIO_PIN_13
#define COUPLING_WORK_GPIO_Port GPIOB
#define HIGHP_CLOSEmcu_Pin GPIO_PIN_14
#define HIGHP_CLOSEmcu_GPIO_Port GPIOB
#define HIGHP_OPENmcu_Pin GPIO_PIN_15
#define HIGHP_OPENmcu_GPIO_Port GPIOB
#define mcuINT_Pin GPIO_PIN_6
#define mcuINT_GPIO_Port GPIOC
#define mcuCLOSE_Pin GPIO_PIN_7
#define mcuCLOSE_GPIO_Port GPIOC
#define mcuOPEN_Pin GPIO_PIN_8
#define mcuOPEN_GPIO_Port GPIOC
#define mcuREADY_Pin GPIO_PIN_9
#define mcuREADY_GPIO_Port GPIOC
#define mcuFAIL_Pin GPIO_PIN_8
#define mcuFAIL_GPIO_Port GPIOA
#define TxD_Pin GPIO_PIN_9
#define TxD_GPIO_Port GPIOA
#define RxD_Pin GPIO_PIN_10
#define RxD_GPIO_Port GPIOA
#define CLOSEmcu_Pin GPIO_PIN_11
#define CLOSEmcu_GPIO_Port GPIOA
#define CLOSEmcu_EXTI_IRQn EXTI15_10_IRQn
#define OPENmcu_Pin GPIO_PIN_12
#define OPENmcu_GPIO_Port GPIOA
#define OPENmcu_EXTI_IRQn EXTI15_10_IRQn
#define AFWD_Pin GPIO_PIN_15
#define AFWD_GPIO_Port GPIOA
#define WIRE_TxD_Pin GPIO_PIN_10
#define WIRE_TxD_GPIO_Port GPIOC
#define WIRE_RxD_Pin GPIO_PIN_11
#define WIRE_RxD_GPIO_Port GPIOC

#if REINIT
	#define A_ZeroCross_Pin GPIO_PIN_3
	#define A_ZeroCross_GPIO_Port GPIOB
	#define A_ZeroCross_EXTI_IRQn EXTI3_IRQn
#else
	#define A_ZeroCross_Pin GPIO_PIN_3
	#define A_ZeroCross_GPIO_Port GPIOB
#endif

#define BFWD_Pin GPIO_PIN_4
#define BFWD_GPIO_Port GPIOB

#if REINIT
	#define B_ZeroCross_Pin GPIO_PIN_5
	#define B_ZeroCross_GPIO_Port GPIOB
	#define B_ZeroCross_EXTI_IRQn EXTI9_5_IRQn
#else
	#define B_ZeroCross_Pin GPIO_PIN_5
	#define B_ZeroCross_GPIO_Port GPIOB
#endif

#define CFWD_Pin GPIO_PIN_6
#define CFWD_GPIO_Port GPIOB

#if REINIT
	#define C_ZeroCross_Pin GPIO_PIN_7
	#define C_ZeroCross_GPIO_Port GPIOB
	#define C_ZeroCross_EXTI_IRQn EXTI9_5_IRQn
#else
	#define C_ZeroCross_Pin GPIO_PIN_7
	#define C_ZeroCross_GPIO_Port GPIOB
#endif

#define distOPEN_Pin GPIO_PIN_8
#define distOPEN_GPIO_Port GPIOB
#define distOPEN_EXTI_IRQn EXTI9_5_IRQn
#define CREV_Pin GPIO_PIN_9
#define CREV_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define DEBUG_USART		0				//Отладка через USART 1 - Вкл; 0 - Выкл
//------------------------UPD_Firmware------------------------
#define FW_CRC32_OK		"OK"			//Контрольная сумма совпала, пакет данных цел
#define FW_CRC32_ERR	"ERROR"			//Ошибка передачи пакета данных(необходимо повторить посылку пакета)
#define FW_UPD_ERROR	"UPD_ERROR"		//Ошибка инициализации карты
#define FW_COMPLETE		"COMPLETE"		//Операция передачи файла и его записи на карту завершена
//------------------------------------------------------------
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
