/********************************************************************
  *
  * FileName    tree_mode.c
  * Brief       the mode when the product have been lay out
  *
*********************************************************************/
#include "display.h"


static void Shift_N(Layer_T *p, uint8_t *r, uint8_t *g, uint8_t *b);
static void Shift_P(Layer_T *p, uint8_t *r, uint8_t *g, uint8_t *b);
static void Shift_Vertical_UpDown(uint8_t *r, uint8_t *g, uint8_t *b);
static void Shift_Vertical_DownUp(uint8_t *r, uint8_t *g, uint8_t *b);
static void Shift_Triangle_RightLeft(uint8_t *r, uint8_t *g, uint8_t *b);
static void Shift_Fan_LeftRight(uint8_t *r, uint8_t *g, uint8_t *b);



static void Shift_N(Layer_T *p, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint16_t led = 0;
	uint16_t led1 =0;

	for (uint8_t i = 0; i < p->layer_total - 1; i++)
	{
		led = p->head[i];
		led1 = p->head[i+1];
		LedData[led].DutyR = LedData[led1].DutyR;
		LedData[led].DutyG = LedData[led1].DutyG;
		LedData[led].DutyB = LedData[led1].DutyB;
		
	}
	led = p->head[p->layer_total-1];
	LedData[led].DutyR = *r;
	LedData[led].DutyG = *g;
	LedData[led].DutyB = *b;

	for (uint16_t i = 0; i < LED_TOTAL; i++)
	{
		uint8_t layer = p->info[i];
		led = p->head[layer];
		LedData[i].DutyR = LedData[led].DutyR;
		LedData[i].DutyG = LedData[led].DutyG;
		LedData[i].DutyB = LedData[led].DutyB;
	}
}
static void Shift_P(Layer_T *p, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint16_t led = 0;
	uint16_t led1 =0;

	for (uint8_t i = p->layer_total - 1; i > 0; i--)
	{
		led = p->head[i];
		led1 = p->head[i-1];
		LedData[led].DutyR = LedData[led1].DutyR;
		LedData[led].DutyG = LedData[led1].DutyG;
		LedData[led].DutyB = LedData[led1].DutyB;
		
	}
	led = p->head[0];
	LedData[led].DutyR = *r;
	LedData[led].DutyG = *g;
	LedData[led].DutyB = *b;

	for (uint16_t i = 0; i < LED_TOTAL; i++)
	{
		uint8_t layer = p->info[i];
		led = p->head[layer];
		LedData[i].DutyR = LedData[led].DutyR;
		LedData[i].DutyG = LedData[led].DutyG;
		LedData[i].DutyB = LedData[led].DutyB;
	}
}


static void Shift_Vertical_UpDown(uint8_t *r, uint8_t *g, uint8_t *b)
{
	Shift_N(&vertical_layer, r, g, b);
}
static void Shift_Vertical_DownUp(uint8_t *r, uint8_t *g, uint8_t *b)
{
	Shift_P(&vertical_layer, r, g, b);
}
static void Shift_Triangle_RightLeft(uint8_t *r, uint8_t *g, uint8_t *b)
{
	Shift_N(&triangle_layer, r, g, b);
}
static void Shift_Fan_LeftRight(uint8_t *r, uint8_t *g, uint8_t *b)
{
	Shift_N(&fan_layer, r, g, b);
}



/**
  * FunctionName    Display_str_steady
  */
void Display_Tree_Steady(void)
{
	uint16_t temp = 0;
	
	if(display_data.init == TRUE)
	{	
		display_data.init = FALSE;

		Para_Err_Check(&mode_para_data[STEADY]);

		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[STEADY].Bright + 1;

		SpeedCtrl = 0;
		OtherCtrl = 0;
		TempColor = 0;

		for (temp = 0; temp < LED_TOTAL; temp++)
		{
			TempColor = temp % mode_para_data[STEADY].ColorNum;
			LedData[temp].DutyR = mode_para_data[STEADY].Color[TempColor].BufR;
			LedData[temp].DutyG = mode_para_data[STEADY].Color[TempColor].BufG;
			LedData[temp].DutyB = mode_para_data[STEADY].Color[TempColor].BufB;
		}
	}
}



void Display_Tree_Rainbow(void)
{
  uint16_t temp = 0;  
  uint16_t	i = 0;
  uint16_t	j = 0;
  
	if(display_data.init == true)
	{	
    display_data.init = false;

    Para_Err_Check(&mode_para_data[RAINBOW]);
    
    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[RAINBOW].Bright + 1;
    
	SpeedCtrl = 0;
    OtherCtrl = 0;
    TempColor = 0;
    TempStep  = 0;
    
	TempR = 0;
    TempG = 0;
    TempB = 0;
    TopR = 240;
    TopG = 240;
    TopB = 240;

    switch (mode_para_data[RAINBOW].Other){
      default:    FadeR = 12;	TopR = 240;      break;
      case 9:     FadeR = 10;	TopR = 250;      break;
      case 8:     FadeR = 8;	TopR = 240;      break;
      case 7:     FadeR = 7; 	TopR = 245;     break;
      case 6:     FadeR = 6; 	TopR = 240;     break;
      case 5:     FadeR = 5; 	TopR = 225;     break;
      case 4:     FadeR = 5; 	TopR = 250;     break;
      case 3:     FadeR = 4; 	TopR = 220;     break;
      case 2:     FadeR = 4; 	TopR = 240;     break;
      case 1:     FadeR = 3; 	TopR = 210;     break;
      case 0:     FadeR = 3; 	TopR = 240;     break;
    }
    
    FadeG = FadeR;
    FadeB = FadeR;
    TopG = TopR;
    TopB = TopR;

    Display_All_Set(0,0,0);

    ModeFirstFlag = true;
    return;
	}
  
  
  
  SpeedCtrl++;
  //if (SpeedCtrl > ((PARA_SPEED_MAX - mode_para_data[RAINBOW].Speed) * (uint8_t)(1 - ModeFirstFlag)))
  {
    SpeedCtrl = 0;

	if (ModeFirstFlag)
	{
		ModeFirstFlag = false;
    	j = LED_TOTAL;
    }
    else
    {
		j = mode_para_data[RAINBOW].Speed + 1;
    }

	for (i = 0; i < j; i++)
	{
	    switch (TempStep)
	   {
	      case 0:
	     {
	        if (TempR < TopR)		TempR += FadeR;
	        if(TempR == TopR)		TempStep++;
	      } break;
	      
	      case 1:
	     {
	        if (TempG < TopG)		TempG += FadeG;
	        if (TempG == TopG)		TempStep++;
	      } break;
	      
	      case 2:{
	        if (TempR > 0)			TempR -= FadeR;
	        if (TempR == 0)			TempStep++;
	      } break;
	      
	      case 3:{
	        if (TempB < TopB)		TempB += FadeB;
	        if (TempB == TopB)		TempStep++;
	      } break;
	      
	      case 4:{
	        if (TempG > 0)			TempG -= FadeG;
	        if (TempG == 0)			TempStep++;
	      } break;
	      
	      case 5:{
	        if (TempR < TopR)		TempR += FadeR;
	        if (TempR == TopR)		TempStep++;
	      } break;
	      
	      case 6:{
	        if (TempB > 0)			TempB -= FadeB;
	        if (TempB == 0)			TempStep = 1;
	      } break;
	    }
	    
	    Shift_Vertical_UpDown(&TempR, &TempG, &TempB);
	}
  }
}

void Display_Tree_Fade(void)
{
	if (display_data.init == TRUE)
	{
		display_data.init = FALSE;

		Para_Err_Check(&mode_para_data[FADE]);
		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[FADE].Bright + 1;

		TempColor = 0;
		TempStep = 0;
		SpeedCtrl = 0;
		HoldTime = 0;
		TempR = mode_para_data[FADE].Color[TempColor].BufR;
		TempG = mode_para_data[FADE].Color[TempColor].BufG;
		TempB = mode_para_data[FADE].Color[TempColor].BufB;
		Display_All_Set(TempR, TempG, TempB);

		if (mode_para_data[FADE].ColorNum == 1)
		{
			TopR = 0;
			TopG = 0;
			TopB = 0;
		}
		else 
		{
			TempColor = 1;
			TopR = mode_para_data[FADE].Color[TempColor].BufR;
			TopG = mode_para_data[FADE].Color[TempColor].BufG;
			TopB = mode_para_data[FADE].Color[TempColor].BufB;
		}

		//Get the fade level
		if (TopR >= TempR)    FadeR = (TopR - TempR) / FADE_LEVEL;
		else                  FadeR = (TempR - TopR) / FADE_LEVEL;

		if (TopG >= TempG)    FadeG = (TopG - TempG) / FADE_LEVEL;
		else                  FadeG = (TempG - TopG) / FADE_LEVEL;

		if (TopB >= TempB)    FadeB = (TopB - TempB) / FADE_LEVEL;
		else                  FadeB = (TempB - TopB) / FADE_LEVEL;

		return;
	}



	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[FADE].Speed))
	{
		SpeedCtrl = 0;
		switch (TempStep)
		{
			//fade
			case 0:
			{
				if (TempR > TopR)       TempR -= FadeR;
				else if (TopR > TempR)  TempR += FadeR;

				if (TempG > TopG)       TempG -= FadeG;
				else if (TopG > TempG)  TempG += FadeG;

				if (TempB > TopB)       TempB -= FadeB;
				else if (TopB > TempB)  TempB += FadeB;

				if (TempR == TopR && TempG == TopG && TempB == TopB)
				{
					TempStep++;
				}
			} break;

			//hold
			case 1:
			{
				if (mode_para_data[FADE].ColorNum == 1)
				{
					TempStep++;
				}
				else
				{
					HoldTime++;
					if (HoldTime >= 10)
					{
						HoldTime = 0;
						TempStep++;
					}
				}
			}break;

			default:
			{
				if (mode_para_data[FADE].ColorNum == 1)
				{
					TopR = mode_para_data[FADE].Color[0].BufR - TempR;
					TopG = mode_para_data[FADE].Color[0].BufG - TempG;
					TopB = mode_para_data[FADE].Color[0].BufB - TempB;
				}
				else
				{
					TempColor++;
					if (TempColor >= mode_para_data[FADE].ColorNum)   TempColor = 0;
					TopR = mode_para_data[FADE].Color[TempColor].BufR;
					TopG = mode_para_data[FADE].Color[TempColor].BufG;
					TopB = mode_para_data[FADE].Color[TempColor].BufB;
				}

				//Get the fade value again
				if (TopR >= TempR)    FadeR = (TopR - TempR) / FADE_LEVEL;
				else                  FadeR = (TempR - TopR) / FADE_LEVEL;

				if (TopG >= TempG)    FadeG = (TopG - TempG) / FADE_LEVEL;
				else                  FadeG = (TempG - TopG) / FADE_LEVEL;

				if (TopB >= TempB)    FadeB = (TopB - TempB) / FADE_LEVEL;
				else                  FadeB = (TempB - TopB) / FADE_LEVEL;


				TempStep = 0;
			} break;
		}
		Display_All_Set(TempR, TempG, TempB);
	}
}



