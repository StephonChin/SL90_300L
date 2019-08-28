#include <stdint.h>
#include "../display/display.h"
#include "../display/layout.h"
#include "../display/music.h"
#include "../process/data_process.h"
#include "user_flash.h"

//flash sector size(4K)
#ifndef SPI_FLASH_SEC_SIZE
#define	SPI_FLASH_SEC_SIZE	(uint32_t)(4*1024)
#endif

//user normal flash parameters defined
//sector = 1018
#define	USER_NORMAL_FLASH_SECTOR		1018
#define	USER_NORMAL_FLASH_SIZE			1024
#define	USER_NORMAL_FLASH_ADD_START		USER_NORMAL_FLASH_SECTOR * SPI_FLASH_SEC_SIZE
#define	USER_NORMAL_FLASH_DELAY_TIME	100


//user custom mode  flash parameters defined
//sector = 1017
#define	USER_CUSTOM_FLASH_SECTOR		1017
#define	USER_CUSTOM_FLASH_SIZE			1024
#define	USER_CUSTOM_FLASH_ADD_START		USER_CUSTOM_FLASH_SECTOR * SPI_FLASH_SEC_SIZE
#define	USER_CUSTOM_FLASH_DELAY_TIME	100


//global paramters
bool 			user_normal_flash_write_flag;
bool			user_normal_flash_write_clear_flag;
static uint16_t	user_normal_flash_write_time;

bool 			user_custom_flash_write_flag;
bool			user_custom_flash_write_clear_flag;
static uint16_t	user_custom_flash_write_time;


/****
	* read_user_normal_flash
	* 	Get the mode, color, layer, timing information from nomal flash(sector 1018)
	*/
void read_user_normal_flash(void)
{
	uint32_t normal_flash_add_head = USER_NORMAL_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
	uint32_t data_buf = 0;

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_FIRST_NUMBER_OFFSET, (uint32_t*)&flash_first_number, 4);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_MODE_VALUE_OFFSET, (uint32_t*)&data_buf, 4);
	display_data.mode_buf = (uint8_t)data_buf;

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_TIMING_DATA_OFFSET, (uint32_t*)&timing_data, 24);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_MUSIC_INFORMATION_OFFSET, (uint32_t*)&music_data, 8);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_ALL_LAYER_BRIEF_OFFSET, (uint32_t*)&layer_brief, 8);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_VERTICAL_LAYER_OFFSET, (uint32_t*)&vertical_layer, 200);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_TRIANGLE_LAYER_OFFSET, (uint32_t*)&triangle_layer, 200);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_FAN_LAYER_OFFSET, (uint32_t*)&fan_layer, 200);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_MODE_INFORMATION_OFFSET, (uint32_t*)mode_para_data, 56 * MODE_MAX);
}


void write_user_normal_flash(void)
{
	if (user_normal_flash_write_flag == true)
	{
		if (user_normal_flash_write_clear_flag == true)
		{
			user_normal_flash_write_clear_flag = false;
			user_normal_flash_write_time = 0;
		}

		user_normal_flash_write_time++;
		if (user_normal_flash_write_time >= USER_NORMAL_FLASH_DELAY_TIME)
		{
			user_normal_flash_write_time = 0;
			user_normal_flash_write_flag = false;

			uint32_t normal_flash_add_head = USER_NORMAL_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
			uint32_t data_buf = 0;
			uint32_t err_num = 0;

			//disable all interrupts
			noInterrupts();
			if(spi_flash_erase_sector(USER_NORMAL_FLASH_SECTOR) == SPI_FLASH_RESULT_OK) 
			{
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_FIRST_NUMBER_OFFSET, (uint32_t*)&flash_first_number, 4) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}

				data_buf = (uint32_t)display_data.mode_buf;
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_MODE_VALUE_OFFSET, (uint32_t*)&data_buf, 4) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}

				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_TIMING_DATA_OFFSET, (uint32_t*)&timing_data, 24) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_MUSIC_INFORMATION_OFFSET, (uint32_t*)&music_data, 8) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_ALL_LAYER_BRIEF_OFFSET, (uint32_t*)&layer_brief, 8) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_VERTICAL_LAYER_OFFSET, (uint32_t*)&vertical_layer, 200) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_TRIANGLE_LAYER_OFFSET, (uint32_t*)&triangle_layer, 200) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_FAN_LAYER_OFFSET, (uint32_t*)&fan_layer, 200) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_MODE_INFORMATION_OFFSET, 
				(uint32_t*)mode_para_data, 56 * MODE_MAX) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
			}

			//enable all interrupts
			interrupts();

			if (err_num == 9)
			{
				printf("=FLASH= write sucessfully!!");
			}
			else
			{
				printf("=FLASH= write failed!!");
			}
		}
	}

	else
	{
		user_normal_flash_write_time = 0;
	}
}


void user_flash_init(void)
{
#if 0
	uint32_t i = 0x12345678;
	noInterrupts();
	if(spi_flash_erase_sector(USER_FLASH_SECTOR) == SPI_FLASH_RESULT_OK) 
	{
		if(spi_flash_write(USER_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, (uint32_t*)&i, 4) == SPI_FLASH_RESULT_OK) 
		{
			printf("write ok\n");
		}
	}
	interrupts();
#endif
}


void user_flash_read(void)
{
	
}

void user_flash_write(void)
{
#if 0
	if (user_flash_write_flag)
	{
		if (user_flash_write_clear_flag)
		{
			user_flash_write_clear_flag = false;
			user_flash_write_time = 0;
		}
		
		user_flash_write_time++;
		if (user_flash_write_time >= USER_FLASH_DELAY_TIME)
		{
			user_flash_write_time = 0;
			user_flash_write_flag = false;

			noInterrupts();
			if(spi_flash_erase_sector(USER_FLASH_SECTOR) == SPI_FLASH_RESULT_OK) 
			{
				if(spi_flash_write(USER_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, (uint32_t*)&display_data.mode_buf, 4) == SPI_FLASH_RESULT_OK) 
				{
					printf("write ok\n");
				}
			}
			interrupts();
		}	
	}
#endif
}



