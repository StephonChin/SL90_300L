/**************************************************************
  * 
  * FileName  Display.c
  * Date      26 NOV 2018
  * Author    DS.Chin
  *
****************************************************************/
#include "Display.h"


//global parameters
Display_t   Display;
ModePara_t  ParaData[MODE_MAX + 1];
Layer_t     Layer[LAYER_MAX];
Layer_t 	LayerTemp[LAYER_MAX];
uint8_t     LayerMax;
uint8_t		LayerTest;
uint8_t 	MusicMode;
bool		MusicUpdateFlag;


uint8_t           SpeedCtrl;
uint8_t           OtherCtrl;
uint8_t           TempR;
uint8_t           TempG;
uint8_t           TempB;
uint8_t           TempR1;
uint8_t           TempG1;
uint8_t           TempB1;
uint8_t           TopR;
uint8_t           TopG;
uint8_t           TopB;
uint8_t           FadeR;
uint8_t           FadeG;
uint8_t           FadeB;
uint8_t           TempColor;
uint8_t           FadeR1;
uint8_t           FadeG1;
uint8_t           FadeB1;
uint8_t           TempColor1;
uint8_t           TempStep;
uint8_t           FadeLevel;
uint16_t          RptCtrl;
uint16_t          RptTotal;
uint16_t          HoldTime;
uint8_t           LayerStep;
uint16_t          LedPickAll;
uint8_t           ModeTime[LED_TOTAL];
uint8_t           ModeStep[LED_TOTAL];
uint16_t          LedPick[LED_TOTAL];
int16_t           RndSeed;
bool			  ModeFirstFlag;




/**
  * FunctionName    Display_control
  */