/*
 * FunctionName  Display_str_sparkle
 */
#define SPARKLE_DARK_VAL		3
#define SPARKLE_GRP_CNT			3
void Display_Tree_Sparkle(void)
{
	if (display_data.init == true)
	{
		display_data.init = false;

		Para_Err_Check(&mode_para_data[SPARKLE]);

		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[SPARKLE].Bright + 1;

		SpeedCtrl = 0;
		OtherCtrl = 0;
		TempColor = 0;
		
		for (uint16_t i = 0; i < LED_TOTAL; i++)
		{
			TempColor = i % mode_para_data[SPARKLE].ColorNum;
			LedData[i].DutyR = mode_para_data[SPARKLE].Color[TempColor].BufR / SPARKLE_DARK_VAL;
			LedData[i].DutyG = mode_para_data[SPARKLE].Color[TempColor].BufG / SPARKLE_DARK_VAL;
			LedData[i].DutyB = mode_para_data[SPARKLE].Color[TempColor].BufB / SPARKLE_DARK_VAL;

			ModeTime[i] = 0;
			LedPick[i] = 0xffff;
		}

		//get the group count
		switch(mode_para_data[SPARKLE].Other)
		{
			case 0:		RptTotal = 7;		break;
			case 1:		RptTotal = 8;		break;
			case 2:		RptTotal = 9;		break;
			case 3:		RptTotal = 10;		break;
			case 4:		RptTotal = 11;		break;
			case 5:		RptTotal = 12;		break;
			case 6:		RptTotal = 13;		break;
			case 7:		RptTotal = 14;		break;
			case 8:		RptTotal = 15;		break;
			case 9:		RptTotal = 16;		break;
			default:	RptTotal = 17;		break;
		}
		

		//first , random 5 group led different numbers and save them to LedPick
		//at the same time, lighten the leds
		for (uint8_t i = 0; i < SPARKLE_GRP_CNT * RptTotal; i++)
		{
			uint16_t j = 0;
			do
			{
				RAND();
				j = (uint16_t)rand() % LED_TOTAL;
			}while(LedPick[i] == j);
			LedPick[i] = j;			//LedPick save the led number

			LedData[j].DutyR *= SPARKLE_DARK_VAL;
			LedData[j].DutyG *= SPARKLE_DARK_VAL;
			LedData[j].DutyB *= SPARKLE_DARK_VAL;
		}
		TempStep = 0;

		return;
	}


	//every 50ms, darken one group LED
	for (OtherCtrl = 0; OtherCtrl < RptTotal; OtherCtrl++)
	{
		uint16_t picknum = TempStep + OtherCtrl * SPARKLE_GRP_CNT;
		
		uint16_t led_num = LedPick[picknum];
		LedData[led_num].DutyR /= SPARKLE_DARK_VAL;
		LedData[led_num].DutyG /= SPARKLE_DARK_VAL;
		LedData[led_num].DutyB /= SPARKLE_DARK_VAL;	

		//get new led numbers and lighten them
		bool rpt_flag = false;
		while(1)
		{
			RAND();
			led_num = (uint16_t)rand() % LED_TOTAL;

			rpt_flag = false;
			for (uint8_t i = 0; i < SPARKLE_GRP_CNT * RptTotal; i++)
			{
				if (LedPick[i] == led_num)
				{
					rpt_flag = true;
				}
				if (rpt_flag)	break;		//exit for()
			}

			if (rpt_flag)	
			{
				continue;	//continue while
			}
			else
			{
				LedPick[picknum] = led_num;
				LedData[led_num].DutyR *= SPARKLE_DARK_VAL;
				LedData[led_num].DutyG *= SPARKLE_DARK_VAL;
				LedData[led_num].DutyB *= SPARKLE_DARK_VAL;
				break;		//exit while
			}
		}
	}

	//increase the darken led parameter
	TempStep++;
	if (TempStep >= SPARKLE_GRP_CNT)	TempStep = 0;
	
	
	return;







	
	#if 0
	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[SPARKLE].Speed))
	{
		SpeedCtrl = 0;

		for (i = 0; i < LED_TOTAL; i++)
		{
			if (LedPick[i] == 1)
			{
				LedPick[i] = 0;
				LedData[i].DutyR /= 3;
				LedData[i].DutyG /= 3;
				LedData[i].DutyB /= 3;
			}
		}

		uint16_t  j = 0;
		uint16_t timeout = LED_TOTAL;
		for (i = 0; i < (mode_para_data[SPARKLE].Other + 1) * 4; i++)
		{
			timeout = LED_TOTAL * 5;
			do
			{
				timeout--;
				RAND();
				j = (uint16_t)(rand() % LED_TOTAL);
			} while((LedPick[j] == 1) && timeout);

			//if get valid led number
			if (timeout != 0)
			{
				LedPick[j] = 1;
				LedData[j].DutyR *= 3;
				LedData[j].DutyG *= 3;
				LedData[j].DutyB *= 3;
			}
		}
	}
	#endif
}


/**
  * FunctionName  Display_Tree_Snow
  */
void Display_Tree_Snow(void)
{
#if 1
	if(display_data.init == true)
	{	
	    display_data.init = false;

	    Para_Err_Check(&mode_para_data[SNOW]);
	    
	    //BrightLevel = PARA_BRIGHT_MAX - ParaData[SNOW].Bright + 1;
		BrightLevel=1;
		
		SpeedCtrl = 0;
	    OtherCtrl = 0;
	    TempColor = 0;
	    TempStep  = 0;
	    LayerStep = 0;
	    
		TempR = mode_para_data[SNOW].Color[TempColor].BufR;
	    TempG = mode_para_data[SNOW].Color[TempColor].BufG;
	    TempB = mode_para_data[SNOW].Color[TempColor].BufB;
	    
	    TopR = TempR;
	    TopG = TempG;
	    TopB = TempB;
	    
	    FadeR = TopR / FADE_LEVEL;
	    FadeG = TopG / FADE_LEVEL;
	    FadeB = TopB / FADE_LEVEL;
	    
	    
	    Display_All_Set(0,0,0);
	    return;
	}
  
  
	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[SNOW].Speed + 2))
	{
		SpeedCtrl = 0;

	    switch (TempStep)
	    {
			case 0:
			{
				if (LayerStep < vertical_layer.layer_total)
				{
					TempR = TopR;
					TempG = TopG;
					TempB = TopB;
					LayerStep++;
				}
				else
				{
					TempStep++;
					LayerStep = 0;
				}
			}break;

			case 1:
			{
				if (TempR > 0 || TempG > 0 || TempB > 0)
				{
					TempR -= FadeR;
					TempG -= FadeG;
					TempB -= FadeB;
				}
				else
				{
					TempStep++;
				}
			}break;

			case 2:
			{
				if (LayerStep < vertical_layer.layer_total)
				{
					LayerStep++;
				}
				else
				{
					TempStep++;
				}
			}break;

			default:
			{
				TempStep = 0;
				LayerStep = 0;
				TempColor++;
				if (TempColor >= mode_para_data[SNOW].ColorNum)
				{
					TempColor = 0;
				}

				TopR = mode_para_data[SNOW].Color[TempColor].BufR;
				TopG = mode_para_data[SNOW].Color[TempColor].BufG;
				TopB = mode_para_data[SNOW].Color[TempColor].BufB;
				FadeR = TopR / FADE_LEVEL;
				FadeG = TopG / FADE_LEVEL;
				FadeB = TopB / FADE_LEVEL;
			}break;
	    }

		Shift_Vertical_UpDown(&TempR, &TempG, &TempB);
    }
