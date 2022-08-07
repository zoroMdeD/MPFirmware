/*
 * sd_main.h
 *
 *  Created on: 4 мая 2022 г.
 *      Author: MMorozov
 */

#ifndef FATFS_INC_SD_MAIN_H_
#define FATFS_INC_SD_MAIN_H_

#include "../Inc/main.h"
//--------------------------------------------------
#define CS_SD_GPIO_PORT GPIOB
#define CS_SD_PIN GPIO_PIN_6
#define SS_SD_SELECT() HAL_GPIO_WritePin(CS_SD_GPIO_PORT, CS_SD_PIN, GPIO_PIN_RESET)
#define SS_SD_DESELECT() HAL_GPIO_WritePin(CS_SD_GPIO_PORT, CS_SD_PIN, GPIO_PIN_SET)

#define LD_ON HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 	//RED
#define LD_OFF HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); 		//RED
//--------------------------------------------------
//Card type flags (CardType)
#define CT_MMC 0x01 			//MMC ver 3
#define CT_SD1 0x02 			//SD ver 1
#define CT_SD2 0x04 			//SD ver 2
#define CT_SDC (CT_SD1|CT_SD2) 	//SD
#define CT_BLOCK 0x08 			//Block addressing
//--------------------------------------------------
typedef struct sd_info {
	volatile uint8_t type;	//тип карты
} sd_info_ptr;
//--------------------------------------------------
void SD_PowerOn(void);
uint8_t sd_ini(void);
void SPI_Release(void);
//--------------------------------------------------
#endif /* FATFS_INC_SD_MAIN_H_ */
