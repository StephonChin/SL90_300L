/*************************************************************
  *
  * FileName    layout.c
  * Brief       lay out information
  *
**************************************************************/
#include "display.h"
#include "layout.h"

//gloable paramters
LayerBrief_T		layer_brief;
LayerData_T			vertical_layer[50];
LayerData_T			triangle_layer[50];
LayerData_T			fan_layer[50];



/*
 * FUNCTION NAME	Display_Layout_Enter
 * BRIEF			Enter the layout, set all led to color white
 */
void Display_Layout_Enter(void)
{
	uint16_t i = 0;

	if (display_data.init == true)
	{
		display_data.init = false;

		LayerTest = 0;
		for (i = 0; i < LED_TOTAL; i++)
		{
			LedData[i].DutyR = 0xff;
			LedData[i].DutyG = 0xff;
			LedData[i].DutyB = 0xff;
		}

		//set the head and the tail to 0xffff
		for (i = 0; i < LAYER_MAX; i++)
		{
			LayerTemp[i].Head = 0xffff;
			LayerTemp[i].Tail = 0xffff;
		}
	}
}


/*
 * FUNCTION NAME	Display_Layout_Cancel
 * BRIEF			Exit the layout mode
 */
void Display_Layout_Cancel(void)
{
	if (display_data.init == true)
	{
		display_data.init = false;

		display_data.mode= display_data.mode_buf;
		display_data.init = true;
	}
}


/*
 * FUNCTION NAME 	Display_Layout_Test
 * BRIEF			Layout test mode
 */
void Display_Layout_Test(void)
{
	uint16_t i = 0;
	uint16_t j = 0;
	
	if (display_data.init == true)
	{
		display_data.init = false;

		//
		if (LayerTemp[LayerTest].Head > LayerTemp[LayerTest].Tail)
		{
			j = LayerTemp[LayerTest].Head;
			LayerTemp[LayerTest].Tail = LayerTemp[LayerTest].Head;
			LayerTemp[LayerTest].Head = j;
		}

		//supplement the next layer automatically
		if (LayerTest < LAYER_MAX - 1 && LayerTemp[LayerTest].Tail < (LED_TOTAL - 1))
		{
			LayerTemp[LayerTest+1].Head = LayerTemp[LayerTest].Tail + 1;
			LayerTemp[LayerTest+1].Tail = LED_TOTAL - 1;
		}

		printf("=layout=head:0x%x",LayerTemp[LayerTest].Head);
		printf("=layout=tail:0x%x",LayerTemp[LayerTest].Tail);

		//change the layer color
		j = (uint16_t)(LayerTest % 3);
		for (i = 0; i < LED_TOTAL; i++)
		{
			if (i < LayerTemp[LayerTest].Head)	continue;

			if (i >= LayerTemp[LayerTest].Head && i <= LayerTemp[LayerTest].Tail)
			{
				LedData[i].DutyR = 0;
				LedData[i].DutyG = 0;
				LedData[i].DutyB = 0;
				if (j == 0)
				{
					LedData[i].DutyR = 0xff;
				}
				else if (j == 1)
				{
					LedData[i].DutyG = 0xff;
				}
				else
				{
					LedData[i].DutyB = 0xff;
				}
			}
			else
			{
				LedData[i].DutyR = 0xff;
				LedData[i].DutyG = 0xff;
				LedData[i].DutyB = 0xff;
			}
		}
	}
}


/*
 * FUNCTION NAME	Display_Layout_Save
 * BRIEF			Save the layout data
 */
void Display_Layout_Save(void)
{
	uint16_t i = 0;
	
	if (display_data.init == true)
	{
		display_data.init = false;

		for (i = 0; i < LayerTest; i++)
		{
			Layer[i].Head = LayerTemp[i].Head;
			Layer[i].Tail = LayerTemp[i].Tail;
		}

		LayerMax = LayerTest;
		
		display_data.mode= GREEN_FLASH;
		display_data.init = true;
	}
}


void Display_Layout_Photo_Ctrl(void)
{

}


/**
  * FunctionName  Display_Layout_None_Init
  */
void Display_Layout_None_Init(void)
{
	for (uint16_t i = 0; i < STR_LAYER_MAX; i++)
	{
		Layer[i].Head = i * STR_LAYER_SEC;
		Layer[i].Tail = (i+1) * STR_LAYER_SEC - 1;
	}

	Layer[STR_LAYER_MAX - 1].Tail = LED_TOTAL - 1;

	LayerMax = STR_LAYER_MAX;

	#if 0
	LayerMax = 18;

	Layer[0].Head = 0;
	Layer[0].Tail = 34;

	Layer[1].Head = 35;
	Layer[1].Tail = 57;

	Layer[2].Head = 58;
	Layer[2].Tail = 88;

	Layer[3].Head = 89;
	Layer[3].Tail = 117;

	Layer[4].Head = 118;
	Layer[4].Tail = 144;

	Layer[5].Head = 145;
	Layer[5].Tail = 166;

	Layer[6].Head = 167;
	Layer[6].Tail = 183;

	Layer[7].Head = 184;
	Layer[7].Tail = 198;

	Layer[8].Head = 199;
	Layer[8].Tail = 213;

	Layer[9].Head = 214;
	Layer[9].Tail = 226;

	Layer[10].Head = 227;
	Layer[10].Tail = 239;

	Layer[11].Head = 240;
	Layer[11].Tail = 252;

	Layer[12].Head = 253;
	Layer[12].Tail = 263;

	Layer[13].Head = 264;
	Layer[13].Tail = 274;

	Layer[14].Head = 275;
	Layer[14].Tail = 285;

	Layer[15].Head = 286;
	Layer[15].Tail = 294;

	Layer[16].Head = 295;
	Layer[16].Tail = 299;

	Layer[17].Head = 300;
	Layer[17].Tail = 300;
	#endif
}






/**
  * FunctionName  Display_Layout_Triangle_Left
  */
void Display_Layout_Triangle_Left(void)
{

}


/**
  * FunctionName  Display_Layout_Triangle_Right
  */
void Display_Layout_Triangle_Right(void)
{

}