#else
	bool  RandFlag = false;
	uint16_t temp = 0;  
	uint16_t temp2 = 0;
	uint16_t temp3 = 0;
	uint16_t temp4 = 0;
  
	if(display_data.init == true){	
	    display_data.init = false;

	    Para_Err_Check(&mode_para_data[SNOW]);
	    
	    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[SNOW].Bright + 1;
	    
		SpeedCtrl = 0;
	    OtherCtrl = 0;
	    TempColor = 0;
	    TempStep  = 0;
	    
		TempR = mode_para_data[SNOW].Color[TempColor].BufR;
	    TempG = mode_para_data[SNOW].Color[TempColor].BufG;
	    TempB = mode_para_data[SNOW].Color[TempColor].BufB;
	    
	    TopR = TempR;
	    TopG = TempG;
	    TopB = TempB;
	    
	    FadeR = TopR / FADE_LEVEL;
	    FadeG = TopG / FADE_LEVEL;
	    FadeB = TopB / FADE_LEVEL;
	    
	    RptTotal = SNOW_FREQ;
	    RptCtrl = 0;
	    
	    Display_All_Set(0,0,0);
	    return;
	}
  
  
  SpeedCtrl++;
  if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[SNOW].Speed)){
    SpeedCtrl = 0;

	  for (temp = 0; temp < LED_TOTAL; temp++){
	    TempR = LedData[temp].DutyR;
	    TempG = LedData[temp].DutyG;
	    TempB = LedData[temp].DutyB;
	    
	    //Fade out
	    if (TempR > 0)    LedData[temp].DutyR -= FadeR;
	    if (TempG > 0)    LedData[temp].DutyG -= FadeG;
	    if (TempB > 0)    LedData[temp].DutyB -= FadeB;
	  }
      
  
    //begining
    if (TempStep == 0){
      for (temp = Layer[LayerMax - 1].Head; temp <= Layer[LayerMax - 1].Tail; temp++){
        LedData[temp].DutyR = TopR;
        LedData[temp].DutyG = TopG;
        LedData[temp].DutyB = TopB;
      }
      TempStep++;
    }
    
    //section = SNOW_LONG layer
    else if (TempStep < SNOW_LONG){
      temp2 = LayerMax - 1;
      temp3 = LayerMax - TempStep;
      TempStep++;
      RandFlag = TRUE;
    } 
    
    //to bottom
    else if (TempStep < LayerMax){
      temp2 = (LayerMax - 1) + (SNOW_LONG - 1) - TempStep;
      temp3 = (LayerMax - 1) - TempStep;
      TempStep++;
      RandFlag = TRUE;
    }
    
    //to bottom 
    else if (TempStep < (LayerMax + SNOW_LONG - 1 - SNOW_BOTTOM)){
      temp2 = (LayerMax - 1) + (SNOW_LONG - 1) - TempStep;
      temp3 = 0;
      TempStep++;
      RandFlag = TRUE;
    }
    
    //Hold
    else if(TempStep < (LayerMax + SNOW_LONG - SNOW_BOTTOM - 1 + SNOW_BOTTOM_HOLD))
    {
      temp2 = SNOW_BOTTOM;
      temp3 = 0;
      TempStep++;
      RandFlag = TRUE;
    }
    else if(TempStep < (LayerMax + SNOW_LONG - 1 + SNOW_BOTTOM_HOLD))
    {
      temp2 = LayerMax - 1 + SNOW_LONG - 1 + SNOW_BOTTOM_HOLD - TempStep;
      temp3 = 0;
      TempStep++;
      RandFlag = TRUE;
    }
    else if (TempStep < (LayerMax + SNOW_LONG - 1 + SNOW_BOTTOM_HOLD + 5)){
      TempStep++;
    }
    else{
      TempStep = 0;
    }
    
    if (RandFlag){
      RptCtrl = RptTotal;
      while (RptCtrl > 0){
        RAND();
        temp4 = Layer[temp2].Tail - Layer[temp3].Head + 1;
        temp = (uint16_t)((uint16_t)rand() % temp4) + Layer[temp3].Head;
        LedData[temp].DutyR = TopR;
        LedData[temp].DutyG = TopG;
        LedData[temp].DutyB = TopB;
        RptCtrl--;
      }
    }
  }
 #endif
}


/**
  * FunctionName  Display_Tree_Snake
  */

void Display_Tree_Snake(void)
{
	uint16_t temp = 0; 
	uint16_t temp1 = 0;
	uint16_t	j = 0;

	if(display_data.init == true)
	{	
		display_data.init = false;

		Para_Err_Check(&mode_para_data[SNAKE]);

		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[SNAKE].Bright + 1;

		SpeedCtrl = 0;
		OtherCtrl = 0;
		TempColor = 0;
		TempStep  = 0;
		RptCtrl = 0;
		TempColor = 0;

		TempR = mode_para_data[SNAKE].Color[TempColor].BufR;
		TempG = mode_para_data[SNAKE].Color[TempColor].BufG;
		TempB = mode_para_data[SNAKE].Color[TempColor].BufB;
		TopR = TempR;
		TopG = TempG;
		TopB = TempB;

		switch (mode_para_data[SNAKE].Other)
		{
			case 0:     RptTotal = 22;   break;
			case 1:     RptTotal = 20;   break;
			case 2:     RptTotal = 18;   break;
			case 3:     RptTotal = 16;   break;
			case 4:     RptTotal = 14;   break;
			case 5:     RptTotal = 12;   break;
			case 6:     RptTotal = 10;   break;
			case 7:     RptTotal = 8;   break;
			case 8:     RptTotal = 6;   break;
			case 9:     RptTotal = 4;   break;
			default:    RptTotal = 2;   break;
		}

		FadeLevel = FADE_LEVEL;

		FadeR = TempR / FadeLevel;
		FadeG = TempG / FadeLevel;
		FadeB = TempB / FadeLevel;

		Display_All_Set(0,0,0);

		ModeFirstFlag = true;

		return;
	}

	if (ModeFirstFlag)
	{
		ModeFirstFlag = false;
		j = LED_TOTAL * 2;
	}
	else
	{
		j = 4;
	}

	//Repeat twince in order to increase the speed
	for (temp1 = 0; temp1 < j; temp1++)
	{
		SpeedCtrl++;
		if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[SNAKE].Speed) * (uint8_t)(1 - ModeFirstFlag))
		{
			SpeedCtrl = 0;

			switch (TempStep)
			{
				case 0:
				{
					if (TempR > 0 || TempG > 0 || TempB > 0)
					{
						RptCtrl++;
						if (RptCtrl > RptTotal)
						{
							RptCtrl = 0;
							TempR -= FadeR;
							TempG -= FadeG;
							TempB -= FadeB;
						}
					}

					//Space control between previous and the next
					else
					{
						RptCtrl++;
						if (RptCtrl > ((14 - mode_para_data[SNAKE].Other)))
						{
							RptCtrl = 0;
							TempStep++;
						}
					}
				}break;

				default:
				{
					TempColor++;
					if (TempColor >= mode_para_data[SNAKE].ColorNum)    TempColor = 0;
					TempR = mode_para_data[SNAKE].Color[TempColor].BufR;
					TempG = mode_para_data[SNAKE].Color[TempColor].BufG;
					TempB = mode_para_data[SNAKE].Color[TempColor].BufB;
					FadeR = TempR / FadeLevel;
					FadeG = TempG / FadeLevel;
					FadeB = TempB / FadeLevel;
					TempStep = 0;
				} break;
			}

			for (temp = LED_TOTAL - 1; temp > 0; temp--)
			{
				LedData[temp].DutyR = LedData[temp - 1].DutyR;
				LedData[temp].DutyG = LedData[temp - 1].DutyG;
				LedData[temp].DutyB = LedData[temp - 1].DutyB;
			}

			LedData[0].DutyR = TempR;
			LedData[0].DutyG = TempG;
			LedData[0].DutyB = TempB;
		}
	}
}



