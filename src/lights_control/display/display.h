/**************************************************************
  * 
  * FileName  display.h
  * Date      26 NOV 2018
  * Author    DS.Chin
  *
**************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_


//include files
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../leddrv/ws2812_i2s.h"
#include "../../include/m2m_log.h"
#include "../process/data_process.h"
#include "layout.h"


//const value define
#define     COLOR_VAL_MAX         (uint8_t)0x0f  	/* the single colors count  number */
#define     THEME_VAL_MAX         (uint8_t)0x0d   	/* the themes count number */
#define     FADE_LEVEL            (uint8_t)15


/*
 * Parameter value
 */
#define     PARA_INVALID          	(uint8_t)0
#define     PARA_BRIGHT_MAX       	(uint8_t)4
#define     PARA_SPEED_MAX        	(uint8_t)4
#define     PARA_OTHER_MAX        	(uint8_t)10
#define     PARA_COLORNUM_MAX     	(uint8_t)15   /* each mode allow to own MODE_COLOR_MAX color value */


#define		DYNAMIC_MAX_TIME		(uint8_t)100

#define		CUSTOM_STEADY_1			(uint8_t)0xe1
#define		CUSTOM_STEADY_2			(uint8_t)0xe2
#define		CUSTOM_STEADY_3			(uint8_t)0xe3
#define		CUSTOM_DYNAMIC_1 		(uint8_t)0xe8

#define     POWER_OFF             	(uint8_t)0xf0
#define     POWER_ON              	(uint8_t)0xf1
#define     RED_FLASH             	(uint8_t)0xf2
#define     GREEN_FLASH           	(uint8_t)0xf3
#define     BLUE_FLASH            	(uint8_t)0xf4
#define		LAYOUT_PHOTO_CTRL		(uint8_t)0xf9
#define		LAYOUT_ENTER		 	(uint8_t)0xfa
#define		LAYOUT_TEST				(uint8_t)0xfb
#define		LAYOUT_SAVE				(uint8_t)0xfc
#define		LAYOUT_CANCEL			(uint8_t)0xfd
#define     MUSIC_MODE            	(uint8_t)0xfe
#define     COMMON_MODE_LIMIT     	POWER_OFF


#define     STEADY                	(uint8_t)0x00
#define     SPARKLE               	(uint8_t)0x01
#define     RAINBOW               	(uint8_t)0x02
#define     FADE                  	(uint8_t)0x03
#define     SNOW                  	(uint8_t)0x04
#define     SNAKE                 	(uint8_t)0x05
#define     TWINKLE               	(uint8_t)0x06
#define     FIREWORKS             	(uint8_t)0x07
#define     VERTIGO               	(uint8_t)0x08
#define     HORIZONTAL            	(uint8_t)0x09
#define     WAVES                 	(uint8_t)0x0a
#define     UPDWN                 	(uint8_t)0x0b
#define     DIAGONAL              	(uint8_t)0x0c
#define     SUNSET                	(uint8_t)0x0d
#define     VINTAGE               	(uint8_t)0x0e
#define     GLOW                  	(uint8_t)0x0f
#define     COLOR_RAND            	(uint8_t)0x10
#define		CARNIVAL				(uint8_t)0x11
#define		ALTERNATE				(uint8_t)0x12

#define		CURRENT_MODE_MAX		ALTERNATE
#define     MODE_MAX              	30

#define		RAND()					RndSeed += 199;srand(RndSeed);


#define 	SNOW_LONG               8
#define 	SNOW_BOTTOM             6
#define 	SNOW_BOTTOM_HOLD        6
#define 	SNOW_FREQ               30

//type redefine
typedef struct
{
  uint8_t mode;
  uint8_t mode_buf;
  uint8_t init;
  uint8_t reserved;
}Display_t;

