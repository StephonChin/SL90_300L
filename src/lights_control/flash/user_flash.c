#include <stdint.h>
#include "../display/display.h"
#include "../display/layout.h"
#include "../display/music.h"
#include "../process/data_process.h"
#include "../../include/util.h"
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
#define	USER_CUSTOM_STEADY_FLASH_SECTOR			1017
#define	USER_CUSTOM_STEADY_FLASH_SIZE			1024
#define	USER_CUSTOM_STEADY_FLASH_ADD_START		USER_CUSTOM_STEADY_FLASH_SECTOR * SPI_FLASH_SEC_SIZE

#define	USER_CUSTOM_DYNAMIC_FLASH_SECTOR		990
#define	USER_CUSTOM_DYNAMIC_FLASH_SIZE			1024
#define	USER_CUSTOM_DYNAMIC_FLASH_ADD_START		USER_CUSTOM_DYNAMIC_FLASH_SECTOR * SPI_FLASH_SEC_SIZE



//global paramters
bool 			user_normal_flash_write_flag;
bool			user_normal_flash_write_clear_flag;
static uint16_t	user_normal_flash_write_time;


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

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_VERTICAL_LAYER_OFFSET, (uint32_t*)&vertical_layer, INFO_STRUCT_TOTAL);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_TRIANGLE_LAYER_OFFSET, (uint32_t*)&triangle_layer, INFO_STRUCT_TOTAL);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_FAN_LAYER_OFFSET, (uint32_t*)&fan_layer, INFO_STRUCT_TOTAL);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_DYNAMIC_TIME_FLAG, (uint32_t*	)DynamicTimeFlag, DYNAMIC_MAX_TIME);

	spi_flash_read(normal_flash_add_head + NORMAL_FLASH_CUSTOM_ENABLE, (uint32_t*	)CustomEnableFlag, 4);

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
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_VERTICAL_LAYER_OFFSET, (uint32_t*)&vertical_layer, INFO_STRUCT_TOTAL) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_TRIANGLE_LAYER_OFFSET, (uint32_t*)&triangle_layer, INFO_STRUCT_TOTAL) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}
				
				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_FAN_LAYER_OFFSET, (uint32_t*)&fan_layer, INFO_STRUCT_TOTAL) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}

				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_DYNAMIC_TIME_FLAG, (uint32_t*)DynamicTimeFlag, DYNAMIC_MAX_TIME) == SPI_FLASH_RESULT_OK)
				{
					err_num++;
				}

				if(spi_flash_write(normal_flash_add_head + NORMAL_FLASH_CUSTOM_ENABLE, (uint32_t*)CustomEnableFlag, 4) == SPI_FLASH_RESULT_OK)
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

			if (err_num == 10)
			{
				//printf("=FLASH= write sucessfully!!");
			}
			else
			{
				//printf("=FLASH= write failed!!");
			}
		}
	}

	else
	{
		user_normal_flash_write_time = 0;
	}
}



/****
	* read_user_normal_flash
	* 	Get the mode, color, layer, timing information from nomal flash(sector 1018)
	*/
void read_custom_steady_flash(uint8_t sec, uint8_t *dst)
{
	uint32_t flash_add_head = USER_CUSTOM_STEADY_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;

	spi_flash_read(flash_add_head + (uint32_t)sec * 1024, (uint32_t *)dst, 1024);
}