void Display_Tree_Twinkle(void)
{
	uint16_t  i = 0;
	uint16_t  j = 0;
	uint16_t	l = 0;
	uint16_t	m = 0;

	if (display_data.init == true)
	{
		display_data.init = false;

		Para_Err_Check(&mode_para_data[TWINKLE]);

		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[TWINKLE].Bright + 1;

		SpeedCtrl = 0;
		OtherCtrl = 0;
		TempColor = 0;

		for (i = 0; i < LED_TOTAL; i++)
		{
			TempColor = i % mode_para_data[TWINKLE].ColorNum;
			FadeR = mode_para_data[TWINKLE].Color[TempColor].BufR / FADE_LEVEL;
			FadeG = mode_para_data[TWINKLE].Color[TempColor].BufG / FADE_LEVEL;
			FadeB = mode_para_data[TWINKLE].Color[TempColor].BufB / FADE_LEVEL;
			RAND();
			j = (uint8_t)rand() % FADE_LEVEL;
			LedData[i].DutyR = FadeR * j;
			LedData[i].DutyG = FadeG * j;
			LedData[i].DutyB = FadeB * j;
			ModeStep[i] = 0;
			ModeTime[i] = 0;
			LedPick[i]  = 0;

			ModeFirstFlag = true;
		}

		return;
	}

	if (ModeFirstFlag)
	{
		ModeFirstFlag = false;
		m = 80;
	}
	else
	{
		m = 1;
	}

	for (l = 0; l < m; l++)
	{
		for (i = 0; i < LED_TOTAL; i++)
		{
			TempColor = i % mode_para_data[TWINKLE].ColorNum;
			FadeR = mode_para_data[TWINKLE].Color[TempColor].BufR / FADE_LEVEL;
			FadeG = mode_para_data[TWINKLE].Color[TempColor].BufG / FADE_LEVEL;
			FadeB = mode_para_data[TWINKLE].Color[TempColor].BufB / FADE_LEVEL;

			//Fade in and fade out
			if (LedPick[i] == 0)
			{
				ModeTime[i]++;
				if (ModeTime[i] >= ((i % 6) + 2))
				{
					ModeTime[i] = 0;
					switch (ModeStep[i])
					{
						case 0:
						{
							if (LedData[i].DutyR > 0 || LedData[i].DutyG > 0 || LedData[i].DutyB > 0)
							{
								LedData[i].DutyR -= FadeR;
								LedData[i].DutyG -= FadeG;
								LedData[i].DutyB -= FadeB;
							}
							else
							{
								ModeStep[i]++;
							}
						} break;

						default:
							{
							if (LedData[i].DutyR < (FadeR * FADE_LEVEL) || 
								LedData[i].DutyG < (FadeG * FADE_LEVEL) || 
								LedData[i].DutyB < (FadeB * FADE_LEVEL))
							{
								LedData[i].DutyR += FadeR;
								LedData[i].DutyG += FadeG;
								LedData[i].DutyB += FadeB;
							}
							else
							{
								ModeStep[i] = 0;
							}
						} break;
					}
				}
			}

			//flash
			else
			{
				ModeTime[i]++;
				if (ModeTime[i] >= (i % 8) + 2)
				{
					ModeTime[i] = 0;
					switch (ModeStep[i])
					{
						case 0:
						{
							LedData[i].DutyR = FadeR * FADE_LEVEL;
							LedData[i].DutyG = FadeG * FADE_LEVEL;
							LedData[i].DutyB = FadeB * FADE_LEVEL;
							ModeStep[i]++;
						} break;

						default:
						{
							LedData[i].DutyR = 0;
							LedData[i].DutyG = 0;
							LedData[i].DutyB = 0;
							ModeStep[i] = 0;
							LedPick[i] = 0;
						}break;
					}
				}
			}
		}
	}

	//get random leds to flash
	SpeedCtrl++;
	if (SpeedCtrl >= 30)
	{
		SpeedCtrl = 0;

		uint16_t	timeout = LED_TOTAL;
		for (i = 0; i < (mode_para_data[TWINKLE].Other + 1); i++)
		{
			timeout = LED_TOTAL * 5;
			do
			{
				timeout--;
				RAND();
				j = (uint16_t)(rand() % LED_TOTAL);
			} while((LedPick[j] == 1) && timeout);

			if (timeout != 0)
			{
				LedPick[j] = 1;
				ModeStep[j] = 0;
				ModeTime[j] = 0;
				TempColor = j % mode_para_data[TWINKLE].ColorNum;
				LedData[j].DutyR = mode_para_data[TWINKLE].Color[TempColor].BufR;
				LedData[j].DutyG = mode_para_data[TWINKLE].Color[TempColor].BufG;
				LedData[j].DutyB = mode_para_data[TWINKLE].Color[TempColor].BufB;
			}
		}
	}
}


void Display_Tree_Fireworks(void)
{
  uint16_t i = 0, j = 0;
  
  if (display_data.init == true){
    display_data.init = false;

    Para_Err_Check(&mode_para_data[FIREWORKS]);
    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[FIREWORKS].Bright + 1;

    SpeedCtrl = 0;
    TempStep = 0;
    TempColor = 0;
    LayerStep = 0;
    RptCtrl = 0;
    FadeLevel = 0;
    Display_All_Set(0, 0, 0);
    return;
  }

  
  switch (TempStep){
    //turn on from bottom to top layer
    case 0:{
      SpeedCtrl++;
      if (SpeedCtrl > PARA_SPEED_MAX - mode_para_data[FIREWORKS].Speed){
        SpeedCtrl = 0;
        for (i = 0; i < LED_TOTAL; i++)
        {
        	if (LayerStep == vertical_layer.info[i])
        	{
          		LedData[i].DutyR = mode_para_data[FIREWORKS].Color[TempColor].BufR;
          		LedData[i].DutyG = mode_para_data[FIREWORKS].Color[TempColor].BufG;
          		LedData[i].DutyB = mode_para_data[FIREWORKS].Color[TempColor].BufB;
          	}
        }

        LayerStep++;
        if (LayerStep >= vertical_layer.layer_total){
          TempStep++;
          FadeR = mode_para_data[FIREWORKS].Color[TempColor].BufR / FADE_LEVEL;
          FadeG = mode_para_data[FIREWORKS].Color[TempColor].BufG / FADE_LEVEL;
          FadeB = mode_para_data[FIREWORKS].Color[TempColor].BufB / FADE_LEVEL;
          FadeLevel = FADE_LEVEL;
        }
      }
    }break;

    //fade out to 1/2
    case 1:{
      if (FadeLevel > FADE_LEVEL / 2)
      {
        FadeLevel--;
        for (i = 0; i < LED_TOTAL; i++){
          LedData[i].DutyR = FadeR * FadeLevel;
          LedData[i].DutyG = FadeG * FadeLevel;
          LedData[i].DutyB = FadeB * FadeLevel;
        }
      }
      else    TempStep++;
    } break;

    //to maximum birght and all color 
    case 2:{
      for (i = 0; i < LED_TOTAL; i++)
      {
        j = i % mode_para_data[FIREWORKS].ColorNum;
        LedData[i].DutyR = mode_para_data[FIREWORKS].Color[j].BufR;
        LedData[i].DutyG = mode_para_data[FIREWORKS].Color[j].BufG;
        LedData[i].DutyB = mode_para_data[FIREWORKS].Color[j].BufB;
        LedPick[i] = 1;
        ModeTime[i] = 0;
      }
      
      TempStep++;
      HoldTime = 0;
    }break;

    //hold for 2 seconds
    case 3:{
      HoldTime++;
      if (HoldTime >= 40){
        HoldTime = 0;
        TempStep++;
        LedPickAll = 0;
      }
    } break;

    //fade out and flash
    case 4:{
      //fade out
      for (i = 0; i < LED_TOTAL; i++){
        if (LedPick[i] == 0)    continue;     //if the led has been fade off, exit this circle
        
        RAND();
        ModeTime[i]++;
        if (ModeTime[i] >= ((uint8_t)rand() % 6)){
          ModeTime[i] = 0;

          j = i % mode_para_data[FIREWORKS].ColorNum;
          TempR = mode_para_data[FIREWORKS].Color[j].BufR;
          TempG = mode_para_data[FIREWORKS].Color[j].BufG;
          TempB = mode_para_data[FIREWORKS].Color[j].BufB;
          FadeR = TempR / FADE_LEVEL;
          FadeG = TempG / FADE_LEVEL;
          FadeB = TempB / FADE_LEVEL;
          if (LedData[i].DutyR > 0 || LedData[i].DutyG > 0 || LedData[i].DutyB > 0){
            LedData[i].DutyR -= FadeR;
            LedData[i].DutyG -= FadeG;
            LedData[i].DutyB -= FadeB;
          }
          else{
            LedPick[i] = 0;
            LedPickAll++;
            if (LedPickAll >= LED_TOTAL)   TempStep++;      //all leds are off
          }
        }
      }
    }break;

    //change color and repeat
    default:{
      TempColor++;
      if (TempColor >= mode_para_data[FIREWORKS].ColorNum)    TempColor = 0;
      TempStep = 0;
      LayerStep = 0;
    }break;
  }
}



