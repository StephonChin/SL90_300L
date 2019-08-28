#ifndef _USER_FLASH_H_
#define _USER_FLASH_H_

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "c_types.h"
#include "spi_flash.h"


//user normal flash address offset
#define	NORMAL_FLASH_FIRST_NUMBER_OFFSET		0
#define	NORMAL_FLASH_MODE_VALUE_OFFSET			4
#define	NORMAL_FLASH_TIMING_DATA_OFFSET			8
#define NORMAL_FLASH_MUSIC_INFORMATION_OFFSET	32
#define	NORMAL_FLASH_ALL_LAYER_BRIEF_OFFSET		40
#define	NORMAL_FLASH_VERTICAL_LAYER_OFFSET		48
#define	NORMAL_FLASH_TRIANGLE_LAYER_OFFSET		248
#define NORMAL_FLASH_FAN_LAYER_OFFSET			448
#define	NORMAL_FLASH_MODE_INFORMATION_OFFSET	648


void user_flash_init(void);
void read_user_normal_flash(void);
void write_user_normal_flash(void);




extern bool 	user_normal_flash_write_flag;
extern bool		user_normal_flash_write_clear_flag;


#endif