uint8_t  write_custom_steady_flash(uint8_t sec, uint8_t *src)
{
	uint32_t flash_add_head = USER_CUSTOM_STEADY_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
	uint32_t err_num = 0;


	uint8_t *dst0 = 0;
	uint8_t *dst1 = 0;
	uint8_t *dst2 = 0;

	dst0 = mmalloc(1024);
	dst1 = mmalloc(1024);
	dst2 = mmalloc(1024);

	if (dst0 != 0 && dst1 != 0 && dst2 != 0)
	{
		if ( sec == 0)
		{
			mcpy((uint8_t *)dst0, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(0, (uint8_t *)dst0);
		}

		if ( sec == 1)
		{
			mcpy((uint8_t *)dst1, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(1, (uint8_t *)dst1);
		}

		if ( sec == 2)
		{
			mcpy((uint8_t *)dst2, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(2, (uint8_t *)dst2);
		}

		//disable all interrupts
		noInterrupts();
		if(spi_flash_erase_sector(USER_CUSTOM_STEADY_FLASH_SECTOR) == SPI_FLASH_RESULT_OK) 
		{
			if(spi_flash_write(flash_add_head, (uint32_t *)dst0, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}

			if(spi_flash_write(flash_add_head+1024, (uint32_t *)dst1, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}

			
			if(spi_flash_write(flash_add_head+2048, (uint32_t *)dst2, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}

		}

		//enable all interrupts
		interrupts();
	}

	mfree(dst0);
	mfree(dst1);
	mfree(dst2);

	if (err_num == 3)
	{
		//printf("=FLASH= write sucessfully!!");
	}
	else
	{
		//printf("=FLASH= write failed!!");
	}

	return err_num;
}


/****
	* read_user_normal_flash
	* 	Get the mode, color, layer, timing information from nomal flash(sector 1018)
	*/
void read_custom_dynamic_flash(uint8_t sec, uint8_t *dst)
{
	uint32_t sec_offset = (uint32_t)sec / 4;
	uint32_t add_offset = ((uint32_t)sec % 4) * 1024;
	uint32_t flash_add_head = 0;

	flash_add_head = (USER_CUSTOM_DYNAMIC_FLASH_SECTOR + sec_offset) * SPI_FLASH_SEC_SIZE + add_offset;

	spi_flash_read(flash_add_head, (uint32_t *)dst, 1024);
}


uint8_t write_custom_dynamic_flash(uint8_t sec, uint8_t *src)
{
	uint32_t sec_offset = (uint32_t)sec / 4;
	uint32_t flash_add_head = 0;
	uint32_t err_num = 0;

	flash_add_head = (USER_CUSTOM_DYNAMIC_FLASH_SECTOR + sec_offset) * SPI_FLASH_SEC_SIZE;

	uint8_t *dst0 = 0;
	uint8_t *dst1 = 0;
	uint8_t *dst2 = 0;
	uint8_t *dst3 = 0;

	dst0 = mmalloc(1024);
	dst1 = mmalloc(1024);
	dst2 = mmalloc(1024);
	dst3 = mmalloc(1024);

	if (dst0 != 0 && dst1 != 0 && dst2 != 0 && dst3 != 0)
	{
		uint8_t i = (sec % 4);
		if ( i == 0)
		{
			mcpy((uint8_t *)dst0, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(sec_offset * 4 + 0, (uint8_t *)dst0);
		}

		if ( i == 1)
		{
			mcpy((uint8_t *)dst1, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(sec_offset * 4 + 1, (uint8_t *)dst1);
		}

		if ( i == 2)
		{
			mcpy((uint8_t *)dst2, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(sec_offset * 4 + 2, (uint8_t *)dst2);
		}

		if ( i == 3)
		{
			mcpy((uint8_t *)dst3, src, LED_TOTAL * 3);
		}
		else
		{
			read_custom_steady_flash(sec_offset * 4 + 3, (uint8_t *)dst3);
		}


		//disable all interrupts
		noInterrupts();
		if(spi_flash_erase_sector(USER_CUSTOM_DYNAMIC_FLASH_SECTOR+sec_offset) == SPI_FLASH_RESULT_OK) 
		{
			if(spi_flash_write(flash_add_head, (uint32_t *)dst0, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}
			if(spi_flash_write(flash_add_head+1024, (uint32_t *)dst1, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}
			if(spi_flash_write(flash_add_head+2048, (uint32_t *)dst2, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}
			if(spi_flash_write(flash_add_head+3072, (uint32_t *)dst3, 1024) == SPI_FLASH_RESULT_OK)
			{
				err_num++;
			}
		}

		//enable all interrupts
		interrupts();
	}


	mfree(dst0);
	mfree(dst1);
	mfree(dst2);
	mfree(dst3);

	if (err_num == 4)
	{
		printf("=FLASH= write sucessfully!!");
	}
	else
	{
		printf("=FLASH= write failed!!");
	}

	return err_num;
}

void clear_all_dynamic_flash(void)
{
	uint32_t flash_add_head = 0;
	uint32_t err_num = 0;

	flash_add_head = USER_CUSTOM_DYNAMIC_FLASH_SECTOR;

	//disable all interrupts
	noInterrupts();
	for (uint8_t i = 0; i < 26; i++)
	{
		if(spi_flash_erase_sector(USER_CUSTOM_DYNAMIC_FLASH_SECTOR+i) == SPI_FLASH_RESULT_OK) 
		{
			err_num++;
		}
	}
	//enable all interrupts
	interrupts();

	if (err_num == 26)
	{
		printf("=FLASH= clear sucessfully!!");
	}
	else
	{
		printf("=FLASH= clear failed!!");
	}

	return err_num;
}