//rainbow mode by triange layer
void Display_Tree_Vertigo(void)
{
	if (triangle_layer.en_flag == 0)
	{
		display_data.mode++;
		display_data.mode_buf++;
	}
	else
	{
		uint16_t temp = 0;  
		uint16_t	i = 0;
		uint16_t	j = 0;

		if(display_data.init == true)
		{	
			display_data.init = false;

			Para_Err_Check(&mode_para_data[VERTIGO]);

			BrightLevel = PARA_BRIGHT_MAX - mode_para_data[VERTIGO].Bright + 1;

			SpeedCtrl = 0;
			OtherCtrl = 0;
			TempColor = 0;
			TempStep  = 0;

			TempR = 0;
			TempG = 0;
			TempB = 0;
			TopR = 240;
			TopG = 240;
			TopB = 240;

			switch (mode_para_data[VERTIGO].Other)
			{
				default:    FadeR = 12;	TopR = 240;      break;
				case 9:     FadeR = 10;	TopR = 250;      break;
				case 8:     FadeR = 8;	TopR = 240;      break;
				case 7:     FadeR = 7; 	TopR = 245;     break;
				case 6:     FadeR = 6; 	TopR = 240;     break;
				case 5:     FadeR = 5; 	TopR = 225;     break;
				case 4:     FadeR = 5; 	TopR = 250;     break;
				case 3:     FadeR = 4; 	TopR = 220;     break;
				case 2:     FadeR = 4; 	TopR = 240;     break;
				case 1:     FadeR = 3; 	TopR = 210;     break;
				case 0:     FadeR = 3; 	TopR = 240;     break;
			}

			FadeG = FadeR;
			FadeB = FadeR;
			TopG = TopR;
			TopB = TopR;

			Display_All_Set(0,0,0);

			ModeFirstFlag = true;
			return;
		}



		SpeedCtrl++;
		//if (SpeedCtrl > ((PARA_SPEED_MAX - mode_para_data[RAINBOW].Speed) * (uint8_t)(1 - ModeFirstFlag)))
		{
			SpeedCtrl = 0;

			if (ModeFirstFlag)
			{
				ModeFirstFlag = false;
				j = LED_TOTAL;
			}
			else
			{
				j = mode_para_data[VERTIGO].Speed + 1;
			}

			for (i = 0; i < j; i++)
			{
				switch (TempStep)
				{
					case 0:
					{
						if (TempR < TopR)		TempR += FadeR;
						if(TempR == TopR)		TempStep++;
					} break;

					case 1:
					{
						if (TempG < TopG)		TempG += FadeG;
						if (TempG == TopG)		TempStep++;
					} break;

					case 2:
					{
						if (TempR > 0)			TempR -= FadeR;
						if (TempR == 0)			TempStep++;
					} break;

					case 3:
					{
						if (TempB < TopB)		TempB += FadeB;
						if (TempB == TopB)		TempStep++;
					} break;

					case 4:
					{
						if (TempG > 0)			TempG -= FadeG;
						if (TempG == 0)			TempStep++;
					} break;

					case 5:
					{
						if (TempR < TopR)		TempR += FadeR;
						if (TempR == TopR)		TempStep++;
					} break;

					case 6:
					{
						if (TempB > 0)			TempB -= FadeB;
						if (TempB == 0)			TempStep = 1;
					} break;
				}

				Shift_Triangle_RightLeft(&TempR, &TempG, &TempB);
			}
		}
	}
}
void Display_Tree_Horizontal(void)
{
  uint16_t temp = 0;  
  uint16_t temp1 = 0;
  uint16_t temp2 = 0;
  uint16_t i = 0;
  uint16_t j = 0;
  
	if(display_data.init == true){	
    display_data.init = false;

    Para_Err_Check(&mode_para_data[HORIZONTAL]);
    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[HORIZONTAL].Bright + 1;
    
	SpeedCtrl = 0;
    OtherCtrl = 0;
    TempColor = 0;
    TempStep  = 0;
    RptCtrl   = 0;
    
	TempR = mode_para_data[HORIZONTAL].Color[TempColor].BufR;
    TempG = mode_para_data[HORIZONTAL].Color[TempColor].BufG;
    TempB = mode_para_data[HORIZONTAL].Color[TempColor].BufB;

    RptTotal = PARA_OTHER_MAX - mode_para_data[HORIZONTAL].Other + 3; 
    
    Display_All_Set(0,0,0);

    ModeFirstFlag = true;
    
    return;
	}
  
  
  SpeedCtrl++;
  if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[HORIZONTAL].Speed) * (uint8_t)(1 - ModeFirstFlag))
  {
    SpeedCtrl = 0;

    if (ModeFirstFlag)
    {
		ModeFirstFlag = false;
		j = vertical_layer.layer_total;
    }
    else
    {
		j = 1;
    }

    for (i = 0; i < j; i++)
    {
	    switch (TempStep){
	      case 0:{
	        RptCtrl++;
	        if (RptCtrl >= RptTotal - 1){
	          RptCtrl = 0;
	          TempStep++;
	        }
	      }break;
	      
	      //Check the count of color, if the mode has only one color, fill it with dark
	      case 1:{
	        if (mode_para_data[HORIZONTAL].ColorNum == 1)   TempStep = 2;
	        else                                      TempStep = 3;
	      } break;
	      
	      //dark
	      case 2:{
	        TempR = mode_para_data[HORIZONTAL].Color[0].BufR / 4;
          	TempG = mode_para_data[HORIZONTAL].Color[0].BufG / 4;
          	TempB = mode_para_data[HORIZONTAL].Color[0].BufB / 4;
	        RptCtrl++;
	        if (RptCtrl > RptTotal){
	          RptCtrl = 0;
	          TempStep = 0;
	          TempR = mode_para_data[HORIZONTAL].Color[0].BufR;
	          TempG = mode_para_data[HORIZONTAL].Color[0].BufG;
	          TempB = mode_para_data[HORIZONTAL].Color[0].BufB;
	        }
	      } break;
	      
	      //change color
	      case 3:{
	        TempColor++;
	        if (TempColor >= mode_para_data[HORIZONTAL].ColorNum)   TempColor = 0;
	        TempR = mode_para_data[HORIZONTAL].Color[TempColor].BufR;
	        TempG = mode_para_data[HORIZONTAL].Color[TempColor].BufG;
	        TempB = mode_para_data[HORIZONTAL].Color[TempColor].BufB;
	        TempStep = 0;
	      } break;
	    }
	    
	    
	    Shift_Vertical_UpDown(&TempR, &TempG, &TempB);
	}
  }
}

void Display_Tree_Waves(void)
{
	uint16_t	i = 0;
	uint16_t	j = 0;
	uint16_t	k = 0;
	uint16_t	l = 0;
	uint16_t	m = 0;

	if (display_data.init == TRUE)
	{
		display_data.init = FALSE;

		Para_Err_Check(&mode_para_data[WAVES]);

		//BrightLevel = PARA_BRIGHT_MAX - mode_para_data[WAVES].Bright + 1;
		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[WAVES].Bright + 1;

		TempColor = 0;
		TempStep = 0;
		SpeedCtrl = 0;
		HoldTime = 0;
		TempR = mode_para_data[WAVES].Color[TempColor].BufR;
		TempG = mode_para_data[WAVES].Color[TempColor].BufG;
		TempB = mode_para_data[WAVES].Color[TempColor].BufB;
		Display_All_Set(TempR, TempG, TempB);

		if (mode_para_data[WAVES].ColorNum == 1)
		{
			TopR = 0;
			TopG = 0;
			TopB = 0;
		}
		else 
		{
			TempColor = 1;
			TopR = mode_para_data[WAVES].Color[TempColor].BufR;
			TopG = mode_para_data[WAVES].Color[TempColor].BufG;
			TopB = mode_para_data[WAVES].Color[TempColor].BufB;
		}

		//Get the fade level
		if (TopR >= TempR)    FadeR = (TopR - TempR) / FADE_LEVEL;
		else                  FadeR = (TempR - TopR) / FADE_LEVEL;

		if (TopG >= TempG)    FadeG = (TopG - TempG) / FADE_LEVEL;
		else                  FadeG = (TempG - TopG) / FADE_LEVEL;

		if (TopB >= TempB)    FadeB = (TopB - TempB) / FADE_LEVEL;
		else                  FadeB = (TempB - TopB) / FADE_LEVEL;

		ModeFirstFlag = true;

		return;
	}



	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[WAVES].Speed) * (uint8_t)(1 - ModeFirstFlag))
	{
		SpeedCtrl = 0;

		if (ModeFirstFlag)
		{
			ModeFirstFlag = false;
			m = vertical_layer.layer_total;
		}
		else
		{
			m = 1;
		}

		for (l = 0; l < m; l++)
		{
			switch (TempStep)
			{
				//fade
				case 0:
				{
					if (TempR > TopR)       TempR -= FadeR;
					else if (TopR > TempR)  TempR += FadeR;

					if (TempG > TopG)       TempG -= FadeG;
					else if (TopG > TempG)  TempG += FadeG;

					if (TempB > TopB)       TempB -= FadeB;
					else if (TopB > TempB)  TempB += FadeB;

					if (TempR == TopR && TempG == TopG && TempB == TopB)
					{
						TempStep++;
					}
				} break;


				default:
				{
					if (mode_para_data[WAVES].ColorNum == 1)
					{
						TopR = mode_para_data[WAVES].Color[0].BufR - TempR;
						TopG = mode_para_data[WAVES].Color[0].BufG - TempG;
						TopB = mode_para_data[WAVES].Color[0].BufB - TempB;
					}
					else
					{
						TempColor++;
						if (TempColor >= mode_para_data[WAVES].ColorNum)   TempColor = 0;
						TopR = mode_para_data[WAVES].Color[TempColor].BufR;
						TopG = mode_para_data[WAVES].Color[TempColor].BufG;
						TopB = mode_para_data[WAVES].Color[TempColor].BufB;
					}

					//Get the fade value again
					if (TopR >= TempR)    FadeR = (TopR - TempR) / FADE_LEVEL;
					else                  FadeR = (TempR - TopR) / FADE_LEVEL;

					if (TopG >= TempG)    FadeG = (TopG - TempG) / FADE_LEVEL;
					else                  FadeG = (TempG - TopG) / FADE_LEVEL;

					if (TopB >= TempB)    FadeB = (TopB - TempB) / FADE_LEVEL;
					else                  FadeB = (TempB - TopB) / FADE_LEVEL;


					TempStep = 0;
				} break;
			}

			Shift_Vertical_UpDown(&TempR, &TempG, &TempB);
		}
	}
}

