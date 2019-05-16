/********************************************************************
  *
  * FileName    string_mode.c
  * Brief       the mode when the product haven't been lay out
  *
*********************************************************************/
#include "display.h"



/**
  * FunctionName    Display_str_steady
  */
void Display_Str_Steady(void)
{
	Display_Tree_Steady();
}


/**
  * FunctionName  Display_str_sparkle
  */
void Display_Str_Sparkle(void)
{
	Display_Tree_Sparkle();
}


/**
  * FunctionName  Display_str_rainbow
  */
void Display_Str_Rainbow(void)
{
	uint16 i = 0;  

	if(Display.Init == true){	
		Display.Init = false;

		Para_Err_Check(&ParaData[RAINBOW]);

		BrightLevel = 1;

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

		switch (ParaData[RAINBOW].Other)
		{
			default:    FadeR = 80;      break;
			case 3:     FadeR = 60;      break;
			case 2:     FadeR = 40;      break;
			case 1:     FadeR = 20;      break;
			case 0:     FadeR = 10;      break;
		}

		FadeG = FadeR;
		FadeB = FadeR;

		Display_All_Set(0,0,0);
		return;
	}



	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - ParaData[RAINBOW].Speed)){
		SpeedCtrl = 0;

		switch (TempStep)
		{
			case 0:
			{
				if (TempR < TopR)    TempR += FadeR;
				else              TempStep++;
			} break;

			case 1:
			{
				if (TempG < TopG)    TempG += FadeG;
				else              TempStep++;
			} break;

			case 2:
			{
				if (TempR > 0)    TempR -= FadeR;
				else              TempStep++;
			} break;

			case 3:
			{
				if (TempB < TopB)    TempB += FadeB;
				else              TempStep++;
			} break;

			case 4:
			{
				if (TempG > 0)    TempG -= FadeG;
				else              TempStep++;
			} break;

			case 5:
			{
				if (TempR < TopR)    TempR += FadeR;
				else              TempStep++;
			} break;

			case 6:
			{
				if (TempB > 0)    TempB -= FadeR;
				else              TempStep = 1;
			} break;
		}

		for (i = LED_TOTAL - 1; i > 0; i--)
		{
			LedData[i].DutyR = LedData[i - 1].DutyR;
			LedData[i].DutyG = LedData[i - 1].DutyG;
			LedData[i].DutyB = LedData[i - 1].DutyB;
		}
		LedData[0].DutyR = TempR;
		LedData[0].DutyG = TempG;
		LedData[0].DutyB = TempB;
	}
}

void Display_Str_Fade(void)
{
	Display_Tree_Fade();
}



/**
  * FunctionName  Display_Tree_Snow
  */
void Display_Str_Snow(void)
{
	Display.Mode = SNAKE;
	Display.ModeBuf = SNAKE;
	Display.Init = true;
}


/**
  * FunctionName  Display_Tree_Snake
  */

void Display_Str_Snake(void)
{
	Display_Tree_Snake();
}



void Display_Str_Twinkle(void)
{
	Display_Tree_Twinkle();
}


