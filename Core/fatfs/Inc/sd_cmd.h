/*
 * sd_cmd.h
 *
 *  Created on: 6 мая 2022 г.
 *      Author: MMorozov
 */

#ifndef FATFS_INC_SD_CMD_H_
#define FATFS_INC_SD_CMD_H_

#include "main.h"

void my_init_card(void);
void my_read_file(void);
void my_write_file_json(char *path, char *text);
char *my_write_file_firmware(char *name, char *data_bytes, uint32_t crc32);
void fl_close(void);
void save_dido(char *D_IN, char *text);
void save_aido(char *A_IN, char *text);
void save_pwm(char *PWM_OUT, char *text);
void save_tsido(char *ROM_RAW, char *text);

#endif /* FATFS_INC_SD_CMD_H_ */