//if the layermax > FADE_LEVEL, this mode need to fixed
void Display_Tree_Updwn(void)
{
	uint16 i = 0, j = 0, k = 0;

	if(display_data.init == true){	
		display_data.init = false;

		Para_Err_Check(&mode_para_data[UPDWN]);

		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[UPDWN].Bright + 1;

		SpeedCtrl 	= 0;
		OtherCtrl 	= 0;
		TempColor 	= 0;
		TempStep  	= 0;
		LayerStep 	= 0;
		RptCtrl 	= 0;
		TempColor 	= 0;
		TempColor1 = 1;
		if (TempColor1 >= mode_para_data[UPDWN].ColorNum)	TempColor1 = 0;
		
		FadeR = mode_para_data[UPDWN].Color[TempColor].BufR / FADE_LEVEL;
		FadeG = mode_para_data[UPDWN].Color[TempColor].BufG / FADE_LEVEL;
		FadeB = mode_para_data[UPDWN].Color[TempColor].BufB / FADE_LEVEL;
		FadeR1 = mode_para_data[UPDWN].Color[TempColor1].BufR / FADE_LEVEL;
		FadeG1 = mode_para_data[UPDWN].Color[TempColor1].BufG / FADE_LEVEL;
		FadeB1 = mode_para_data[UPDWN].Color[TempColor1].BufB / FADE_LEVEL;

		TempR = 0;
		TempG = 0;
		TempB = 0;

		TempR1 = 0;
		TempG1 = 0;
		TempB1 = 0;

		Display_All_Set(0,0,0);

		return;
	}

	SpeedCtrl++;
	if (SpeedCtrl >= 2)
	{
		SpeedCtrl = 0;

		switch (TempStep)
		{
			case 0:
			{
				LayerStep++;
				if (LayerStep > vertical_layer.layer_total + FADE_LEVEL)	TempStep++;

				for (i = 0; i < vertical_layer.layer_total; i++)
				{
					if (LayerStep < FADE_LEVEL)
					{
						//from bottom to top
						if (i > LayerStep)
						{
							TempR = 0;
							TempG = 0;
							TempB = 0;
						}
						else
						{
							j = FADE_LEVEL - LayerStep + i;
							TempR = FadeR * j;
							TempG = FadeG * j;
							TempB = FadeB * j;
						}

						//from top to bottom
						if ((vertical_layer.layer_total - 1 - i) > LayerStep)
						{
							TempR1 = 0;
							TempG1 = 0;
							TempB1 = 0;
						}
						else
						{
							j = FADE_LEVEL - LayerStep + vertical_layer.layer_total - 1 - i;
							TempR1 = FadeR1 * j;
							TempG1 = FadeG1 * j;
							TempB1 = FadeB1 * j;
						}
					}
					else{

						if (LayerStep < vertical_layer.layer_total)
						{
							//from bottom to top
							if ((i < (LayerStep - FADE_LEVEL)) || (i > LayerStep)){
								TempR = 0;
								TempG = 0;
								TempB = 0;
							}
							else{
								j = i - (LayerStep - FADE_LEVEL);
								TempR = FadeR * j;
								TempG = FadeG * j;
								TempB = FadeB * j;
							}

							//from top to bottom
							if (((vertical_layer.layer_total - 1 - i) < (LayerStep - FADE_LEVEL)) || (vertical_layer.layer_total - 1 - i > LayerStep)){
								TempR1 = 0;
								TempG1 = 0;
								TempB1 = 0;
							}
							else{
								j = vertical_layer.layer_total - 1 - i - (LayerStep - FADE_LEVEL);
								TempR1 = FadeR1 * j;
								TempG1 = FadeG1 * j;
								TempB1 = FadeB1 * j;
							}
						}

						else
						{
							//from bottom to top
							if (i < (LayerStep - FADE_LEVEL)){
								TempR = 0;
								TempG = 0;
								TempB = 0;
							}
							else{
								j = i - (LayerStep - FADE_LEVEL);
								TempR = FadeR * j;
								TempG = FadeG * j;
								TempB = FadeB * j;
							}

							//from top to bottom
							if ((vertical_layer.layer_total - 1 - i) < (LayerStep - FADE_LEVEL)){
								TempR1 = 0;
								TempG1 = 0;
								TempB1 = 0;
							}
							else{
								j = vertical_layer.layer_total - 1 - i - (LayerStep - FADE_LEVEL);
								TempR1 = FadeR1 * j;
								TempG1 = FadeG1 * j;
								TempB1 = FadeB1 * j;
							}
						}
					}

					//get the layer color value
					k = (uint16_t)TempR + (uint16_t)TempR1;
					if (k > 255)	k = 255;
					for (j = 0; j < LED_TOTAL; j++)
					{
						if (vertical_layer.info[j] == i)
						{
							LedData[j].DutyR = (uint8_t)k;
						}
					}
					
					k = (uint16_t)TempG + (uint16_t)TempG1;
					if (k > 255)	k = 255;
					for (j = 0; j < LED_TOTAL; j++)
					{
						if (vertical_layer.info[j] == i)
						{
							LedData[j].DutyG = (uint8_t)k;
						}
					}
					
					k = (uint16_t)TempB + (uint16_t)TempB1;
					if (k > 255)	k = 255;
					for (j = 0; j < LED_TOTAL; j++)
					{
						if (vertical_layer.info[j] == i)
						{
							LedData[j].DutyB = (uint8_t)k;
						}
					}
				}
			}break;

			//change the color
			default:{
				TempColor++;
				if (TempColor >= mode_para_data[UPDWN].ColorNum)	TempColor = 0;
				TempColor1++;
				if (TempColor1 >= mode_para_data[UPDWN].ColorNum) TempColor1 = 0;
				FadeR = mode_para_data[UPDWN].Color[TempColor].BufR / FADE_LEVEL;
				FadeG = mode_para_data[UPDWN].Color[TempColor].BufG / FADE_LEVEL;
				FadeB = mode_para_data[UPDWN].Color[TempColor].BufB / FADE_LEVEL;
				FadeR1 = mode_para_data[UPDWN].Color[TempColor1].BufR / FADE_LEVEL;
				FadeG1 = mode_para_data[UPDWN].Color[TempColor1].BufG / FADE_LEVEL;
				FadeB1 = mode_para_data[UPDWN].Color[TempColor1].BufB / FADE_LEVEL;
				TempStep = 0;
				LayerStep = 0;
				TempR = 0;
				TempG = 0;
				TempB = 0;
				TempR1 = 0;
				TempG1 = 0;
				TempB1 = 0;
			}break;
		}
	}
}

//rolling mode by triangle layer
void Display_Tree_Diagonal(void)
{
	if (triangle_layer.en_flag == 0)
	{
		display_data.mode++;
		display_data.mode_buf++;
	}
	else
	{
		uint16_t temp = 0;  
		uint16_t temp1 = 0;
		uint16_t temp2 = 0;
		uint16_t i = 0;
		uint16_t j = 0;

		if(display_data.init == true)
		{	
			display_data.init = false;

			Para_Err_Check(&mode_para_data[DIAGONAL]);
			BrightLevel = PARA_BRIGHT_MAX - mode_para_data[DIAGONAL].Bright + 1;

			SpeedCtrl = 0;
			OtherCtrl = 0;
			TempColor = 0;
			TempStep  = 0;
			RptCtrl   = 0;

			TempR = mode_para_data[DIAGONAL].Color[TempColor].BufR;
			TempG = mode_para_data[DIAGONAL].Color[TempColor].BufG;
			TempB = mode_para_data[DIAGONAL].Color[TempColor].BufB;

			RptTotal = PARA_OTHER_MAX - mode_para_data[DIAGONAL].Other + 2; 
			//RptTotal = 5;

			Display_All_Set(0,0,0);

			ModeFirstFlag = true;

			return;
		}


		SpeedCtrl++;
		if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[DIAGONAL].Speed) * (uint8_t)(1 - ModeFirstFlag))
		{
			SpeedCtrl = 0;

			if (ModeFirstFlag)
			{
				ModeFirstFlag = false;
				j = vertical_layer.layer_total;
			}
			else
			{
				j = 1;
			}

			for (i = 0; i < j; i++)
			{
				switch (TempStep)
				{
					case 0:
					{
						RptCtrl++;
						if (RptCtrl >= RptTotal - 1)
						{
							RptCtrl = 0;
							//TempStep++;
							if (mode_para_data[DIAGONAL].ColorNum == 1)   	TempStep = 2;
							else                                      		TempStep = 3;
						}
					}break;

					//Check the count of color, if the mode has only one color, fill it with dark
					case 1:
					{
						if (mode_para_data[DIAGONAL].ColorNum == 1)   	TempStep = 2;
						else                                      		TempStep = 3;
					} break;

					//dark
					case 2:
					{
						TempR = mode_para_data[DIAGONAL].Color[0].BufR / 4;
						TempG = mode_para_data[DIAGONAL].Color[0].BufG / 4;
						TempB = mode_para_data[DIAGONAL].Color[0].BufB / 4;
						RptCtrl++;
						if (RptCtrl > RptTotal)
						{
							RptCtrl = 0;
							TempStep = 0;
							TempR = mode_para_data[DIAGONAL].Color[0].BufR;
							TempG = mode_para_data[DIAGONAL].Color[0].BufG;
							TempB = mode_para_data[DIAGONAL].Color[0].BufB;
						}
					} break;

					//change color
					case 3:
					{
						TempColor++;
						if (TempColor >= mode_para_data[DIAGONAL].ColorNum)   TempColor = 0;
						TempR = mode_para_data[DIAGONAL].Color[TempColor].BufR;
						TempG = mode_para_data[DIAGONAL].Color[TempColor].BufG;
						TempB = mode_para_data[DIAGONAL].Color[TempColor].BufB;
						TempStep = 0;
					} break;
				}

				Shift_Triangle_RightLeft(&TempR, &TempG, &TempB);
			}
		}
	}
}