void Display_Str_Fireworks(void)
{
	uint16_t i = 0, j = 0;

	if (Display.Init == true)
	{
		Display.Init = false;
		Para_Err_Check(&ParaData[FIREWORKS]);
		BrightLevel = 1;

		SpeedCtrl = 0;
		TempStep = 0;
		TempColor = 0;
		LayerStep = 0;
		RptCtrl = 0;
		FadeLevel = 0;
		Display_All_Set(0, 0, 0);
		
		return;
	}


	switch (TempStep)
	{
		//turn on from bottom to top layer
		case 0:
		{
			SpeedCtrl++;
			if (SpeedCtrl > PARA_SPEED_MAX - ParaData[FIREWORKS].Speed)
			{
				SpeedCtrl = 0;

				FadeR = ParaData[FIREWORKS].Color[TempColor].BufR / FADE_LEVEL;
				FadeG = ParaData[FIREWORKS].Color[TempColor].BufG / FADE_LEVEL;
				FadeB = ParaData[FIREWORKS].Color[TempColor].BufB / FADE_LEVEL;

				LayerStep++;	//temporarily used as fading step
				if (LayerStep >= FADE_LEVEL)		TempStep++;
				
				for (i = 0; i <= LED_TOTAL; i++)
				{
					LedData[i].DutyR = FadeR * LayerStep;
					LedData[i].DutyG = FadeG * LayerStep;
					LedData[i].DutyB = FadeB * LayerStep;
				}
			}
		}break;

		//fade out to 1/2
		case 1:
		{
			if (FadeLevel > FADE_LEVEL / 2)
			{
				FadeLevel--;
				for (i = 0; i < LED_TOTAL; i++)
				{
					LedData[i].DutyR = FadeR * FadeLevel;
					LedData[i].DutyG = FadeG * FadeLevel;
					LedData[i].DutyB = FadeB * FadeLevel;
				}
			}
			else    TempStep++;
		} break;

		//to maximum birght and all color 
		case 2:
		{
		for (i = 0; i < LED_TOTAL; i++)
		{
		j = i % ParaData[FIREWORKS].ColorNum;
		LedData[i].DutyR = ParaData[FIREWORKS].Color[j].BufR;
		LedData[i].DutyG = ParaData[FIREWORKS].Color[j].BufG;
		LedData[i].DutyB = ParaData[FIREWORKS].Color[j].BufB;
		LedPick[i] = 1;
		ModeTime[i] = 0;
		}

		TempStep++;
		HoldTime = 0;
		}break;

		//hold for 2 seconds
		case 3:
		{
		HoldTime++;
		if (HoldTime >= 40)
		{
		HoldTime = 0;
		TempStep++;
		LedPickAll = 0;
		}
		} break;

		//fade out and flash
		case 4:
		{
		//fade out
		for (i = 0; i < LED_TOTAL; i++)
		{
		if (LedPick[i] == 0)    continue;     //if the led has been fade off, exit this circle

		RAND();
		ModeTime[i]++;
		if (ModeTime[i] >= ((uint8_t)rand() % 6))
		{
		ModeTime[i] = 0;

		j = i % ParaData[FIREWORKS].ColorNum;
		TempR = ParaData[FIREWORKS].Color[j].BufR;
		TempG = ParaData[FIREWORKS].Color[j].BufG;
		TempB = ParaData[FIREWORKS].Color[j].BufB;
		FadeR = TempR / FADE_LEVEL;
		FadeG = TempG / FADE_LEVEL;
		FadeB = TempB / FADE_LEVEL;
		if (LedData[i].DutyR > 0 || LedData[i].DutyG > 0 || LedData[i].DutyB > 0)
		{
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
		default:
		{
		TempColor++;
		if (TempColor >= ParaData[FIREWORKS].ColorNum)    TempColor = 0;
		TempStep = 0;
		LayerStep = 0;
		}break;
	}
}


void Display_Str_Horizontal(void)
{
  uint16_t temp = 0;  
  uint16_t temp1 = 0;
  uint16_t temp2 = 0;
  
	if(Display.Init == true){	
    Display.Init = false;
    Para_Err_Check(&ParaData[HORIZONTAL]);
    BrightLevel = 1;
    
	SpeedCtrl = 0;
    OtherCtrl = 0;
    TempColor = 0;
    TempStep  = 0;
    RptCtrl   = 0;
    
	TempR = ParaData[HORIZONTAL].Color[TempColor].BufR;
    TempG = ParaData[HORIZONTAL].Color[TempColor].BufG;
    TempB = ParaData[HORIZONTAL].Color[TempColor].BufB;

    RptTotal = PARA_OTHER_MAX - ParaData[HORIZONTAL].Other + 3; 
    
    Display_All_Set(0,0,0);
    
    return;
	}
  
  
  SpeedCtrl++;
  if (SpeedCtrl > (PARA_SPEED_MAX - ParaData[HORIZONTAL].Speed) + 2){
    SpeedCtrl = 0;
    
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
        if (ParaData[HORIZONTAL].ColorNum == 1)   TempStep = 2;
        else                                      TempStep = 3;
      } break;
      
      //dark
      case 2:{
        TempR = 0;
        TempG = 0;
        TempB = 0;
        RptCtrl++;
        if (RptCtrl > RptTotal)
        {
          RptCtrl = 0;
          TempStep = 0;
          TempR = ParaData[HORIZONTAL].Color[0].BufR;
          TempG = ParaData[HORIZONTAL].Color[0].BufG;
          TempB = ParaData[HORIZONTAL].Color[0].BufB;
        }
      } break;
      
      //change color
      case 3:{
        TempColor++;
        if (TempColor >= ParaData[HORIZONTAL].ColorNum)   TempColor = 0;
        TempR = ParaData[HORIZONTAL].Color[TempColor].BufR;
        TempG = ParaData[HORIZONTAL].Color[TempColor].BufG;
        TempB = ParaData[HORIZONTAL].Color[TempColor].BufB;
        TempStep = 0;
      } break;
    }
    

    for (temp = 0; temp < STR_LAYER_MAX - 1; temp++)
    {
		temp2 = temp + 1;
		for (temp1 = temp * STR_LAYER_SEC; temp1 < temp2 * STR_LAYER_SEC; temp1++)
		{
			LedData[temp1].DutyR = LedData[temp2].DutyR;
	        LedData[temp1].DutyG = LedData[temp2].DutyG;
	        LedData[temp1].DutyB = LedData[temp2].DutyB;
		}
    }
    
    temp = STR_LAYER_MAX - 1;
    for (temp1 = temp * STR_LAYER_SEC; temp1 < LED_TOTAL; temp1++)
	{
		LedData[temp1].DutyR = TempR;
        LedData[temp1].DutyG = TempG;
        LedData[temp1].DutyB = TempB;
	}
  }
}

void Display_Str_Waves(void)
{
	uint16 i = 0;

	if(Display.Init == true){	
		Display.Init = false;

		Para_Err_Check(&ParaData[WAVES]);

		BrightLevel = 1;

		SpeedCtrl 	= 0;
		OtherCtrl 	= 0;
		TempColor 	= 0;
		TempStep  	= 0;
		RptCtrl 	= 0;
		TempColor 	= 0;

		TopR = ParaData[WAVES].Color[TempColor].BufR;
		TopG = ParaData[WAVES].Color[TempColor].BufG;
		TopB = ParaData[WAVES].Color[TempColor].BufB;
		TempR = 0;
		TempG = 0;
		TempB = 0;

		if (TopR >= TopG && TopR >= TopB){
			FadeR = 0;
			FadeG = TopG / (ParaData[WAVES].Other + 1);
			FadeB = TopB / (ParaData[WAVES].Other + 1);
			TempR = TopR;
		}
		else if (TopG >= TopB && TopG >= TopR){
			FadeG = 0;
			FadeR = TopR / (ParaData[WAVES].Other + 1);
			FadeB = TopB / (ParaData[WAVES].Other + 1);
			TempG = TopG;
		}
		else if (TopB >= TopR && TopB >= TopG){
			FadeB = 0;
			FadeR = TopR / (ParaData[WAVES].Other + 1);
			FadeG = TopG / (ParaData[WAVES].Other + 1);
			TempB = TopB;
		}
		

		Display_All_Set(0,0,0);

		return;
	}
  

	SpeedCtrl++;
	if (SpeedCtrl > (PARA_SPEED_MAX - ParaData[WAVES].Speed)){
		SpeedCtrl = 0;

		switch (TempStep){
			case 0:{
				if (FadeR != 0){
					if (TempR < TopR - FadeR){
						TempR += FadeR;
					}
					else{
						TempR = TopR;
						TempStep++;
					}
				}
				else	TempStep++;
			}break;

			case 1:{
				if (FadeG != 0){
					if (TempG < TopG - FadeG){
						TempG += FadeG;
					}
					else{
						TempG = TopG;
						TempStep++;
					}
				}
				else	TempStep++;
			}break;

			case 2:{
				if (FadeB != 0){
					if (TempB < TopB - FadeB){
						TempB += FadeB;
					}
					else{
						TempB = TopB;
						TempStep++;
					}
				}
				else	TempStep++;
			}break;

			case 3:{
				if (FadeR == 0)		TempStep++;
				else{
					if (TempR > FadeR)		TempR -= FadeR;
					else{
						TempR = 0;
						TempStep++;
					}
				}
			} break;

			case 4:{
				if (FadeG == 0)		TempStep++;
				else{
					if (TempG > FadeG)		TempG -= FadeG;
					else{
						TempG = 0;
						TempStep++;
					}
				}
			}break;

			case 5:{
				if (FadeB == 0)	TempStep++;
				else{
					if (TempB > FadeB)		TempB -= FadeB;
					else{
						TempB = 0;
						TempStep++;
					}
				}
			}break;

			case 6:{
				FadeR = TempR / FADE_LEVEL;
				FadeG = TempG / FADE_LEVEL;
				FadeB = TempB / FADE_LEVEL;
				TempStep++;
			}break;

			case 7:{
				for (uint8_t m = 0; m <= (PARA_OTHER_MAX - ParaData[WAVES].Other + 2); m++){
					if (TempR > 0 || TempG > 0 || TempB > 0){
						TempR -= FadeR;
						TempG -= FadeG;
						TempB -= FadeB;
					}
					else	TempStep++;
				}
			}break;

			default:{
				TempColor++;
				if (TempColor >= ParaData[WAVES].ColorNum)	TempColor = 0;
				TopR = ParaData[WAVES].Color[TempColor].BufR;
				TopG = ParaData[WAVES].Color[TempColor].BufG;
				TopB = ParaData[WAVES].Color[TempColor].BufB;
				TempR = 0;
				TempG = 0;
				TempB = 0;

				if (TopR >= TopG && TopR >= TopB){
					FadeR = 0;
					FadeG = TopG / (ParaData[WAVES].Other + 1);
					FadeB = TopB / (ParaData[WAVES].Other + 1);
					TempR = TopR;
				}
				else if (TopG >= TopB && TopG >= TopR){
					FadeG = 0;
					FadeR = TopR / (ParaData[WAVES].Other + 1);
					FadeB = TopB / (ParaData[WAVES].Other + 1);
					TempG = TopG;
				}
				else if (TopB >= TopR && TopB >= TopG){
					FadeB = 0;
					FadeR = TopR / (ParaData[WAVES].Other + 1);
					FadeG = TopG / (ParaData[WAVES].Other + 1);
					TempB = TopB;
				}
				TempStep = 0;
			}break;
		}

		for (i = 0; i < LED_TOTAL - 1; i++)
		{
			LedData[i].DutyR = LedData[i+1].DutyR;
			LedData[i].DutyG = LedData[i+1].DutyG;
			LedData[i].DutyB = LedData[i+1].DutyB;
		}
		i = LED_TOTAL - 1;
		LedData[i].DutyR = TempR;
		LedData[i].DutyG = TempG;
		LedData[i].DutyB = TempB;
	}
}

//if the layermax > FADE_LEVEL, this mode need to fixed
void Display_Str_Updwn(void)
{
	uint16 i = 0, j = 0, k = 0;

	if(Display.Init == true){	
		Display.Init = false;

		Para_Err_Check(&ParaData[UPDWN]);

		BrightLevel = 1;

		SpeedCtrl 	= 0;
		OtherCtrl 	= 0;
		TempColor 	= 0;
		TempStep  	= 0;
		LayerStep 	= 0;
		RptCtrl 	= 0;
		TempColor 	= 0;
		TempColor1 = 1;
		
		FadeR = ParaData[UPDWN].Color[TempColor].BufR / FADE_LEVEL;
		FadeG = ParaData[UPDWN].Color[TempColor].BufG / FADE_LEVEL;
		FadeB = ParaData[UPDWN].Color[TempColor].BufB / FADE_LEVEL;
		FadeR1 = ParaData[UPDWN].Color[TempColor1].BufR / FADE_LEVEL;
		FadeG1 = ParaData[UPDWN].Color[TempColor1].BufG / FADE_LEVEL;
		FadeB1 = ParaData[UPDWN].Color[TempColor1].BufB / FADE_LEVEL;

		TempR = 0;
		TempG = 0;
		TempB = 0;

		Display_All_Set(0,0,0);

		return;
	}

	SpeedCtrl++;
	if (SpeedCtrl >= 2){
		SpeedCtrl = 0;

		switch (TempStep){
			case 0:{
				LayerStep++;
				if (LayerStep > LayerMax + FADE_LEVEL)	TempStep++;

				for (i = 0; i < LayerMax; i++){
					if (LayerStep < FADE_LEVEL){
						//from bottom to top
						if (i > LayerStep){
							TempR = 0;
							TempG = 0;
							TempB = 0;
						}
						else{
							j = FADE_LEVEL - LayerStep + i;
							TempR = FadeR * j;
							TempG = FadeG * j;
							TempB = FadeB * j;
						}

						//from top to bottom
						if ((LayerMax - 1 - i) > LayerStep){
							TempR1 = 0;
							TempG1 = 0;
							TempB1 = 0;
						}
						else{
							j = FADE_LEVEL - LayerStep + LayerMax    - 1 - i;
							TempR1 = FadeR1 * j;
							TempG1 = FadeG1 * j;
							TempB1 = FadeB1 * j;
						}
					}
					else{
						if (LayerStep < LayerMax)
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
							if (((LayerMax - 1 - i) < (LayerStep - FADE_LEVEL)) || (LayerMax - 1 - i > LayerStep)){
								TempR1 = 0;
								TempG1 = 0;
								TempB1 = 0;
							}
							else{
								j = LayerMax - 1 - i - (LayerStep - FADE_LEVEL);
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
							if ((LayerMax - 1 - i) < (LayerStep - FADE_LEVEL)){
								TempR1 = 0;
								TempG1 = 0;
								TempB1 = 0;
							}
							else{
								j = LayerMax - 1 - i - (LayerStep - FADE_LEVEL);
								TempR1 = FadeR1 * j;
								TempG1 = FadeG1 * j;
								TempB1 = FadeB1 * j;
							}
						}
					}

					//get the layer color value
					k = (uint16_t)TempR + (uint16_t)TempR1;
					if (k > 255)	k = 255;
					for (j = Layer[i].Head; j <= Layer[i].Tail; j++)	LedData[j].DutyR = (uint8_t)k;
					k = (uint16_t)TempG + (uint16_t)TempG1;
					if (k > 255)	k = 255;
					for (j = Layer[i].Head; j <= Layer[i].Tail; j++)	LedData[j].DutyG = (uint8_t)k;
					k = (uint16_t)TempB + (uint16_t)TempB1;
					if (k > 255)	k = 255;
					for (j = Layer[i].Head; j <= Layer[i].Tail; j++)	LedData[j].DutyB = (uint8_t)k;
				}
			}break;

			//change the color
			default:{
				TempColor++;
				if (TempColor >= ParaData[UPDWN].ColorNum)	TempColor = 0;
				TempColor1++;
				if (TempColor1 >= ParaData[UPDWN].ColorNum) TempColor1 = 0;
				FadeR = ParaData[UPDWN].Color[TempColor].BufR / FADE_LEVEL;
				FadeG = ParaData[UPDWN].Color[TempColor].BufG / FADE_LEVEL;
				FadeB = ParaData[UPDWN].Color[TempColor].BufB / FADE_LEVEL;
				FadeR1 = ParaData[UPDWN].Color[TempColor1].BufR / FADE_LEVEL;
				FadeG1 = ParaData[UPDWN].Color[TempColor1].BufG / FADE_LEVEL;
				FadeB1 = ParaData[UPDWN].Color[TempColor1].BufB / FADE_LEVEL;
				TempStep = 0;
				LayerStep = 0;
			}break;
		}
	}
}


void Display_Str_Vintage(void)
{
	Display.Mode = COLOR_RAND;
	Display.ModeBuf = COLOR_RAND;
}
void Display_Str_Glow(void)
{
	Display.Mode = COLOR_RAND;
	Display.ModeBuf = COLOR_RAND;
}
void Display_Str_Color_Rand(void)
{
  uint16_t    i = 0;
  
  if (Display.Init == true){
    Display.Init  = false;
    Para_Err_Check(&ParaData[COLOR_RAND]);
    BrightLevel = 1;

    SpeedCtrl = 0;
    OtherCtrl = ParaData[COLOR_RAND].Other;

    TempR = 0;
    TempG = 0;
    TempB = 0;

    HoldTime = 0;
    Display_All_Set(0,0,0); 
    return;
	}
  
  HoldTime++;
  if (HoldTime > OtherCtrl * 20){
    HoldTime = 0;
    TempStep++;
    if (TempStep > 5)   TempStep = 0;
  }
    
  SpeedCtrl++;
  if (SpeedCtrl > (PARA_SPEED_MAX - ParaData[COLOR_RAND].Speed)){
    SpeedCtrl = 0;
    switch (TempStep){
      case 0:        TempB = 0; TempR += 12; TempG -= 7;	break;
      case 1:        TempG = 0; TempB -= 15; TempR += 9;	break;
      case 2:        TempR = 0; TempG += 7; TempB -= 16;	break;
      case 3:        TempB = 0; TempR -= 13; TempG += 8;	break;
      case 4:        TempG = 0; TempB += 10; TempR -= 7;	break;
      default:       TempR = 0; TempG -= 8; TempB += 11;	break;
    }
    
    
    for (i = LED_TOTAL - 1; i > 0; i--)
	{
		LedData[i].DutyR = LedData[i - 1].DutyR;
		LedData[i].DutyG = LedData[i - 1].DutyG;
		LedData[i].DutyB = LedData[i - 1].DutyB;
	}
	LedData[0].DutyR = TempR;
	LedData[0].DutyG = TempG;
	LedData[0].DutyB = TempB;
  }
}


