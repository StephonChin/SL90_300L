/*****************************************************************
  *
  * FileName    Key.c
  * Brief       scan the key status
  *
******************************************************************/
#include "Key.h"

//global prarameters
_TypeKey   KeyMode, KeyColor;

//define
#define	KEY_POWER_ON_DELAY		150		//1.5s	

/***********************************
 Key_Scan
 ***********************************
    > Input
        ==> None
    > Output
        ==> None
    > Brief
        ==> Scan the mode key and color key status
*************************************/
void Key_Scan(void)
{
	static uint16_t   KeyModeTime = 0;
	static uint16_t   KeyColorTime = 0;
	static uint8_t    KeyModeFlag = 0;
	static uint8_t    KeyColorFlag = 0;

	static uint16_t	PowerOnDelay = 0;

	//delay 1.5s(do not change the mode when press mode key to reset the wifi mode)
	if (PowerOnDelay < KEY_POWER_ON_DELAY)
	{
		PowerOnDelay++;
		return;
	}

	if(0 == digitalRead(MODE_PIN))
	{
		if (KeyModeFlag == 0)
		{
			KeyModeTime++;
			if (KeyModeTime >= 250)
			{
				KeyModeTime = 0;
				KeyModeFlag = 1;
				KeyMode = KEY_LONG;
			}
		}
	}
	else
	{
		if (KeyModeFlag == 0 && KeyModeTime > 0 && KeyModeTime < 100)
		{
			KeyMode = KEY_SHORT;
		}
		
		KeyModeTime = 0;
		KeyModeFlag = 0;
	}

	if(0 == digitalRead(COLOR_PIN))
	{
		if (KeyColorFlag == 0)
		{
			KeyColorTime++;
			if (KeyColorTime >= 75)
			{
				KeyColorTime = 0;
				KeyColorFlag = 1;
				KeyColor = KEY_LONG;
			}
		}
	}
	else
	{
		if (KeyColorFlag == 0 && KeyColorTime > 0)
		{
			KeyColor = KEY_SHORT;
		}
		
		KeyColorTime = 0;
		KeyColorFlag = 0;
	}
}