//rainbow mode by fan layer
void Display_Tree_Sunset(void)
{
  	if (fan_layer.en_flag == 0)
	{
		display_data.mode++;
		display_data.mode_buf++;
	}
	else
	{
		uint16_t i = 0;
		uint16_t j = 0;

		if(display_data.init == true)
		{	
			display_data.init = false;

			Para_Err_Check(&mode_para_data[SUNSET]);
			BrightLevel = PARA_BRIGHT_MAX - mode_para_data[SUNSET].Bright + 1;

			SpeedCtrl = 0;
			OtherCtrl = 0;
			TempColor = 0;
			TempStep  = 0;
			RptCtrl   = 0;

			TempR = mode_para_data[SUNSET].Color[TempColor].BufR;
			TempG = mode_para_data[SUNSET].Color[TempColor].BufG;
			TempB = mode_para_data[SUNSET].Color[TempColor].BufB;

			//RptTotal = PARA_OTHER_MAX - mode_para_data[SUNSET].Other + 3; 
			RptTotal = 2;

			Display_All_Set(0,0,0);

			ModeFirstFlag = true;

			return;
		}


		SpeedCtrl++;
		if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[SUNSET].Speed) * (uint8_t)(1 - ModeFirstFlag))
		{
			SpeedCtrl = 0;

			if (ModeFirstFlag)
			{
				ModeFirstFlag = false;
				j = vertical_layer.layer_total;
			}
			else
			{
				j = 1;
			}

			for (i = 0; i < j; i++)
			{
				switch (TempStep)
				{
					case 0:
					{
						RptCtrl++;
						if (RptCtrl >= RptTotal - 1)
						{
							RptCtrl = 0;
							//TempStep++;
							if (mode_para_data[SUNSET].ColorNum == 1)   TempStep = 2;
							else                                      TempStep = 3;
						}
					}break;

					//Check the count of color, if the mode has only one color, fill it with dark
					case 1:
					{
						if (mode_para_data[SUNSET].ColorNum == 1)   TempStep = 2;
						else                                      TempStep = 3;
					} break;

					//dark
					case 2:
					{
						TempR = mode_para_data[SUNSET].Color[0].BufR / 4;
						TempG = mode_para_data[SUNSET].Color[0].BufG / 4;
						TempB = mode_para_data[SUNSET].Color[0].BufB / 4;
						RptCtrl++;
						if (RptCtrl > RptTotal)
						{
							RptCtrl = 0;
							TempStep = 0;
							TempR = mode_para_data[SUNSET].Color[0].BufR;
							TempG = mode_para_data[SUNSET].Color[0].BufG;
							TempB = mode_para_data[SUNSET].Color[0].BufB;
						}
					} break;

					//change color
					case 3:
					{
						TempColor++;
						if (TempColor >= mode_para_data[SUNSET].ColorNum)   TempColor = 0;
						TempR = mode_para_data[SUNSET].Color[TempColor].BufR;
						TempG = mode_para_data[SUNSET].Color[TempColor].BufG;
						TempB = mode_para_data[SUNSET].Color[TempColor].BufB;
						TempStep = 0;
					} break;
				}

				Shift_Fan_LeftRight(&TempR, &TempG, &TempB);
			}
		}
	}
}

//rainbow mode by fan layer
void Display_Tree_Vintage(void)
{
	if (fan_layer.en_flag == 0)
	{
		display_data.mode++;
		display_data.mode_buf++;
	}
	else
	{
		uint16_t temp = 0;  
		uint16_t	i = 0;
		uint16_t	j = 0;

		if(display_data.init == true)
		{	
			display_data.init = false;

			Para_Err_Check(&mode_para_data[VINTAGE]);

			BrightLevel = PARA_BRIGHT_MAX - mode_para_data[VINTAGE].Bright + 1;

			SpeedCtrl = 0;
			OtherCtrl = 0;
			TempColor = 0;
			TempStep  = 0;

			TempR = 0;
			TempG = 0;
			TempB = 0;
			TopR = 240;
			TopG = 240;
			TopB = 240;

			switch (mode_para_data[VINTAGE].Other)
			{
				default:    FadeR = 12;	TopR = 240;      break;
				case 9:     FadeR = 10;	TopR = 250;      break;
				case 8:     FadeR = 8;	TopR = 240;      break;
				case 7:     FadeR = 7; 	TopR = 245;     break;
				case 6:     FadeR = 6; 	TopR = 240;     break;
				case 5:     FadeR = 5; 	TopR = 225;     break;
				case 4:     FadeR = 5; 	TopR = 250;     break;
				case 3:     FadeR = 4; 	TopR = 220;     break;
				case 2:     FadeR = 4; 	TopR = 240;     break;
				case 1:     FadeR = 3; 	TopR = 210;     break;
				case 0:     FadeR = 3; 	TopR = 240;     break;
			}

			FadeG = FadeR;
			FadeB = FadeR;
			TopG = TopR;
			TopB = TopR;

			Display_All_Set(0,0,0);

			ModeFirstFlag = true;
			return;
		}



		SpeedCtrl++;
		//if (SpeedCtrl > ((PARA_SPEED_MAX - mode_para_data[RAINBOW].Speed) * (uint8_t)(1 - ModeFirstFlag)))
		{
			SpeedCtrl = 0;

			if (ModeFirstFlag)
			{
				ModeFirstFlag = false;
				j = LED_TOTAL;
			}
			else
			{
				j = mode_para_data[VINTAGE].Speed + 1;
			}

			for (i = 0; i < j; i++)
			{
				switch (TempStep)
				{
					case 0:
					{
						if (TempR < TopR)		TempR += FadeR;
						if(TempR == TopR)		TempStep++;
					} break;

					case 1:
					{
						if (TempG < TopG)		TempG += FadeG;
						if (TempG == TopG)		TempStep++;
					} break;

					case 2:
					{
						if (TempR > 0)			TempR -= FadeR;
						if (TempR == 0)			TempStep++;
					} break;

					case 3:
					{
						if (TempB < TopB)		TempB += FadeB;
						if (TempB == TopB)		TempStep++;
					} break;

					case 4:
					{
						if (TempG > 0)			TempG -= FadeG;
						if (TempG == 0)			TempStep++;
					} break;

					case 5:
					{
						if (TempR < TopR)		TempR += FadeR;
						if (TempR == TopR)		TempStep++;
					} break;

					case 6:
					{
						if (TempB > 0)			TempB -= FadeB;
						if (TempB == 0)			TempStep = 1;
					} break;
				}

				Shift_Fan_LeftRight(&TempR, &TempG, &TempB);
			}
		}
	}
}
void Display_Tree_Glow(void)
{
	uint16_t	i = 0;
  	uint16_t	j = 0;
  	uint16_t	l = 0;
  	uint16_t	m = 0;
  
	if(display_data.init)
	{	
	    display_data.init = false;

	    Para_Err_Check(&mode_para_data[GLOW]);
	    
	    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[GLOW].Bright + 1;
	    
		SpeedCtrl = 0;
	    OtherCtrl = 0;
	    TempColor = 0;
	    TempStep  = 0;
	    
		TempR = 0;
	    TempG = 0;
	    TempB = 0;

	    for ( i = 0; i < LED_TOTAL; i++)
	    {
			ModeStep[i] = 0;
			RAND();
			ModeTime[i] = (uint8_t)(rand() % 50);
	    }

	    for (i = 0; i < LED_TOTAL; i++)
	    {
			j = i % mode_para_data[GLOW].ColorNum;
			LedData[i].DutyR = mode_para_data[GLOW].Color[j].BufR;
			LedData[i].DutyG = mode_para_data[GLOW].Color[j].BufG;
			LedData[i].DutyB = mode_para_data[GLOW].Color[j].BufB;
	    }

	    ModeFirstFlag = true;
	    return;
	}
  
  	SpeedCtrl++;
  	if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[GLOW].Speed))
  	{
		if (ModeFirstFlag)
		{
			ModeFirstFlag = false;
			m = 1;
		}
		else
		{
			m = 1;
		}
			

		for (l = 0; l < m; l++)
		{
			for (i = 0; i < LED_TOTAL; i++)
			{
				if (ModeTime[i])
				{
					ModeTime[i]--;
					continue;
				}

				j = i % mode_para_data[GLOW].ColorNum;
				TopR = mode_para_data[GLOW].Color[j].BufR;
				TopG = mode_para_data[GLOW].Color[j].BufG;
				TopB = mode_para_data[GLOW].Color[j].BufB;
				FadeR = TopR / FADE_LEVEL;
				FadeG = TopG / FADE_LEVEL;
				FadeB = TopB / FADE_LEVEL;
				
				switch (ModeStep[i])
				{
			      	case 0:
			      	{
				        if (LedData[i].DutyR < TopR || LedData[i].DutyG < TopG || LedData[i].DutyB < TopB)
				        {
				        	LedData[i].DutyR += FadeR;
							LedData[i].DutyG += FadeG;
							LedData[i].DutyB += FadeB;
				        }
				        else
				        {
				        	ModeStep[i]++;
				        }
				     } break;
				      
				      default:
				      {
				        if (LedData[i].DutyR > 0 || LedData[i].DutyG > 0 || LedData[i].DutyB > 0)
				        {
							LedData[i].DutyR -= FadeR;
							LedData[i].DutyG -= FadeG;
							LedData[i].DutyB -= FadeB;
				        }
				        else
				        {
				        	ModeStep[i] = 0;
				        }
				      } break;
			    }
			}
		}
	}
}
void Display_Tree_Color_Rand(void)
{
  uint16_t    i = 0, j = 0, k = 0;
  uint16_t	l = 0;
  uint16_t	m = 0;
  
  if (display_data.init == true){
    display_data.init  = false;

    Para_Err_Check(&mode_para_data[COLOR_RAND]);
    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[COLOR_RAND].Bright + 1;

    SpeedCtrl = 0;
    OtherCtrl = mode_para_data[COLOR_RAND].Other;

    TempR = 0;
    TempG = 0;
    TempB = 0;

    HoldTime = 0;
    Display_All_Set(0,0,0); 

	ModeFirstFlag = true;
    
    return;
	}
  
  HoldTime++;
  if (HoldTime > OtherCtrl * 20){
    HoldTime = 0;
    TempStep++;
    if (TempStep > 5)   TempStep = 0;
  }
    
  SpeedCtrl++;
  if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[COLOR_RAND].Speed) * (uint8_t)(1 - ModeFirstFlag))
  {
    SpeedCtrl = 0;

    if (ModeFirstFlag)
    {
		ModeFirstFlag = false;
		m = vertical_layer.layer_total;
    }
    else
    {
		m = 1;
    }

    for (l = 0; l < m; l++)
    {
	    switch (TempStep){
	      case 0:        TempB = 0; TempR += 12; TempG -= 7;	break;
	      case 1:        TempG = 0; TempB -= 15; TempR += 9;	break;
	      case 2:        TempR = 0; TempG += 7; TempB -= 16;	break;
	      case 3:        TempB = 0; TempR -= 13; TempG += 8;	break;
	      case 4:        TempG = 0; TempB += 10; TempR -= 7;	break;
	      default:       TempR = 0; TempG -= 8; TempB += 11;	break;
	    }
	    
	    Shift_Vertical_UpDown(&TempR, &TempG, &TempB);
	}
  }
}