#define PARA_PACK_HEADRE_BYTE	8
typedef struct
{
	uint8_t 	Mode;
	uint8_t   	Speed;
	uint8_t   	Bright;
	uint8_t   	Other;
	uint8_t   	ColorVal;
	uint8_t   	Chksum;
	uint8_t   	Reserve1;
	uint8_t   	ColorNum;
	struct RCV_COLOR_TYPE
	{
		uint8_t		BufR;
		uint8_t		BufG;
		uint8_t 	BufB;
	}RcvColor[PARA_COLORNUM_MAX + 1];
	struct COLOR_TYPE
	{
		uint8_t   BufR;
		uint8_t   BufG;
		uint8_t   BufB;
	}Color[PARA_COLORNUM_MAX + 1];
}ModePara_t;


//exported functions
void Display_Control(void);

//file functions
//common functions
uint8_t  Para_Err_Check(ModePara_t * para);
void Display_All_Set(uint8_t r, uint8_t g, uint8_t b);
void Display_Power_Off(void);
void Display_Power_On(void);
void Display_All_Flash(uint8_t r, uint8_t g, uint8_t b);
void Display_Layout_Photo_Ctrl(void);
void Display_Layout_Enter(void);
void Display_Layout_Cancel(void);
void Display_Layout_Test(void);
void Display_Layout_Save(void);
void Display_Music(void);
void Display_Custom_Steady(uint8_t sec);
void Display_Custom_Dynamic(void);

//string functions
void Display_Str_Steady(void);
void Display_Str_Sparkle(void);
void Display_Str_Rainbow(void);


//tree functions
void Display_Tree_Steady(void);
void Display_Tree_Sparkle(void);
void Display_Tree_Rainbow(void);
void Display_Tree_Fade(void);
void Display_Tree_Snow(void);
void Display_Tree_Snake(void);
void Display_Tree_Twinkle(void);
void Display_Tree_Fireworks(void);
void Display_Tree_Vertigo(void);
void Display_Tree_Horizontal(void);
void Display_Tree_Waves(void);
void Display_Tree_Updwn(void);
void Display_Tree_Diagonal(void);
void Display_Tree_Sunset(void);
void Display_Tree_Vintage(void);
void Display_Tree_Glow(void);
void Display_Tree_Color_Rand(void);
void Display_Tree_Carnival(void);
void Display_Tree_Alternate(void);


//music functions
void Music_Mode_Bar(void);



//layout functions
void Display_Layout_None_Init(void);
void Display_Layout_Triangle_Left(void);
void Display_Layout_Triangle_Right(void);


//exported parameters
extern Display_t    display_data;
extern ModePara_t   mode_para_data[];
extern uint8_t 		MusicMode;
extern bool			MusicUpdateFlag;
extern uint8_t		DynamicTimeFlag[];
extern uint8_t 		CustomEnableFlag[];



extern uint8_t           SpeedCtrl;
extern uint8_t           OtherCtrl;
extern uint8_t           TempR;
extern uint8_t           TempG;
extern uint8_t           TempB;
extern uint8_t           TempR1;
extern uint8_t           TempG1;
extern uint8_t           TempB1;
extern uint8_t           TopR;
extern uint8_t           TopG;
extern uint8_t           TopB;
extern uint8_t           FadeR;
extern uint8_t           FadeG;
extern uint8_t           FadeB;
extern uint8_t           TempColor;
extern uint8_t           FadeR1;
extern uint8_t           FadeG1;
extern uint8_t           FadeB1;
extern uint8_t           TempColor1;
extern uint8_t           TempStep;
extern uint8_t           FadeLevel;
extern uint16_t          RptCtrl;
extern uint16_t          RptTotal;
extern sint16_t          RndSeed;
extern uint16_t           HoldTime;
extern uint8_t           LayerStep;
extern uint16_t          LedPickAll;
extern uint8_t           ModeTime[];
extern uint8_t           ModeStep[];
extern uint16_t           LedPick[];
extern bool			  	 ModeFirstFlag;

#endif
