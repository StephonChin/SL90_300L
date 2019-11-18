/*************************************************************
  *
  * FileName    layout.c
  * Brief       lay out information
  *
**************************************************************/
#include "display.h"
#include "layout.h"

//gloable paramters
Layer_T			vertical_layer;
Layer_T			triangle_layer;
Layer_T			fan_layer;


/*
 * FUNCTION NAME	Display_Layout_Enter
 * BRIEF			Enter the layout, set all led to color white
 */
void Display_Layout_Enter(void)
{

	#if 0
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
	#endif
}


/*
 * FUNCTION NAME	Display_Layout_Cancel
 * BRIEF			Exit the layout mode
 */
void Display_Layout_Cancel(void)
{
#if 0
	if (display_data.init == true)
	{
		display_data.init = false;

		display_data.mode= display_data.mode_buf;
		display_data.init = true;
	}
#endif
}


/*
 * FUNCTION NAME 	Display_Layout_Test
 * BRIEF			Layout test mode
 */
void Display_Layout_Test(void)
{
#if 0
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
#endif
}


/*
 * FUNCTION NAME	Display_Layout_Save
 * BRIEF			Save the layout data
 */
void Display_Layout_Save(void)
{
#if 0
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
#endif
}


void Display_Layout_Photo_Ctrl(void)
{

}


/**
  * FunctionName  Display_Layout_None_Init
  */
void Display_Layout_None_Init(void)
{
	vertical_layer.en_flag = 0;
	vertical_layer.layer_total = STR_LAYER_MAX;

	
	for (uint16_t i = 0; i < STR_LAYER_MAX; i++)
	{
		vertical_layer.head[i] = i * STR_LAYER_SEC;
	}


	for (uint16_t i = 0; i < LED_TOTAL; i++)
	{
		uint8_t layer = i / STR_LAYER_SEC;
		if (layer >= STR_LAYER_MAX)	layer = STR_LAYER_MAX - 1;
		vertical_layer.info[i] = layer;
	}
}



