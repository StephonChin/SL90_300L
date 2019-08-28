/**************************************************************
  * 
  * FileName  Data_process.h
  * Date      26 NOV 2018
  * Author    DS.Chin
  *
****************************************************************/
#ifndef _DATA_PROCESS_H_
#define _DATA_PROCESS_H_

//include files
#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>

#include "osapi.h"
#include "../key/key.h"
#include "../../include/m2m_log.h"
#include "../../include/m2m.h"
#include "../display/display.h"
#include "../com/mcu_com.h"
#include "countdown.h"
#include "../flash/user_flash.h"
#include "../display/music.h"
#include "../display/layout.h"
#include "../../include/util.h"

/*
 * PRODUCT_TYPE		super lights(SL90)  -0x01
 * PROTOCOL_VER		protocol version
 */
#define     PRODUCT_TYPE            		(uint8_t)0x01
#define     APP_PROTOCOL_VER            	(uint8_t)0x01
#define     VOICE_PROTOCOL_VER            	(uint8_t)0x55



/*
 * command value(from app to device)
 */
#define     CHECK_ALL_STATUS_CMD        (uint8_t)0x01
#define     CHECK_MODE_STATUS_CMD       (uint8_t)0x02
#define     CHECK_NAME_STATUS_CMD       (uint8_t)0x03

#define     SET_ON_OFF_CMD              (uint8_t)0x10
#define     SET_MODE_CMD                (uint8_t)0x11
#define     SET_COLOR_CMD               (uint8_t)0x12
#define		SET_MODE_COLOR_CMD			(uint8_t)0x13

#define     SET_CNTDWN_HOUR_CMD         (uint8_t)0x34
#define		SET_REALTIME_CMD			(uint8_t)0x35
#define     SET_CNTDWN_TIME_CMD         (uint8_t)0x36


#define 	LAYOUT_ENTER_CMD			(uint8_t)0x20
#define		LAYOUT_TEST_CMD				(uint8_t)0x21
#define		LAYOUT_SAVE_CMD				(uint8_t)0x22
#define 	LAYOUT_SEC_CTRL				(uint8_t)0x28
#define 	LAYOUT_MOD_CTRL				(uint8_t)0x29
#define 	LAYOUT_DOT_CTRL				(uint8_t)0x2a
#define 	LAYOUT_DOT_NOADD_CTRL		(uint8_t)0x2b
#define 	LAYOUT_DOT_NOADD_CTRL_2		(uint8_t)0x2c

#define     VOICE_SET_ON_OFF_CMD        (uint8_t)0x60
#define     VOICE_SET_MODE_CMD          (uint8_t)0x61
#define     VOICE_SET_COLOR_CMD         (uint8_t)0x62
#define     VOICE_SET_THEME_CMD         (uint8_t)0x63
#define     VOICE_SET_CNTDWN_HOUR_CMD   (uint8_t)0x64
#define     VOICE_SET_CNTDWN_TIME_CMD   (uint8_t)0x65
#define     VOICE_MSC_EN_CMD            (uint8_t)0x66


#define		VOICE_ON_NEW				(uint8_t)0x40
#define		VOICE_OFF_NEW				(uint8_t)0x41
#define		VOICE_COLOR_NEW				(uint8_t)0x42
#define		VOICE_THEME_NEW				(uint8_t)0x42
#define		VOICE_MODE_NEW				(uint8_t)0x43
#define		VOICE_TIMER_NEW				(uint8_t)0x45


/*
 * ack command value(from device to app)
 */
#define     ALL_STATUS_ACK          		(uint8_t)0x81
#define     MODE_STATUS_ACK         		(uint8_t)0x82
#define     NAME_STATUS_ACK         		(uint8_t)0x83

#define		CNTDWN_HOUR_STATUS_ACK			(uint8_t)0xb4
#define		REALTIME_STATUS_ACK				(uint8_t)0xb5
#define		CNTDWN_TIME_STATUS_ACK			(uint8_t)0xb6

#define		LAYOUT_ENTER_ACK				(uint8_t)0xa0
#define		LAYOUT_TEST_ACK					(uint8_t)0xa1
#define		LAYOUT_SAVE_ACK					(uint8_t)0xa2
#define 	LAYOUT_SEC_CTRL_ACK				(uint8_t)0xa8
#define 	LAYOUT_MOD_CTRL_ACK				(uint8_t)0xa9
#define 	LAYOUT_DOT_CTRL_ACK				(uint8_t)0xaa
#define 	LAYOUT_DOT_NOADD_CTRL_ACK		(uint8_t)0xab
#define 	LAYOUT_DOT_NOADD_CTRL_2_ACK		(uint8_t)0xac