void Display_Tree_Carnival(void)
{
	uint16_t    i = 0;
	uint16_t	l = 0;
	uint16_t	m = 0;

	if (display_data.init)
	{
		display_data.init  = false;
		Para_Err_Check(&mode_para_data[CARNIVAL]);
		BrightLevel = PARA_BRIGHT_MAX - mode_para_data[CARNIVAL].Bright + 1;

		SpeedCtrl = 0;

		TempR = 0;
		TempG = 0;
		TempB = 0;

		HoldTime = 0;
		Display_All_Set(0,0,0); 

		TempStep = 1;

		ModeFirstFlag = true;
		return;
	}

	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - mode_para_data[CARNIVAL].Speed) * (uint8_t)( 1 - ModeFirstFlag))
	{
		SpeedCtrl = 0;

		if (ModeFirstFlag)
		{
			ModeFirstFlag = false;
			m = 50;
		}
		else
		{
			m = 1;
		}
			

		for (l = 0; l < m; l++)
			{
			for (i = 0; i < LED_TOTAL; i++)
			{
				switch (i % 6)
				{
					case 0:
					{
						LedData[i].DutyB = 0;
						RAND();
						LedData[i].DutyR += (uint8_t)rand() % 10;
						RAND();
						LedData[i].DutyG -= (uint8_t)rand() % 10;
					}break;

					case 1:
					{
						LedData[i].DutyG = 0;
						RAND();
						LedData[i].DutyB += (uint8_t)rand() % 11;
						RAND();
						LedData[i].DutyR -= (uint8_t)rand() % 9;
					}break;

					case 2:
					{
						LedData[i].DutyR = 0;
						RAND();
						LedData[i].DutyG += (uint8_t)rand() % 12;
						RAND();
						LedData[i].DutyB -= (uint8_t)rand() % 8;
					}break;

					case 3:
					{
						LedData[i].DutyB = 0;
						RAND();
						LedData[i].DutyG += (uint8_t)rand() % 13;
						RAND();
						LedData[i].DutyR -= (uint8_t)rand() % 7;
					}break;

					case 4:
					{
						LedData[i].DutyG = 0;
						RAND();
						LedData[i].DutyR += (uint8_t)rand() % 14;
						RAND();
						LedData[i].DutyB -= (uint8_t)rand() % 6;
					}break;

					default:
					{
						LedData[i].DutyR = 0;
						RAND();
						LedData[i].DutyB += (uint8_t)rand() % 15;
						RAND();
						LedData[i].DutyG -= (uint8_t)rand() % 5;
					}break;
				}
			}
		}
	}
}

void Display_Tree_Alternate(void)
{
	uint16_t	i = 0;
	uint16_t	l = 0;
	uint16_t	m = 0;
  
	if(display_data.init)
	{	
	    display_data.init = false;

	    Para_Err_Check(&mode_para_data[ALTERNATE]);
	    
	    BrightLevel = PARA_BRIGHT_MAX - mode_para_data[ALTERNATE].Bright + 1;
	    
		SpeedCtrl = 0;
	    OtherCtrl = 0;
	    TempColor = 0;
	    TempStep  = 0;
	    
		TempR = 0;
	    TempG = 0;
	    TempB = 0;
	    TopR = 240;
	    TopG = 240;
	    TopB = 240;

	    switch (mode_para_data[ALTERNATE].Speed)
	    {
			case 0:		FadeR = 10;	RptTotal = 40; break;
			case 1:		FadeR = 15; RptTotal = 30; break;
			case 2: 	FadeR = 20; RptTotal = 20; break;
			case 3:		FadeR = 24;	RptTotal = 18; break;
			case 4:		FadeR = 30;	RptTotal = 15; break;
			default:	FadeR = 40;	RptTotal = 10; break;
	    }
	    
	    FadeG = FadeR;
	    FadeB = FadeR;

	    for ( i = 0; i < LED_TOTAL; i++)
	    {
			ModeStep[i] = 0;
			RAND();
			ModeTime[i] = (uint8_t)(rand() % RptTotal);
	    }

	    Display_All_Set(0,0,0);

	    ModeFirstFlag = true;
	    return;
	}
  
  

	if (ModeFirstFlag)
	{
		ModeFirstFlag = false;
		m = RptTotal;
	}
	else
	{
		m = 1;
	}
		

	for (l = 0; l < m; l++)
	{
		for (i = 0; i < LED_TOTAL; i++)
		{
			if (ModeTime[i])
			{
				ModeTime[i]--;
				continue;
			}
			
			switch (ModeStep[i])
			{
		      	case 0:
		      	{
			        if (LedData[i].DutyR < TopR)    LedData[i].DutyR += FadeR;
			        else              ModeStep[i]++;
			     } break;
			      
			      case 1:{
			        if (LedData[i].DutyG < TopG)    LedData[i].DutyG += FadeG;
			        else              ModeStep[i]++;
			      } break;
			      
			      case 2:{
			        if (LedData[i].DutyR > 0)    LedData[i].DutyR -= FadeR;
			        else              ModeStep[i]++;
			      } break;
			      
			      case 3:{
			        if (LedData[i].DutyB < TopB)    LedData[i].DutyB += FadeB;
			        else              ModeStep[i]++;
			      } break;
			      
			      case 4:{
			        if (LedData[i].DutyG > 0)    LedData[i].DutyG -= FadeG;
			        else              ModeStep[i]++;
			      } break;
			      
			      case 5:{
			        if (LedData[i].DutyR < TopR)    LedData[i].DutyR += FadeR;
			        else              ModeStep[i]++;
			      } break;
			      
			      case 6:{
			        if (LedData[i].DutyB > 0)    LedData[i].DutyB -= FadeR;
			        else              ModeStep[i] = 1;
			      } break;
		    }
		}
	}
}


void Display_Custom_Steady(uint8_t sec)
{
	if (display_data.init)
	{	
		uint8_t	*dst = 0;

		dst = mmalloc(1024);

		if (dst != 0)
		{
			display_data.init = false;
			read_custom_steady_flash(sec, (uint8_t *)dst);
			mcpy((uint8_t *)LedData, dst, LED_TOTAL * 3);
		}

		mfree(dst);
	}
}


void Display_Custom_Dynamic(void)
{
	static uint8_t	DynamicTimeStep;
	static uint8_t	DynamicTime;
	if (display_data.init)
	{
		display_data.init = false;
		DynamicTimeStep = 0;
		DynamicTime = 1;
	}

	DynamicTime++;
	if (DynamicTime >= 2)
	{
		DynamicTime = 0;
		if (DynamicTimeFlag[DynamicTimeStep] == true)
		{
			uint8_t	*dst = 0;

			dst = mmalloc(1024);
			if (dst != 0)
			{
				read_custom_dynamic_flash(DynamicTimeStep, (uint8_t *)dst);
				mcpy((uint8_t *)LedData, dst, LED_TOTAL);
			}
			mfree(dst);
		}
		else
		{
			for (uint16_t i = 0; i < LED_TOTAL; i++)
			{
				LedData[i].DutyR = 0;
				LedData[i].DutyG = 0;
				LedData[i].DutyB = 0;
			}
		}
		DynamicTimeStep++;
		if (DynamicTimeStep >= DYNAMIC_MAX_TIME)
		{
			DynamicTimeStep = 0;
		}
	}
}