void Display_Control(void)
{
	//common mode display
	if (Display.Mode >= COMMON_MODE_LIMIT)
	{
		switch (Display.Mode)
		{  
			case POWER_OFF:   		Display_Power_Off();                break;
			case POWER_ON:    		Display_Power_On();                 break;
			case RED_FLASH:   		Display_All_Flash(250, 0, 0);       break;
			case GREEN_FLASH: 		Display_All_Flash(0, 250, 0);       break;
			case BLUE_FLASH:  		Display_All_Flash(0, 0, 250);       break;
			case LAYOUT_PHOTO_CTRL:	Display_Layout_Photo_Ctrl();		break;
			case LAYOUT_ENTER: 		Display_Layout_Enter();				break;
			case LAYOUT_CANCEL: 	Display_Layout_Cancel();			break;
			case LAYOUT_TEST:		Display_Layout_Test();				break;
			case LAYOUT_SAVE:		Display_Layout_Save();				break;
			case MUSIC_MODE:		Display_Music();					break;
			default:  break;
		}

		return;
	}

	//string mode display
	if (Display.LayoutNum == LAYOUT_NONE)
	{
		switch (Display.Mode)
		{
			case STEADY:      	Display_Str_Steady();        break;
			case SPARKLE:     	Display_Str_Sparkle();       break;
			case RAINBOW:     	Display_Str_Rainbow();       break;
			case FADE:        	Display_Str_Fade();          break;
			case SNOW:        	Display_Str_Snow();          break;
			case SNAKE:       	Display_Str_Snake();         break;
			case TWINKLE:     	Display_Str_Twinkle();       break;
			case FIREWORKS:   	Display_Str_Fireworks();     break;
			case HORIZONTAL:  	Display_Str_Horizontal();    break;
			case WAVES:       	Display_Str_Waves();         break;
			case UPDWN:       	Display_Str_Updwn();         break;
			case VINTAGE:     	Display_Str_Vintage();       break;
			case GLOW:        	Display_Str_Glow();          break;
			case COLOR_RAND:  	Display_Str_Color_Rand();    break;
			

			default:
			{
				Display.Mode = STEADY;
				Display.ModeBuf = STEADY;
			}break;
		}

		return;
	}


	//tree mode display
	switch (Display.Mode)
	{
		case STEADY:      	Display_Tree_Steady();        	break;
		case SPARKLE:     	Display_Tree_Sparkle();       	break;
		case RAINBOW:     	Display_Tree_Rainbow();       	break;
		case FADE:        	Display_Tree_Fade();          	break;
		case SNOW:        	Display_Tree_Snow();          	break;
		case SNAKE:       	Display_Tree_Snake();         	break;
		case TWINKLE:     	Display_Tree_Twinkle();       	break;
		case FIREWORKS:   	Display_Tree_Fireworks();     	break;
		case VERTIGO:     	Display_Tree_Vertigo();       	break;
		case HORIZONTAL:  	Display_Tree_Horizontal();    	break;
		case WAVES:       	Display_Tree_Waves();         	break;
		case UPDWN:       	Display_Tree_Updwn();         	break;
		case DIAGONAL:    	Display_Tree_Diagonal();      	break;
		case SUNSET:      	Display_Tree_Sunset();        	break;
		case VINTAGE:     	Display_Tree_Vintage();       	break;
		case GLOW:        	Display_Tree_Glow();          	break;
		case COLOR_RAND:  	Display_Tree_Color_Rand();    	break;
		case CARNIVAL:		Display_Tree_Carnival();	 	break;
		case ALTERNATE:		Display_Tree_Alternate();	 	break;
		default:    										break;
	}
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//---------------------------Common Functions-------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
/**
  * FunctionName  Para_Err_Check
  * Input         PataData
  * Output        New checksum
  */
uint8_t  Para_Err_Check(ModePara_t * para)
{
  uint8_t   chksum = para->Chksum;
  char      err = 0;

  //reset the bright to 4 level
  if (para->Bright > PARA_BRIGHT_MAX){
    chksum ^= para->Bright;
    para->Bright = 0x4;
    chksum ^= 0x4;
    err |= 0x01;
  }

  //reset the speed to maximum
  if (para->Speed > PARA_SPEED_MAX){
    chksum ^= para->Speed;
    para->Speed = PARA_SPEED_MAX;
    chksum ^= PARA_SPEED_MAX;
    err |= 0x02;
  }

  //reset the othr parameter to maximum
  if (para->Other > PARA_OTHER_MAX){
    chksum ^= para->Other;
    para->Other = PARA_OTHER_MAX;
    chksum ^= PARA_OTHER_MAX;
    err |= 0x04;
  }

  //reset the color to red
  if (para->ColorNum > PARA_COLORNUM_MAX || para->ColorNum == 0){
    chksum ^= para->ColorNum;
    chksum ^= para->Color[0].BufR;
    chksum ^= para->Color[0].BufG;
    chksum ^= para->Color[0].BufB;
    para->ColorNum = 0x1;
    para->Color[0].BufR = 250;
    para->Color[0].BufG = 0;
    para->Color[0].BufB = 0;
    chksum ^= 0x1;
    chksum ^= 250;
    err |= 0x08;
  }

  return err;
}




/**
  * FunctionName  Display_all_set
  * Para
  *     ==> r : red data
  *     ==> g : green data
  *     ==> b : blue data
  */
void Display_All_Set(uint8_t r, uint8_t g, uint8_t b)
{
  uint16_t  i;

  for (i = 0; i < LED_TOTAL; i++){
    LedData[i].DutyR = r;
    LedData[i].DutyG = g;
    LedData[i].DutyB = b;
  }
} 


/**
  * FunctionName  Display_power_off
  */
void Display_Power_Off(void)
{
  if (Display.Init == true){
    Display.Init = false;
    Display_All_Set(0, 0, 0);
  }
}


/**
  * FunctionName  Display_power_on
  */
void Display_Power_On(void)
{
  //error check
  if (Display.ModeBuf >= MODE_MAX)
  {
    Display.ModeBuf = 0x1; 
  }
  Display.Mode = Display.ModeBuf;
  Display.Init = true;
}


/**
  * FunctionName    Display_all_flash
  */
void Display_All_Flash(uint8_t r, uint8_t g, uint8_t b)
{
  if (Display.Init == true){
    Display.Init = false;
    BrightLevel = 1;
    Display_All_Set(0,0,0);
    SpeedCtrl = 0;
    TempStep = 0;
  }

  SpeedCtrl++;
  if (SpeedCtrl >= 3){
    SpeedCtrl = 0;
    
    switch (TempStep){
      case 0:
      case 2:
      case 4:{
        Display_All_Set(r, g, b);
        TempStep++;
      } break;

      case 1:
      case 3:
      case 5:{
        Display_All_Set(0, 0, 0);
        TempStep++;
      } break;

      default:{
        Display.Mode = Display.ModeBuf;
        Display.Init = true;
      } break;
    }
  }  
}