/*
 * color value
 */
#define		GAMMA							1
#define		R								(uint8_t)0
#define		G								(uint8_t)1
#define		B								(uint8_t)2
#define     RED             				(uint8_t)0x00
#define     GREEN           				(uint8_t)0x01
#define     BLUE            				(uint8_t)0x02
#define     ORANGE         					(uint8_t)0x03
#define     PINK            				(uint8_t)0x04
#define     SPRING_GREEN    				(uint8_t)0x05
#define     GOLD            				(uint8_t)0x06
#define     PEACH           				(uint8_t)0x07
#define     LAWN_GREEN      				(uint8_t)0x08
#define     CYAN            				(uint8_t)0x09
#define     SKY_BLUE        				(uint8_t)0x0a
#define     YELLOW          				(uint8_t)0x0b
#define     PURPLE          				(uint8_t)0x0c
#define     WHITE           				(uint8_t)0x0d
#define		COLD_WHITE						(uint8_t)0x0e
#define		WARM_WHITE						(uint8_t)0x0f
#define		SPRING 							(uint8_t)0x10
#define		SUMMER							(uint8_t)0x11
#define 	AUTUMN							(uint8_t)0x12
#define		WINTER							(uint8_t)0x13
#define		CHRISTMAS						(uint8_t)0x14
#define		VALENTINES						(uint8_t)0x15
#define		INDEPENDENCE					(uint8_t)0x16
#define		THANKSGIVING					(uint8_t)0x17
#define		PATRICKES						(uint8_t)0x18
#define		HALLOWEEN						(uint8_t)0x19
#define		SUN								(uint8_t)0x1a
#define		EARTH							(uint8_t)0x1b
#define		MUTICOLOR						(uint8_t)0x1c
#define     COLOR_SELF          			(uint8_t)0xff


//typedef
#define		MCU_PROTOCOL_VER				0x11
#define		MCU_TIMER_CMD					0x1
#define		MCU_MODE_CMD					0x2
#define		MCU_MUSIC_EN_CMD				0x3
#define		MCU_MUSIC_LOW					0x11
#define		MCU_UART_PACK_HEADER			4
typedef struct
{
  uint8_t   ver;
  uint8_t   index;
  uint8_t   cmd;
  uint8_t   payload;
  uint8_t   payload1[UART0_RCV_BUFFER_MAX];
}_type_mcu_uart;

typedef struct
{
	uint8_t	ver;
	uint8_t	cmd;
	uint8_t	idx;
	uint8_t	len;
	uint8_t	payload[8];
}_type_voice_cmd;


#define APP_PACK_HEADER_BYTE		8
#define	APP_PACK_LEN_MAX			1000
#define APP_PACK_SIZE_MAX       	(APP_PACK_HEADER_BYTE+APP_PACK_LEN_MAX)

typedef struct
{
  uint8_t   type;
  uint8_t   ver;
  uint8_t  	reserve;
  uint8_t   index;
  uint8_t   chksum;
  uint8_t   cmd;
  uint16_t  len;
  uint8_t   payload[APP_PACK_LEN_MAX];
}_type_app_pack;




//global functions
void User_Data_Init(void);
void Data_Process(void);


//file functions
void 		Key_Process(void);
void 		Count_Down_Process(void);
void 		Mcu_com_process(void);
void 		App_data_prcoess(void);
void 		res_to_app(uint8_t cmd,const uint8_t *pdata, uint16_t len);
uint16_t	CRC16_Cal(uint8_t* Buffer, uint16_t len);
uint8_t   	chksum_cal(const uint8_t *src, uint16_t len);
void      	Color_Caculate(uint8_t *rtR, uint8_t *rtG, uint8_t *rtB, uint8_t rIn, uint8_t gIn, uint8_t bIn);
uint8_t		setting_mode_preset_color(uint8_t mode, uint8_t color);
uint8_t   	Color_Value_Get(uint8_t ColorNumBuf);


//global parameters
extern uint32_t				flash_first_number;
extern _type_app_pack    	app_pack,app_ack_pack;

#endif

