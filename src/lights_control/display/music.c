/**********************************************************
	* FILE NAME		music.c
	* DATE			21 JAN 2019
	* AUTOR			Jacky.Chin
	* BRIEF			Music display function
***********************************************************/
#include "display.h"
#include "music.h"

//gloable paramters
MusicData_T	music_data;

uint8_t	Music_Color_Table[LAYER_MAX][3]=
{
	{240,0,240},
	{160,0,240},
	{80,0,240},
	{0,0,240},
	{0,80,240},
	{0,160,240},
	{0,240,240},
	{0,240,80},
	{0,240,0},
	{120,240,0},
	{240,150,0},
	{240,100,0},
	{240,50,0},
	{240,10,0},
	{240,0,0},
	{240,0,0},
};


//File parameters
uint16_t	MusicExitTime;


/*
 * FUNCTION NAME	Display_Music
 */
void Display_Music(void)
{
	switch (MusicMode)
	{
		case 0:		Music_Mode_Bar();		break;
		default:	Music_Mode_Bar();		break;
	}
}


/*
 * FUNCTION NAME	Music_Mode_Bar
 */
void Music_Mode_Bar(void)
{
	uint16_t	i = 0;
	
	if (display_data.init)
	{
		display_data.init = false;

		#if 0
		FadeLevel = (uint8_t)(1200 / (uint16_t)LayerMax); //1200 = 240 * 5 (from 240,0,0 -> 240,0,240)
		uint16_t	j = 0;
		Music_Color_Table[0][0] = 240;
		Music_Color_Table[0][1] = 0;
		Music_Color_Table[0][2] = 0;
		for (uint8_t i = 0; i < LayerMax; i++)
		{
			
		}
		#endif

		TempStep = 0;

		Display_All_Set(0, 0, 0);

		return;
	}

	
	if (MusicUpdateFlag)
	{
		MusicExitTime = 0;
		for (uint8_t j = 0; j < 2; j++)
		{
			if (TempStep < LayerStep)
			{
				if (TempStep < LayerMax)
				{
					for (i = Layer[TempStep].Head; i <= Layer[TempStep].Tail; i++)
					{
						LedData[i].DutyR = Music_Color_Table[TempStep][0];
						LedData[i].DutyG = Music_Color_Table[TempStep][1];
						LedData[i].DutyB = Music_Color_Table[TempStep][2];
					}
					TempStep++;
				}
			}
			else
			{
				MusicUpdateFlag = false;
			}
		}
	}

	else
	{
		if (TempStep > 0)
		{
			TempStep--;
			for (i = Layer[TempStep].Head; i <= Layer[TempStep].Tail; i++)
			{
				LedData[i].DutyR = 0;
				LedData[i].DutyG = 0;
				LedData[i].DutyB = 0;
			}
		}
		
	}

	MusicExitTime++;
	if (MusicExitTime >= 100)
	{
		MusicExitTime = 0;
		display_data.mode= display_data.mode_buf;
		display_data.init = true;
	}
}







