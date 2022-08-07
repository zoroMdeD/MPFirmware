//  ******************************************************************************
//  * @file    sd.h 
//  * @author  CIG
//  * @version V1.0.0
//  * @date    
//  * @brief   
//  ******************************************************************************

#ifndef SD_H_
#define SD_H_

#include "main.h"
#include "../../Core/fatfs/Inc/diskio.h"
#include "../../Core/fatfs/Inc/ff.h"
#include "../../Core/fatfs/Inc/ffconf.h"

/* Port Controls */
#define CS_HIGH()	HAL_GPIO_WritePin(GPIOA, CS_Pin, SET)		//GPIOB->GPIO_BSRR_BS_6		/* MMC CS = H */
#define CS_LOW()	HAL_GPIO_WritePin(GPIOA, CS_Pin, RESET)		//GPIOB->GPIO_BSRR_BR_6		/* MMC CS = L */

void disk_timerproc(void);

#endif /* SD_H_ */
