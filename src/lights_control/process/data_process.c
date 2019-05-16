/*
 * FILENAME	Data_Process
 * BRIEF	process all data for mode, timer, color, communication(app & voice & mcu)
**/
#include "Data_Process.h"


#define	DATA_PROCESS_DEBUG		0

//extern global parameters
extern cntdwn_T	cntdwn_data;


//global parameters
uint8_t           	ColorData[20][3];
_type_app_pack    	app_pack, app_ack_pack;
_type_voice_cmd		voice_cmd;



// File Parameters
_type_status    	AllStatus;
uint8_t				name_chksum;
_type_mcu_uart  	mcu_rcv_pack;
bool				mode_change_flag;
uint16_t			mode_change_time;


extern int sys_eeprom_read(int address, u8 *p_buf, int len);
extern int sys_eeprom_write(int address, u8 *p_buf, int len);




/*
 * COLOR_TABLE
 * DESCRIPTION	Color value combine with R value, G value and B value,
 *				or  with R value, G value , B value and W value
**/
const uint8_t  COLOR_VECTOR[][3]={
	{240,0,0},      		/* 0:red */
	{0,240,0},      		/* 1:green */
	{0,0,240},      		/* 2:blue */
	{240,30,0},     		/* 3:orange */
	{240,30,30},   		  	/* 4:pink */
	{0,240,45},     		/* 5:spring green */
	{240,105,0},    		/* 6:gold */
	{240,0,45},     		/* 7:peach */
	{240,225,0},    		/* 8:lawn green */
	{240,0,225},    		/* 9:Cyan */
	{0,240,240},    		/* a:sky blue */
	{240,180,0},    		/* b:yellow */
	{105,0,240},     		/* c:purple */
	{240,195,225},      	/* d:white */
	{225,240,240},      	/* e:cold white */
	{240,105,15},       	/* f:warm white */
};

//LED gamma table
#if GAMMA == 1

const uint8_t  GAMMA_TABLE[] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
	2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
	5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 
};

#endif



/*
 *
 * FUNCTION NAME	User_Data_Init
 * INPUT			None
 * OUTPUT			None
 * BRIEF			Initialize the user data after power on, include display
 *					mode, color theme, time count down hour and others
 *
**/
void User_Data_Init(void)
{
	uint8_t 	temp = 0;
	bool 		user_rst = true;	

	Display_Layout_None_Init();

	//initialize key port 
	pinMode(MODE_PIN, INPUT);
	pinMode(COLOR_PIN, INPUT);

	//initialize user data flash 
	#if USER_DATA_EN==1
	
	sys_eeprom_read(POWER_FIRST_OFFSET, (uint8_t *)&power, POWER_FIRST_LEN);
	#if DATA_PROCESS_DEBUG==1
	m2m_bytes_dump((u8 *)"[INFO] power =", &power, 1);
	#endif
	
	if (power != 0xaa)
	{
		user_rst = true;
	}
#if 0
	//it's not the first time to power on
	else	
	{
		#if DATA_PROCESS_DEBUG==1
		m2m_bytes_dump((u8 *)"[INFO] power = 0xaa", 0, 0);
		#endif
		
		//restore the mode before power off from flash 
		sys_eeprom_read(MODE_OFFSET, (uint8_t *)&Display.ModeBuf, MODE_LEN);
		if (Display.ModeBuf > MODE_MAX)
		{
			#if DATA_PROCESS_DEBUG==1
			m2m_bytes_dump((u8 *)"==INFO== mode > MODE_MAX!", 0, 0);
			#endif
			Display.ModeBuf = STEADY;
			sys_eeprom_write(MODE_OFFSET, (uint8_t *)&Display.ModeBuf, MODE_LEN);
		}
		Display.Mode = Display.ModeBuf;

		//restore the layout number
		sys_eeprom_read(LAYOUT_OFFSET, (uint8_t *)Display.LayoutNum, LAYOUT_LEN);

		if (Display.LayoutNum == LAYOUT_2D || Display.LayoutNum == LAYOUT_3D)
		{
			#if DATA_PROCESS_DEBUG==1
			m2m_bytes_dump((u8 *)"==INFO==LayoutNum = 2D/3D!", 0, 0);
			#endif
			//restore the layer information
			sys_eeprom_read(LAYER_NUM_OFFSET, (uint8_t *)&LayerMax, LAYER_NUM_LEN);
			uint8_t len = LayerMax * 4;
			sys_eeprom_read(LAYER_OFFSET, (uint8_t *)&Layer, len);
		}
		else
		{
			if (Display.LayoutNum > LAYOUT_3D)
			{
				#if DATA_PROCESS_DEBUG==1
				m2m_bytes_dump((u8 *)"==INFO== LayoutNum > 3D!", 0, 0);
				#endif
				Display.LayoutNum = LAYOUT_NONE;
				sys_eeprom_write(LAYOUT_OFFSET, (uint8_t *)Display.LayoutNum, LAYOUT_LEN);
			}
			Display_Layout_None_Init();
		}

		//restore the count down time
		sys_eeprom_read(TIMER_OFFSET, (uint8_t *)&cntdwn_data.on_hour, TIMER_LEN);
		if (cntdwn_data.on_hour >= 24 || cntdwn_data.on_min >= 60 || cntdwn_data.off_hour >= 24 || cntdwn_data.off_min >= 60)
		{
			cntdwn_data.on_hour = 0xff;
			cntdwn_data.on_min = 0xff;
			cntdwn_data.off_min = 0xff;
			cntdwn_data.off_hour = 0xff;
		}

		//restore the name check value
		sys_eeprom_read(NAME_CHK_OFFSET, (uint8_t *)&AllStatus.nam_check, NAME_CHK_LEN);
	}
#endif

	#endif


	//when need to reset all user data
	if (user_rst)
	{
		#if USER_DATA_EN==1
		power = 0xaa;
		sys_eeprom_write(POWER_FIRST_OFFSET, (uint8_t *)&power, POWER_FIRST_LEN);

		#endif

		Display.LayoutNum 		= LAYOUT_2D;
		#if USER_DATA_EN==1
		sys_eeprom_write(LAYOUT_OFFSET, (uint8_t *)&Display.LayoutNum, LAYOUT_LEN);
		#endif

		Display.ModeBuf 		= STEADY;
		Display.Mode 			= STEADY;
		Display.Init			= true;
		#if USER_DATA_EN==1
		//sys_eeprom_write(MODE_OFFSET, (uint8_t *)&Display.ModeBuf, MODE_LEN);
		#endif

		cntdwn_data.cntdwn_hour = 0;
		#if USER_DATA_EN==1
		//sys_eeprom_write(CNTDWN_OFFSET, (uint8_t *)&cntdwn_data.cntdwn_hour, CNTDWN_LEN);
		#endif

		#if USER_DATA_EN==1
		//sys_eeprom_write(PASSWORD_OFFSET, (uint8_t *)&pw_chksum, PASSWORD_LEN);
		#endif

		name_chksum = 0;
		#if USER_DATA_EN==1
		//sys_eeprom_write(NAME_CHK_OFFSET, (uint8_t *)&name_chksum, NAME_CHK_LEN);
		#endif
		
		Display_Layout_None_Init();
		#if USER_DATA_EN==1
		//sys_eeprom_write(LAYER_NUM_OFFSET, (uint8_t *)&LayerMax, LAYER_NUM_LEN);
		//sys_eeprom_write(LAYER_OFFSET, (uint8_t *)&Layer, LAYER_NUM_LEN);
		#endif
		
		for (temp = 0; temp <= MODE_MAX; temp++)
	  	{
			ParaData[temp].Mode = temp;
			ParaData[temp].Speed = 3;
			ParaData[temp].Bright = 4;
			ParaData[temp].Other = 0;
			ParaData[temp].ColorNum = 0;
			ParaData[temp].ColorVal = COLOR_SELF;
			ParaData[temp].Chksum = 0;
			ParaData[temp].Reserve1 = 0;
			for (uint8_t i = 0; i <= PARA_COLORNUM_MAX; i++)
	    	{
				ParaData[temp].Color[i].BufR = 0;
				ParaData[temp].Color[i].BufG = 0;
				ParaData[temp].Color[i].BufB = 0;
			}
		}

		temp = STEADY;
		ParaData[temp].ColorNum = 1;
		ParaData[temp].ColorVal = 0;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];


		temp = SPARKLE;
		ParaData[temp].Other = 10;
		ParaData[temp].ColorNum = 3;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[BLUE][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[COLD_WHITE][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[COLD_WHITE][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[COLD_WHITE][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[SKY_BLUE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[SKY_BLUE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[SKY_BLUE][B];

		temp = RAINBOW;
		ParaData[temp].Other = 4;

		temp = FADE;
		ParaData[temp].Bright = 4;
		ParaData[temp].ColorNum = 3;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[GREEN][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[GREEN][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[GREEN][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[BLUE][B];

		temp = SNOW;
		ParaData[temp].Speed = 4;
		ParaData[temp].ColorNum = 1;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[WHITE][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[WHITE][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[WHITE][B];

		temp = SNAKE;
		ParaData[temp].Other  = 4;
		ParaData[temp].ColorNum = 3;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[BLUE][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[WHITE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[WHITE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[WHITE][B];

		temp = TWINKLE;
		ParaData[temp].Other  = 5;
		ParaData[temp].ColorNum = 1;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[ORANGE][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[ORANGE][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[ORANGE][B];


		temp = FIREWORKS;
		ParaData[temp].ColorNum = 8;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[GREEN][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[GREEN][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[GREEN][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[BLUE][B];
		ParaData[temp].Color[3].BufR = COLOR_VECTOR[ORANGE][R];
		ParaData[temp].Color[3].BufG = COLOR_VECTOR[ORANGE][G];
		ParaData[temp].Color[3].BufB = COLOR_VECTOR[ORANGE][B];
		ParaData[temp].Color[4].BufR = COLOR_VECTOR[SPRING_GREEN][R];
		ParaData[temp].Color[4].BufG = COLOR_VECTOR[SPRING_GREEN][G];
		ParaData[temp].Color[4].BufB = COLOR_VECTOR[SPRING_GREEN][B];
		ParaData[temp].Color[5].BufR = COLOR_VECTOR[PEACH][R];
		ParaData[temp].Color[5].BufG = COLOR_VECTOR[PEACH][G];
		ParaData[temp].Color[5].BufB = COLOR_VECTOR[PEACH][B];
		ParaData[temp].Color[6].BufR = COLOR_VECTOR[LAWN_GREEN][R];
		ParaData[temp].Color[6].BufG = COLOR_VECTOR[LAWN_GREEN][G];
		ParaData[temp].Color[6].BufB = COLOR_VECTOR[LAWN_GREEN][B];
		ParaData[temp].Color[7].BufR = COLOR_VECTOR[PURPLE][R];
		ParaData[temp].Color[7].BufG = COLOR_VECTOR[PURPLE][G];
		ParaData[temp].Color[7].BufB = COLOR_VECTOR[PURPLE][B];

		temp = VERTIGO;
		ParaData[temp].Other  = 8;
		ParaData[temp].ColorNum = 2;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[WHITE][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[WHITE][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[WHITE][B];


		temp = HORIZONTAL;
		ParaData[temp].Other  = 10;
		ParaData[temp].ColorNum = 6;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[GREEN][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[GREEN][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[GREEN][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[BLUE][B];
		ParaData[temp].Color[3].BufR = COLOR_VECTOR[YELLOW][R];
		ParaData[temp].Color[3].BufG = COLOR_VECTOR[YELLOW][G];
		ParaData[temp].Color[3].BufB = COLOR_VECTOR[YELLOW][B];
		ParaData[temp].Color[4].BufR = COLOR_VECTOR[PEACH][R];
		ParaData[temp].Color[4].BufG = COLOR_VECTOR[PEACH][G];
		ParaData[temp].Color[4].BufB = COLOR_VECTOR[PEACH][B];
		ParaData[temp].Color[5].BufR = COLOR_VECTOR[WHITE][R];
		ParaData[temp].Color[5].BufG = COLOR_VECTOR[WHITE][G];
		ParaData[temp].Color[5].BufB = COLOR_VECTOR[WHITE][B];

		temp = WAVES;
		ParaData[temp].Other  = 5;
		ParaData[temp].ColorNum = 5;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[GREEN][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[GREEN][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[GREEN][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[BLUE][B];
		ParaData[temp].Color[3].BufR = COLOR_VECTOR[YELLOW][R];
		ParaData[temp].Color[3].BufG = COLOR_VECTOR[YELLOW][G];
		ParaData[temp].Color[3].BufB = COLOR_VECTOR[YELLOW][B];
		ParaData[temp].Color[4].BufR = COLOR_VECTOR[PEACH][R];
		ParaData[temp].Color[4].BufG = COLOR_VECTOR[PEACH][G];
		ParaData[temp].Color[4].BufB = COLOR_VECTOR[PEACH][B];

		temp = UPDWN;
		ParaData[temp].Other  = 5;
		ParaData[temp].ColorNum = 2;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[BLUE][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[BLUE][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[BLUE][B];

		temp = GLOW;
		ParaData[temp].ColorNum = 3;
		ParaData[temp].Color[0].BufR = COLOR_VECTOR[RED][R];
		ParaData[temp].Color[0].BufG = COLOR_VECTOR[RED][G];
		ParaData[temp].Color[0].BufB = COLOR_VECTOR[RED][B];
		ParaData[temp].Color[1].BufR = COLOR_VECTOR[GREEN][R];
		ParaData[temp].Color[1].BufG = COLOR_VECTOR[GREEN][G];
		ParaData[temp].Color[1].BufB = COLOR_VECTOR[GREEN][B];
		ParaData[temp].Color[2].BufR = COLOR_VECTOR[WHITE][R];
		ParaData[temp].Color[2].BufG = COLOR_VECTOR[WHITE][G];
		ParaData[temp].Color[2].BufB = COLOR_VECTOR[WHITE][B];

		temp = COLOR_RAND;
		ParaData[temp].Other  = 10;
	}


	//get check sum
	for (temp = 0; temp <= MODE_MAX; temp++)
	{
		ParaData[temp].Chksum = chksum_cal((const uint8_t *)&ParaData[temp], 8 + PARA_COLORNUM_MAX * 3);
	}
}



/*
 * FUNCTION NAME	Data_Process
 * INPUT			None
 * OUTPUT			None
 * DESCRIPTION		Process all the data here. change the mode , color, timer or
 *					other parameters after received valid data from WIFI
**/
void Data_Process(void)
{
  Key_Process();

  Count_Down_Process();

  Mcu_com_process();

  App_data_prcoess();
}



/*
 * FUNCTION NAME	Key_Process
 * DESCRIPTION      process the key status
**/
void Key_Process(void)
{
	uint8_t   i = 0;
	uint8_t	j = 0;
	uint8_t	k = 0;


	//short press mode key
	//change the mode
	if (KeyMode == KEY_SHORT)
	{
		KeyMode = KEY_IDLE;

		if (Display.Mode == POWER_OFF)
		{
			Display.Mode = POWER_ON;
		}
		else
		{
			Display.ModeBuf++;
			if (Display.ModeBuf > MODE_MAX)
			{
				Display.ModeBuf = 0;
				Display.Mode = POWER_OFF;
			}
			else
			{
				Display.Mode = Display.ModeBuf;
			}
		}

		Display.Init = true;

		mode_change_flag = true;
		mode_change_time = 0;
		
		//load to user buffer
		#if USER_DATA_EN==1
		//sys_eeprom_write(MODE_OFFSET, (uint8_t *)&Display.ModeBuf, MODE_LEN);
		#endif
	}
	
	//long press the mode key
	// wifi reset
	else if (KeyMode == KEY_LONG)
	{
		KeyMode = KEY_IDLE;
	}


	//short press the color key
	//change the color
	if (KeyColor == KEY_SHORT)
	{
		KeyColor = KEY_IDLE;

		i = Display.Mode;

		mode_change_flag = true;
		mode_change_time = 0;

		if ((i != POWER_OFF) && (i != RAINBOW) && (i != COLOR_RAND) && (i != CARNIVAL) && (i != ALTERNATE))
		{
		  Display.Init = true;

		  ParaData[i].ColorVal++;
		  if (Display.Mode == SNOW)
		  {
			if (ParaData[i].ColorVal > COLOR_VAL_MAX)    ParaData[i].ColorVal = 0;
		  }
		  else
		  {
		  	if (ParaData[i].ColorVal > COLOR_VAL_MAX + THEME_VAL_MAX)    ParaData[i].ColorVal = 0;
		  }

		  
		  j = Color_Value_Get(ParaData[i].ColorVal);
		  
		  ParaData[i].ColorNum = j;
		  for (k = 0; k < j; k++)
		  {
		    ParaData[i].Color[k].BufR = ColorData[k][R];
		    ParaData[i].Color[k].BufG = ColorData[k][G];
		    ParaData[i].Color[k].BufB = ColorData[k][B];
		  }

		  //Clear the old check sum
		  ParaData[i].Chksum = 0;
		  ParaData[i].Chksum = chksum_cal((const uint8_t *)&ParaData[i], 8 + j * 3);

			//user data load
		  #if USER_DATA_EN==1
		  //sys_eeprom_write(MODE_PARA_OFFSET + i * MODE_PARA_LEN, (uint8_t *)&ParaData[i], MODE_PARA_LEN);
		  #endif
		}
	}
	
	//long press the color key
	//change the count down timer
	else if (KeyColor == KEY_LONG)
	{
		KeyColor = KEY_IDLE;

		cntdwn_data.cntdwn_hour++;
		if (cntdwn_data.cntdwn_hour > 8)	cntdwn_data.cntdwn_hour = 0;
		cntdwn_data.init = true;
	}
}


/*
 * FUNCTION NAME	Count_Down_Process
 * DESCRIPTION		Action when time count down to turned on or turned off status
 */
void Count_Down_Process(void)
{
	if (cntdwn_data.status == CNTDWN_TURN_ON)
	{
		cntdwn_data.status = CNTDWN_IDLE;
		
		if (Display.Mode == POWER_OFF)
		{
			Display.Mode = POWER_ON;
			Display.Init = true;
		}
	}

	else if (cntdwn_data.status == CNTDWN_TURN_OFF)
	{
		cntdwn_data.status = CNTDWN_IDLE;
		
		if (Display.Mode != POWER_OFF)
		{
			Display.Mode = POWER_OFF;
			Display.Init = true;
		}
	}
}



/**
  * FunctionName  Mcu_com_process
  */
void Mcu_com_process(void)
{
	static uint8_t    len_pre;
	static uint8_t*   dst = (uint8_t *)&mcu_rcv_pack;

	static uint8_t	mode_pre = 0xff;
	static uint8_t	snd_index = 0x1;


	//check the mode change
	if (mode_change_flag)
	{
		mode_change_time++;
		if (mode_change_time >= 500)//5s
		{
			mode_change_time = 0;
			mode_change_flag = false;
		}
	}

	//send data
	//when the mode has changed or timer count down hour changed
	if ((Display.Mode != mode_pre) && (Display.Mode <= MODE_MAX || Display.Mode == POWER_OFF))
	{
		mode_pre = Display.Mode;
		uint8_t	snd_data[4];
		snd_data[0] = MCU_PROTOCOL_VER;
		snd_data[1] = snd_index;
		snd_index++;
		if (snd_index == 255)
		{
			snd_index = 1;
		}
		snd_data[2] = MCU_MODE_CMD;
		snd_data[3] = mode_pre;
		uart_write(uart0, (const char *)snd_data, 4);
	}

	else if (cntdwn_data.cntdwn_hour != cntdwn_data.pre_hour)
	{
		cntdwn_data.pre_hour = cntdwn_data.cntdwn_hour;
		
		uint8_t	snd_data[4];
		snd_data[0] = MCU_PROTOCOL_VER;
		snd_data[1] = snd_index;
		snd_index++;
		if (snd_index == 255)
		{
			snd_index = 1;
		}
		snd_data[2] = MCU_TIMER_CMD;
		snd_data[3] = cntdwn_data.cntdwn_hour;
		uart_write(uart0, (const char *)snd_data, 4);
	}
	
	else
	{
		//move data from uart0->rx_buffer mcu_rcv_pack 
		uint8_t len = uart_rx_available(uart0);

		if (len > 0)
		{
			len_pre += len;
			while(uart_rx_available(uart0))      *dst++ = (uint8_t)uart_read_char(uart0);
		}
		else if (len_pre > 0)
		{
			dst = (uint8_t *)&mcu_rcv_pack;

			//deal the received data
			//uart_write(uart0, (const char *)dst, len_pre);
			if (mcu_rcv_pack.cmd >= MCU_MUSIC_LOW)
			{
				if (!mode_change_flag)
				{
					if (Display.Mode != MUSIC_MODE  && Display.Mode <= MODE_MAX)
					{
						Display.Mode = MUSIC_MODE;
						Display.Init = true;
					}

					MusicUpdateFlag = true;

					LayerStep = (mcu_rcv_pack.cmd - 0x10);
					if (LayerStep > LayerMax)	LayerStep = LayerMax;
				}
			}
			len_pre = 0;
		}
	}
}


/**
  * FunctionName   App_data_process
  */
void App_data_prcoess(void)
{
	static uint8_t    index_rcv_pre;
	static uint8_t    index_rcv_time;

	//clear the received index
	if (index_rcv_time < 50)
	{
		index_rcv_time++;
	}
	else  
	{
		index_rcv_pre = 0;
	}


	//received data from voice control device
	if (app_pack.type == VOICE_PROTOCOL_VER)
	{
		uint8_t *src = (uint8_t *)&app_pack;
		uint8_t *dst = (uint8_t *)&voice_cmd;
		
		memcpy(dst, src, 12);

		//clear
		app_pack.type = 0;
		
		#if 1
		switch (voice_cmd.cmd)
		{
			case VOICE_ON_NEW:
			{
				if (voice_cmd.len == 1)
				{
					if (voice_cmd.payload[0] && Display.Mode == POWER_OFF)
					{
						Display.Mode   = POWER_ON;
						Display.Init = true;
					}
				}
			}break;

			case VOICE_OFF_NEW:
			{
				if (voice_cmd.len == 1)
				{
					if (!voice_cmd.payload[0] && Display.Mode != POWER_OFF)
					{
						Display.Mode = POWER_OFF;
						Display.Init = true;
					}
				}
			}break;

			case VOICE_MODE_NEW:
			{
				if (voice_cmd.len == 1)
				{
					uint8_t i = voice_cmd.payload[0];		//get the mode value
					if (i > MODE_MAX)		break;
					Display.Mode = i;
					Display.ModeBuf = i;
					Display.Init = true;

					mode_change_flag = true;
					mode_change_time = 0;
				}
			}break;

			case VOICE_COLOR_NEW:
			{
				if (voice_cmd.len != 1)	break;
				uint8_t i = voice_cmd.payload[0];	//get the color value
				if (i > COLOR_VAL_MAX + THEME_VAL_MAX)		break;

				mode_change_flag = true;
				mode_change_time = 0;

				//save the color values to ParaData
				uint8_t j = Display.Mode;
				uint8_t k = Color_Value_Get(i);
				ParaData[j].ColorVal = i;
				ParaData[j].ColorNum = k;
				for (i = 0; i < k; i++)
				{
					ParaData[j].Color[i].BufR = ColorData[i][R];
					ParaData[j].Color[i].BufG = ColorData[i][G];
					ParaData[j].Color[i].BufB = ColorData[i][B];
				}

				//caculate the new checksum
				ParaData[i].Chksum = 0;
				ParaData[i].Chksum = chksum_cal((const uint8_t *)&ParaData[i], 8 + j * 3);

				//intialize the mode
				Display.Init = true;
			}break;

			

			case VOICE_TIMER_NEW:
			{
				
			}break;
		}
		#endif
	}

	//receive data from APP
	else if (app_pack.type == PRODUCT_TYPE)
	{
		//m2m_printf("===== p = %p\n", app_pack);
		//check the app pack update
		if ((app_pack.index > 0) && (index_rcv_pre != app_pack.index))
		{
			index_rcv_pre = app_pack.index;
			index_rcv_time = 0;


			//if the type or the protocol version is mismatch
			if ((app_pack.type != PRODUCT_TYPE)
			|| (app_pack.ver != APP_PROTOCOL_VER))
			{
				app_pack.index = 0;
				//printf("==ERROR==version or type mismath!");
				return; 
			}


			//check the checksum value
			uint8_t*  p = (uint8_t *)&app_pack;
			uint8_t i = chksum_cal((const uint8_t *)p, app_pack.len + APP_PACK_HEADER_BYTE);
			
			if (i != 0)
			{
				app_pack.index = 0;

				//printf("==ERROR==index mismath!");
				return;
			}

			//clear index
			app_pack.index = 0;

			//app data process
			switch (app_pack.cmd)
			{

				case CHECK_ALL_STATUS_CMD:
				{
					//get the real time
					uint16_t len = app_pack.len;
					if (len != 3)		break;	//exit when the length is mismath
					
					uint8_t hour = app_pack.payload[0];
					uint8_t minute = app_pack.payload[1];
					uint8_t second = app_pack.payload[2];
					if (hour >= 24 || minute >= 60 || second >= 60)		break;		//the wrong time

					cntdwn_data.real_hour = hour;
					cntdwn_data.real_min = minute;
					cntdwn_data.real_sec = second;
					
					//reply
					len = MODE_MAX + ALL_STATUS_PACK_BYTE;
					uint8_t *src = (uint8_t *)&AllStatus;
					res_to_app(ALL_STATUS_ACK, (const uint8_t *)src, len);
				} break;

				case CHECK_MODE_STATUS_CMD:
				{
					uint8_t i = app_pack.payload[0];
					//exit if the inquire mode value is more than upper limiting value
					if (i > MODE_MAX)   break;

					uint16_t len = ParaData[i].Chksum * 3 + PARA_PACK_HEADRE_BYTE;
					uint8_t *src = (uint8_t *)&ParaData[i];
					res_to_app(MODE_STATUS_ACK, (const uint8_t *)src, len);
				}break;

				case CHECK_NAME_STATUS_CMD:
				{
					uint8_t *pname = (uint8_t *)malloc(NAME_LEN);
					uint16_t	len = 0;

					if (pname != NULL)
					{
						sys_eeprom_read(NAME_OFFSET, (uint8_t *)pname, NAME_LEN);

						//get the name real length
						uint8_t *p = pname;
						while (*pname != '\0')
						{
							p++;
							len++;
						}

						//reply
						res_to_app(NAME_STATUS_ACK, (const uint8_t *)pname, len);

						//free the memory
						free(pname);
						pname = NULL;
					}
					else
					{
						m2m_bytes_dump((u8 *)"==ERR== pname require memory error", NULL, 0);
					}
				}break;

				//0 - turn off
				//1 - turn on
				case SET_ON_OFF_CMD:
				{
					if (app_pack.len == 1)
					{
						if (app_pack.payload[0] && Display.Mode == POWER_OFF)
						{
							Display.Mode	= POWER_ON;
							Display.Init 	= true;
							mode_change_flag = true;
							mode_change_time = 0;
						}
						else if (!app_pack.payload[0] && Display.Mode != POWER_OFF)
						{
							Display.Mode = POWER_OFF;
							Display.Init = true;
							mode_change_flag = true;
							mode_change_time = 0;
						}
					}
				}break;

				//set mode
				//mode value, speed, bright level or other control value
				case SET_MODE_CMD:
				{
					uint8_t   i = app_pack.payload[0];  //mode
					uint8_t   j = app_pack.payload[1];  //speed
					uint8_t   k = app_pack.payload[2];  //bright level
					uint8_t   l = app_pack.payload[3];  //other
					if (i > MODE_MAX && i != MUSIC_MODE)    break;
					if (j > PARA_SPEED_MAX || k > PARA_BRIGHT_MAX|| l > PARA_OTHER_MAX)   break;

					mode_change_flag = true;
					mode_change_time = 0;

					Display.Init = true;
					Display.ModeBuf = i;
					Display.Mode = i;
					ParaData[i].Speed = j;
					ParaData[i].Bright = k;
					ParaData[i].Other = l;

					ParaData[i].Chksum = 0;
					ParaData[i].Chksum = chksum_cal((uint8_t *)&ParaData[i], 8 + ParaData[i].ColorNum * 3);
				}break;

				//set color
				//the maxium colors number is PARA_COLORNUM_MAX
				case SET_COLOR_CMD:
				{
					uint8_t   i = app_pack.payload[0];  //mode
					uint8_t   j = app_pack.payload[1];  //colornum
					if (i > MODE_MAX && i != MUSIC_MODE)        break;    //overflow
					if (j > PARA_COLORNUM_MAX)                  break;    //overflow
					if (i != Display.Mode)                      break;    //mismath with the current mode

					mode_change_flag = true;
					mode_change_time = 0;

					ParaData[i].ColorNum = j;
					for (uint8_t k = 0; k < j; k++)
					{
						uint8 l = k * 3 + 2;      //RGB->R inedex
						Color_Caculate(	&ParaData[i].Color[k].BufR,	&ParaData[i].Color[k].BufG,	&ParaData[i].Color[k].BufB,
										app_pack.payload[l], app_pack.payload[l+1],	app_pack.payload[l+2]);
					}

					ParaData[i].Chksum = 0;
					ParaData[i].ColorVal = COLOR_SELF;
					ParaData[i].Chksum = chksum_cal((const uint8_t *)&ParaData[i], 8 + j * 3);

					Display.Init = true;

					//printf("=RGB=(%d, %d, %d)\n", ParaData[i].Color[0].BufR, ParaData[i].Color[0].BufG, ParaData[i].Color[0].BufB);
				}break;

				case SET_CNTDWN_HOUR_CMD:
				{
					
				}break;

				case SET_CNTDWN_TIME_CMD:
				{
					uint16_t len = app_pack.len;
					if (len != 4)	break;		//mismatch

					uint8_t on_hour = app_pack.payload[0];
					uint8_t on_min = app_pack.payload[1];
					uint8_t off_hour = app_pack.payload[2];
					uint8_t off_min = app_pack.payload[3];
					//wrong time - instead the data by 0xff in order to reply to APP
					if (on_hour >= 24 || on_min >= 60 || off_hour >= 24 || off_min >= 60)
					{
						app_pack.payload[0] = 0xff;
						app_pack.payload[1] = 0xff;
						app_pack.payload[2] = 0xff;
						app_pack.payload[3] = 0xff;
					}
					//correct time
					else
					{
						cntdwn_data.on_hour = on_hour;
						cntdwn_data.on_min = on_min;
						cntdwn_data.off_hour = off_hour;
						cntdwn_data.off_min = off_min;
					}

					//reply
					res_to_app(CNTDWN_STATUS_ACK, (const uint8_t *)&app_pack.payload[0], 4);
				}break;

	//--------------------------------------------------------------------------------
	//                        LAYOUT
	//--------------------------------------------------------------------------------
	
				case LAYOUT_ENTER_CMD:
				{
					uint8_t reply = 0;
					
					if (app_pack.len == 1)
					{
						if (app_pack.payload[0] == 0 && (Display.Mode == LAYOUT_ENTER || Display.Mode == LAYOUT_TEST))
						{
							Display.Mode = LAYOUT_CANCEL;
							Display.Init = true;
							reply = 1;
						}

						else if (Display.Mode != LAYOUT_ENTER)
						{
							Display.Mode = LAYOUT_ENTER;
							Display.Init = true;
							reply = 1;
						}
					}

					res_to_app(LAYOUT_ENTER_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_TEST_CMD:
				{
					uint8_t reply = 0;
					
					if (app_pack.len == 5)
					{
						#if DATA_PROCESS_DEBUG==1
							m2m_bytes_dump((u8 *)"=Info=Rcv:", (u8 *)&app_pack, app_pack.len + 8);
						#endif
						
						uint16_t	head = ((uint16_t)app_pack.payload[1] << 8) + (uint16_t)app_pack.payload[2];
						uint16_t	tail = ((uint16_t)app_pack.payload[3] << 8) + (uint16_t)app_pack.payload[4];
						LayerTest = app_pack.payload[0];

						#if DATA_PROCESS_DEBUG==1
							//printf("=Info=Layer: 0x%x",LayerTest);
							//printf("=Info=Head: 0x%x",head);
							//printf("=Info=Tail: 0x%x",tail);
						#endif
						
						if (LayerTest < LAYER_MAX && head < LED_TOTAL && tail < LED_TOTAL 
						&& (Display.Mode == LAYOUT_ENTER || Display.Mode == LAYOUT_TEST))
						{
							LayerTemp[LayerTest].Head = head;
							LayerTemp[LayerTest].Tail = tail;
							Display.Mode = LAYOUT_TEST;
							Display.Init = true;
							reply = 1;
						}
					}

					res_to_app(LAYOUT_TEST_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_SAVE_CMD:
				{
					uint8_t reply = 0;
					
					if (app_pack.len == 4)
					{
						uint16_t	chk = ((uint16_t)app_pack.payload[2] << 8) + (uint16_t)app_pack.payload[3];
						uint16_t	chkcal = 0;
						LayerTest = app_pack.payload[0];

						//layout need set as 2D or 3D, the total layer need be less than the maximum value
						if (LayerTest <= LAYER_MAX && app_pack.payload[1] > 0)
						{
							for (uint8_t i = 0; i < LayerTest; i++)
							{
								chkcal ^= LayerTemp[i].Head;
								chkcal ^= LayerTemp[i].Tail;
							}
							
							if (chk == chkcal)
							{
								Display.Mode = LAYOUT_SAVE;
								Display.Init = true;
								Display.LayoutNum = app_pack.payload[1];
								reply = 1;
							}
						}
					}

					//exit the layout mode if error occur
					if (reply == 0)
					{
						Display.Mode = LAYOUT_CANCEL;
						Display.Init = true;
					}

					//save the layer information to user flash
					else
					{
						#if USER_DATA_EN==1
						sys_eeprom_write(LAYOUT_OFFSET, (uint8_t *)&Display.LayoutNum, LAYOUT_LEN);
						uint8_t len = LayerTest * 4;
						sys_eeprom_write(LAYER_NUM_OFFSET, (uint8_t *)&len, LAYER_NUM_LEN);
						sys_eeprom_write(LAYER_OFFSET, (uint8_t *)&LayerTemp, len);
						#endif
					}

					res_to_app(LAYOUT_SAVE_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_SEC_CTRL:
				{
					uint8_t		reply = 0;
					uint16_t	cnt = app_pack.len / 7;
					Display.Mode = LAYOUT_PHOTO_CTRL;

					while(cnt)
					{
						cnt--;
						uint8_t 	i = cnt * 7;
						uint16_t 	add_start = ((uint16_t)app_pack.payload[i] << 8) + (uint16_t)app_pack.payload[i+1];
						uint16_t 	add_end	= ((uint16_t)app_pack.payload[i+2] << 8) + (uint16_t)app_pack.payload[i+3];
						if ((add_start <= add_end) && (add_end < LED_TOTAL))
						{
							reply = 1;
							uint16_t j = 0;
							for (j = add_start; j <= add_end; j++)
							{
								LedData[j].DutyR = app_pack.payload[i+4];
								if (LedData[j].DutyR > 0xf0)	LedData[j].DutyR = 0xf0;
								LedData[j].DutyG = app_pack.payload[i+5];
								if (LedData[j].DutyG > 0xf0)	LedData[j].DutyG = 0xf0;
								LedData[j].DutyB = app_pack.payload[i+6];
								if (LedData[j].DutyB > 0xf0)	LedData[j].DutyB = 0xf0;
							}
						}
						else
						{
							break;
						}
					}

					res_to_app(LAYOUT_SEC_CTRL_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_MOD_CTRL:
				{
					uint8_t reply = 0;
					uint8_t mod = app_pack.payload[0];
					if (mod == 0)
					{
						res_to_app(LAYOUT_MOD_CTRL_ACK, (const uint8_t *)&reply, 1);
						break;
					}
					reply = 1;

					Display.Mode = LAYOUT_PHOTO_CTRL;

					uint16_t i = 0;
					uint8_t j = 0;
					uint8_t k = 0;
					for (i = 0; i < LED_TOTAL; i++)
					{
						j = mod;
						while (j)
						{
							j--;
							if ((i % mod) == j)
							{
								k = j * 3 + 1;
								LedData[i].DutyR = app_pack.payload[k];
								if (LedData[i].DutyR > 0xf0)	LedData[i].DutyR = 0xf0;
								LedData[i].DutyG = app_pack.payload[k+1];
								if (LedData[i].DutyG > 0xf0)	LedData[i].DutyG = 0xf0;
								LedData[i].DutyB = app_pack.payload[k+2];
								if (LedData[i].DutyB > 0xf0)	LedData[i].DutyB = 0xf0;
								break;
							}
						}
					}

					res_to_app(LAYOUT_MOD_CTRL_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_DOT_CTRL:
				{
					uint8_t		reply = 0;
					uint16_t	cnt = app_pack.len / 5;
					Display.Mode = LAYOUT_PHOTO_CTRL;

					while(cnt)
					{
						cnt--;
						uint16_t 	i = cnt * 5;
						uint16_t 	add_dot = ((uint16_t)app_pack.payload[i] << 8) + (uint16_t)app_pack.payload[i+1];
						if (add_dot < LED_TOTAL)
						{
							reply = 1;
							LedData[add_dot].DutyR = app_pack.payload[i+2];
							if (LedData[add_dot].DutyR > 0xf0)	LedData[add_dot].DutyR = 0xf0;
							LedData[add_dot].DutyG = app_pack.payload[i+3];
							if (LedData[add_dot].DutyG > 0xf0)	LedData[add_dot].DutyG = 0xf0;
							LedData[add_dot].DutyB = app_pack.payload[i+4];
							if (LedData[add_dot].DutyB > 0xf0)	LedData[add_dot].DutyB = 0xf0;
						}
						else
						{
							break;
						}
					}

					res_to_app(LAYOUT_DOT_CTRL_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_DOT_NOADD_CTRL:
				{
					uint8_t		reply = 1;
					uint16_t num = app_pack.len / 3;
					uint16_t cnt = 0;
					uint16_t color = 0;

					Display.Mode = LAYOUT_PHOTO_CTRL;

					while((cnt < num) && (cnt < LED_TOTAL))
					{
						color = cnt * 3;
						LedData[cnt].DutyR = app_pack.payload[color];
						if (LedData[cnt].DutyR > 0xf0)	LedData[cnt].DutyR = 0xf0;
						LedData[cnt].DutyG = app_pack.payload[color+1];
						if (LedData[cnt].DutyG > 0xf0)	LedData[cnt].DutyG = 0xf0;
						LedData[cnt].DutyB = app_pack.payload[color+2];
						if (LedData[cnt].DutyB > 0xf0)	LedData[cnt].DutyB = 0xf0;
						cnt++;
					}

					res_to_app(LAYOUT_DOT_NOADD_CTRL_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_DOT_NOADD_CTRL_2:
				{
					uint8_t		reply = 1;
					uint16_t num = app_pack.len / 3;
					uint16_t cnt = 0;
					uint16_t color = 0;

					Display.Mode = LAYOUT_PHOTO_CTRL;

					while((cnt < num) && ((cnt+200) < LED_TOTAL))
					{
						color = cnt * 3;
						LedData[cnt+200].DutyR = app_pack.payload[color];
						if (LedData[cnt+200].DutyR > 0xf0)	LedData[cnt+200].DutyR = 0xf0;
						LedData[cnt+200].DutyG = app_pack.payload[color+1];
						if (LedData[cnt+200].DutyG > 0xf0)	LedData[cnt+200].DutyG = 0xf0;
						LedData[cnt+200].DutyB = app_pack.payload[color+2];
						if (LedData[cnt+200].DutyB > 0xf0)	LedData[cnt+200].DutyB = 0xf0;
						cnt++;
					}

					res_to_app(LAYOUT_DOT_NOADD_CTRL_2_ACK, (const uint8_t *)&reply, 1);
				}break;
				
					

	//--------------------------------------------------------------------------------
	//                        VOICE CONTROL
	//--------------------------------------------------------------------------------

				case VOICE_SET_ON_OFF_CMD:
					{
						if (app_pack.len == 1)
						{
							if (app_pack.payload[0] && Display.Mode == POWER_OFF)
							{
								Display.Mode   = POWER_ON;
								Display.Init = true;
							}
							else if (!app_pack.payload[0] && Display.Mode != POWER_OFF)
							{
								Display.Mode = POWER_OFF;
								Display.Init = true;
							}
						}
					}break;

				case VOICE_SET_MODE_CMD:
					{
						if (app_pack.len == 1)
						{
							uint8_t i = app_pack.payload[0];		//get the mode value
							if (i > MODE_MAX)		break;
							Display.Mode = i;
							Display.ModeBuf = i;
							Display.Init = true;

							mode_change_flag = true;
							mode_change_time = 0;
						}
					}break;

				case VOICE_SET_COLOR_CMD:
					{
						if (app_pack.len != 1)	break;
						uint8_t i = app_pack.payload[0];	//get the color value
						if (i > COLOR_VAL_MAX)		break;

						mode_change_flag = true;
						mode_change_time = 0;

						//save the color values to ParaData
						uint8_t j = Display.Mode;
						uint8_t k = Color_Value_Get(i);
						ParaData[j].ColorVal = i;
						ParaData[j].ColorNum = k;
						for (i = 0; i < k; i++)
						{
							ParaData[j].Color[i].BufR = ColorData[i][R];
							ParaData[j].Color[i].BufG = ColorData[i][G];
							ParaData[j].Color[i].BufB = ColorData[i][B];
						}

						//caculate the new checksum
						ParaData[i].Chksum = 0;
						ParaData[i].Chksum = chksum_cal((const uint8_t *)&ParaData[i], 8 + j * 3);

						//intialize the mode
						Display.Init = true;
					}break;

				case VOICE_SET_THEME_CMD:
					{
						if (app_pack.len != 1)	break;
						uint8_t i = app_pack.payload[0];	//get the theme value
						if (i > THEME_VAL_MAX)		break;

						mode_change_flag = true;
						mode_change_time = 0;

						//save the color values to ParaData
						i = i + COLOR_VAL_MAX + 1;		//0x0f + 1
						uint8_t j = Display.Mode;
						uint8_t k = Color_Value_Get(i);
						ParaData[j].ColorVal = i;
						ParaData[j].ColorNum = k;
						for (i = 0; i < k; i++)
						{
							ParaData[j].Color[i].BufR = ColorData[i][R];
							ParaData[j].Color[i].BufG = ColorData[i][G];
							ParaData[j].Color[i].BufB = ColorData[i][B];
						}

						//caculate the new checksum
						ParaData[i].Chksum = 0;
						ParaData[i].Chksum = chksum_cal((const uint8_t *)&ParaData[i], 8 + j * 3);

						//initialize the mode
						Display.Init = true;
					}break;

				case VOICE_SET_CNTDWN_HOUR_CMD:
					{

					}break;

				case VOICE_SET_CNTDWN_TIME_CMD:
					{

					}break;

				case RENAME_DEVICE_CMD:
					{
						//check the length
						if (app_pack.payload[1] < 3 && app_pack.payload[1] > 50)	break;

						uint8_t *src = &app_pack.payload[2];
						uint8_t len = app_pack.payload[1];
						uint8_t i = chksum_cal((const uint8_t *)src, len);
						if (i == app_pack.payload[0])
						{
							AllStatus.nam_check = i;

							//set the last char to '\0'
							app_pack.payload[len + 2] = '\0';

							#if USER_DATA_EN==1
							//save the name to flash
							sys_eeprom_write(NAME_OFFSET, (uint8_t *)src, len + 1);

							//save the name checksum to flash
							sys_eeprom_write(NAME_CHK_OFFSET, (uint8_t *)&AllStatus.nam_check, NAME_CHK_LEN);
							#endif
						}
						else	i = AllStatus.nam_check;

						//reply
						res_to_app(NAME_MODIFY_ACK,(const uint8_t *)&i, 1);
						
					}break;

				default:
					{

					}break;
			}

			//clear
			#ifdef APP_PACK_CLEAR
			for (i = 0; i < (app_pack.len + 8); i++)  *(p--) = 0;
			#endif
		}
	}
}


/**
	* FunctionName	res_to_app
	* Input		
	*		> cmd - respond command
	*		> *pdata - data adress
	*		> len - data length
	*/
void res_to_app(uint8_t cmd,const uint8_t *pdata, uint16_t len)
{
	static uint8_t    index_snd_pre;

	if (index_snd_pre == 255) index_snd_pre = 0;
	else                      index_snd_pre++;
	app_ack_pack.type = PRODUCT_TYPE;
	app_ack_pack.ver = APP_PROTOCOL_VER;
	app_ack_pack.reserve = 0;		//old password checksum
	app_ack_pack.index = index_snd_pre;
	app_ack_pack.chksum = 0;
	app_ack_pack.cmd = cmd;
	app_ack_pack.len = len;

	uint8_t *dst = &app_ack_pack.payload[0];
	const uint8_t *src = pdata;
	memcpy(dst, src, len);
	src = (const uint8_t *)&app_ack_pack;
	app_ack_pack.chksum = chksum_cal(src, len + APP_PACK_HEADER_BYTE);
}


/**
  * FunctionName    Color_Caculate
  */
void Color_Caculate(uint8_t *rtR, uint8_t *rtG, uint8_t *rtB,
                           uint8_t rIn, uint8_t gIn, uint8_t bIn)
{
	#if GAMMA == 0
	
	uint16   tempBuf = 0;
	uint16  tempTotal = 0;

	gIn = (uint8_t)(((uint16)gIn * 9) / 10); //green value zoom out(0.9x)
	bIn = (uint8_t)(((uint16)bIn * 9) / 10); //blue value zoom out(0.9x)

	tempBuf = rIn / FADE_LEVEL * FADE_LEVEL;
	*rtR = (uint8_t)tempBuf;

	tempBuf = gIn / FADE_LEVEL * FADE_LEVEL;
	*rtG = (uint8_t)tempBuf;

	tempBuf = bIn / FADE_LEVEL * FADE_LEVEL;
	*rtB = (uint8_t)tempBuf;

	tempTotal = (uint16)*rtR + (uint16)*rtG + (uint16)*rtB;

	//except white
	if (tempTotal < 550)
	{
		if ((*rtR != 0) && (*rtG != 0) && (*rtB != 0))
		{
			if (*rtR == 0xfa)
			{
				if (*rtG > *rtB)      *rtB = FADE_LEVEL;
				else                  *rtG = FADE_LEVEL;
			}
			else if (*rtG == 0xfa)
			{
				if (*rtR > *rtB)      *rtB = FADE_LEVEL;
				else                  *rtR = FADE_LEVEL;
			}
			else
			{
				if (*rtR >= *rtG)     *rtG = FADE_LEVEL;
				else                  *rtR = FADE_LEVEL;
			}
		}
	}  

	#else
	
	*rtR = (GAMMA_TABLE[rIn] / FADE_LEVEL) * FADE_LEVEL;
	*rtG = (GAMMA_TABLE[gIn] / FADE_LEVEL) * FADE_LEVEL;
	*rtB = (GAMMA_TABLE[bIn] / FADE_LEVEL) * FADE_LEVEL;
	
	#endif
}


/**
  * FunctionName  CRC16_Cal
  * Brief         caculate the CRC16_CCITT value of buffer
  */
uint16_t	CRC16_Cal(uint8_t* Buffer, uint16_t len)
{
	uint16_t	ResultBuf = 0x0000;
	uint16_t	CntBuf = 0;
	uint8_t		BitBuf = 0;

	for (CntBuf = 0; CntBuf < len; CntBuf++)
	{
		ResultBuf ^= (((uint16_t)Buffer[CntBuf]) << 8);

		for (BitBuf = 0; BitBuf < 8; BitBuf++)
		{
			if ((ResultBuf & 0x8000) == 0x8000)	
			{
				ResultBuf = (ResultBuf << 1) ^ 0x1021;
			}
			else	ResultBuf <<= 1;
		}
	}

	return ResultBuf;
}

/**
  * FunctionName  chksum_cal
  * Input       
  *       > *buffer  - data head adress
  *       > len      - data length
  */
uint8_t   chksum_cal(const uint8_t *src, uint16_t len)
{
  uint8_t   chksum = 0;
  uint8_t	*buffer = (uint8_t *)src;
  while (len--) chksum ^= *buffer++;
  return chksum;
}


/****************************************************************************
 * Color_Value_Get
 */
uint8_t Color_Value_Get(uint8_t ColorNumBuf)
{
	uint8_t colorBuf = 0;
	uint8_t temp = 0;

  	// single color
	if (ColorNumBuf < 0x10)	
	{
		ColorData[0][0] = COLOR_VECTOR[ColorNumBuf][0];
		ColorData[0][1] = COLOR_VECTOR[ColorNumBuf][1];
		ColorData[0][2] = COLOR_VECTOR[ColorNumBuf][2];
		colorBuf = 1;
	} 

	// many color
	else{

		switch (ColorNumBuf)
		{
			// spring
			case 0x10:
				ColorData[0][0] = COLOR_VECTOR[SPRING_GREEN][0];
				ColorData[0][1] = COLOR_VECTOR[SPRING_GREEN][1];
				ColorData[0][2] = COLOR_VECTOR[SPRING_GREEN][2];

				ColorData[1][0] = COLOR_VECTOR[LAWN_GREEN][0];
				ColorData[1][1] = COLOR_VECTOR[LAWN_GREEN][1];
				ColorData[1][2] = COLOR_VECTOR[LAWN_GREEN][2];

				ColorData[2][0] = COLOR_VECTOR[PEACH][0];
				ColorData[2][1] = COLOR_VECTOR[PEACH][1];
				ColorData[2][2] = COLOR_VECTOR[PEACH][2];

				colorBuf = 3;
			break;

			// summer
			case 0x11:
				ColorData[0][0] = COLOR_VECTOR[GREEN][0];
				ColorData[0][1] = COLOR_VECTOR[GREEN][1];
				ColorData[0][2] = COLOR_VECTOR[GREEN][2];

				ColorData[1][0] = COLOR_VECTOR[RED][0];
				ColorData[1][1] = COLOR_VECTOR[RED][1];
				ColorData[1][2] = COLOR_VECTOR[RED][2];

				ColorData[2][0] = COLOR_VECTOR[LAWN_GREEN][0];
				ColorData[2][1] = COLOR_VECTOR[LAWN_GREEN][1];
				ColorData[2][2] = COLOR_VECTOR[LAWN_GREEN][2];

				ColorData[3][0] = COLOR_VECTOR[ORANGE][0];
				ColorData[3][1] = COLOR_VECTOR[ORANGE][1];
				ColorData[3][2] = COLOR_VECTOR[ORANGE][2];

				colorBuf = 4;
			break;

			// autumn
			case 0x12:
				ColorData[0][0] = COLOR_VECTOR[ORANGE][0];
				ColorData[0][1] = COLOR_VECTOR[ORANGE][1];
				ColorData[0][2] = COLOR_VECTOR[ORANGE][2];

				ColorData[1][0] = COLOR_VECTOR[GOLD][0];
				ColorData[1][1] = COLOR_VECTOR[GOLD][1];
				ColorData[1][2] = COLOR_VECTOR[GOLD][2];

				ColorData[2][0] = COLOR_VECTOR[CYAN][0];
				ColorData[2][1] = COLOR_VECTOR[CYAN][1];
				ColorData[2][2] = COLOR_VECTOR[CYAN][2];

				colorBuf = 3;

			break;

			// winter
			case 0x13:
				ColorData[0][0] = COLOR_VECTOR[COLD_WHITE][0];
				ColorData[0][1] = COLOR_VECTOR[COLD_WHITE][1];
				ColorData[0][2] = COLOR_VECTOR[COLD_WHITE][2];

				ColorData[1][0] = COLOR_VECTOR[SKY_BLUE][0];
				ColorData[1][1] = COLOR_VECTOR[SKY_BLUE][1];
				ColorData[1][2] = COLOR_VECTOR[SKY_BLUE][2];


				ColorData[2][0] = COLOR_VECTOR[BLUE][0];
				ColorData[2][1] = COLOR_VECTOR[BLUE][1];
				ColorData[2][2] = COLOR_VECTOR[BLUE][2];

				colorBuf = 3;
			break;

			// Christmas' day
			case 0x14:
				ColorData[0][0] = COLOR_VECTOR[RED][0];
				ColorData[0][1] = COLOR_VECTOR[RED][1];
				ColorData[0][2] = COLOR_VECTOR[RED][2];;

				ColorData[1][0] = COLOR_VECTOR[GREEN][0];
				ColorData[1][1] = COLOR_VECTOR[GREEN][1];
				ColorData[1][2] = COLOR_VECTOR[GREEN][2];

				ColorData[2][0] = COLOR_VECTOR[WHITE][0];
				ColorData[2][1] = COLOR_VECTOR[WHITE][1];
				ColorData[2][2] = COLOR_VECTOR[WHITE][2];

				colorBuf = 3;
			break;

			// Valentines' day
			case 0x15:
				ColorData[0][0] = COLOR_VECTOR[PEACH][0];
				ColorData[0][1] = COLOR_VECTOR[PEACH][1];
				ColorData[0][2] = COLOR_VECTOR[PEACH][2];

				ColorData[1][0] = COLOR_VECTOR[PINK][0];
				ColorData[1][1] = COLOR_VECTOR[PINK][1];
				ColorData[1][2] = COLOR_VECTOR[PINK][2];

				ColorData[2][0] = COLOR_VECTOR[CYAN][0];
				ColorData[2][1] = COLOR_VECTOR[CYAN][1];
				ColorData[2][2] = COLOR_VECTOR[CYAN][2];

				colorBuf = 3;
			break;

			// Independence day
			case 0x16:
				ColorData[0][0] = COLOR_VECTOR[RED][0];
				ColorData[0][1] = COLOR_VECTOR[RED][1];
				ColorData[0][2] = COLOR_VECTOR[RED][2];

				ColorData[1][0] = COLOR_VECTOR[WHITE][0];
				ColorData[1][1] = COLOR_VECTOR[WHITE][1];
				ColorData[1][2] = COLOR_VECTOR[WHITE][2];

				ColorData[2][0] = COLOR_VECTOR[BLUE][0];
				ColorData[2][1] = COLOR_VECTOR[BLUE][1];
				ColorData[2][2] = COLOR_VECTOR[BLUE][2];

				colorBuf = 3;
			break;

			// Thanks giving
			case 0x17:
				ColorData[0][0] = COLOR_VECTOR[SPRING_GREEN][0];
				ColorData[0][1] = COLOR_VECTOR[SPRING_GREEN][1];
				ColorData[0][2] = COLOR_VECTOR[SPRING_GREEN][2];

				ColorData[1][0] = COLOR_VECTOR[ORANGE][0];
				ColorData[1][1] = COLOR_VECTOR[ORANGE][1];
				ColorData[1][2] = COLOR_VECTOR[ORANGE][2];

				ColorData[2][0] = COLOR_VECTOR[RED][0];
				ColorData[2][1] = COLOR_VECTOR[RED][1];
				ColorData[2][2] = COLOR_VECTOR[RED][2];

				ColorData[3][0] = COLOR_VECTOR[LAWN_GREEN][0];
				ColorData[3][1] = COLOR_VECTOR[LAWN_GREEN][1];
				ColorData[3][2] = COLOR_VECTOR[LAWN_GREEN][2];

				ColorData[4][0] = COLOR_VECTOR[GOLD][0];
				ColorData[4][1] = COLOR_VECTOR[GOLD][1];
				ColorData[4][2] = COLOR_VECTOR[GOLD][2];

				colorBuf = 5;
			break;

			// st. patrick's day
			case 0x18:
				ColorData[0][0] = COLOR_VECTOR[GREEN][0];
				ColorData[0][1] = COLOR_VECTOR[GREEN][1];
				ColorData[0][2] = COLOR_VECTOR[GREEN][2];

				ColorData[1][0] = COLOR_VECTOR[SPRING_GREEN][0];
				ColorData[1][1] = COLOR_VECTOR[SPRING_GREEN][1];
				ColorData[1][2] = COLOR_VECTOR[SPRING_GREEN][2];

				ColorData[2][0] = COLOR_VECTOR[LAWN_GREEN][0];
				ColorData[2][1] = COLOR_VECTOR[LAWN_GREEN][1];
				ColorData[2][2] = COLOR_VECTOR[LAWN_GREEN][2];

				colorBuf = 3;
			break;

			// Halloween
			case 0x19:
				ColorData[0][0] = COLOR_VECTOR[ORANGE][0];
				ColorData[0][1] = COLOR_VECTOR[ORANGE][1];
				ColorData[0][2] = COLOR_VECTOR[ORANGE][2];

				ColorData[1][0] = COLOR_VECTOR[PURPLE][0];
				ColorData[1][1] = COLOR_VECTOR[PURPLE][1];
				ColorData[1][2] = COLOR_VECTOR[PURPLE][2];


				ColorData[2][0] = COLOR_VECTOR[GREEN][0];
				ColorData[2][1] = COLOR_VECTOR[GREEN][1];
				ColorData[2][2] = COLOR_VECTOR[GREEN][2];

				ColorData[3][0] = COLOR_VECTOR[RED][0];
				ColorData[3][1] = COLOR_VECTOR[RED][1];
				ColorData[3][2] = COLOR_VECTOR[RED][2];

				colorBuf = 4;
			break;

			// sun 
			case 0x1A:
				ColorData[0][0] = COLOR_VECTOR[RED][0];
				ColorData[0][1] = COLOR_VECTOR[RED][1];
				ColorData[0][2] = COLOR_VECTOR[RED][2];

				ColorData[1][0] = COLOR_VECTOR[ORANGE][0];
				ColorData[1][1] = COLOR_VECTOR[ORANGE][1];
				ColorData[1][2] = COLOR_VECTOR[ORANGE][2];

				ColorData[2][0] = COLOR_VECTOR[GOLD][0];
				ColorData[2][1] = COLOR_VECTOR[GOLD][1];
				ColorData[2][2] = COLOR_VECTOR[GOLD][2];

				ColorData[3][0] = COLOR_VECTOR[YELLOW][0];
				ColorData[3][1] = COLOR_VECTOR[YELLOW][1];
				ColorData[3][2] = COLOR_VECTOR[YELLOW][2];

				colorBuf = 4;
			break;

			// earth 
			case 0x1B:
				ColorData[0][0] = COLOR_VECTOR[BLUE][0];
				ColorData[0][1] = COLOR_VECTOR[BLUE][1];
				ColorData[0][2] = COLOR_VECTOR[BLUE][2];

				ColorData[1][0] = COLOR_VECTOR[GREEN][0];
				ColorData[1][1] = COLOR_VECTOR[GREEN][1];
				ColorData[1][2] = COLOR_VECTOR[GREEN][2];

				ColorData[2][0] = COLOR_VECTOR[SKY_BLUE][0];
				ColorData[2][1] = COLOR_VECTOR[SKY_BLUE][1];
				ColorData[2][2] = COLOR_VECTOR[SKY_BLUE][2];


				ColorData[3][0] = COLOR_VECTOR[ORANGE][0];
				ColorData[3][1] = COLOR_VECTOR[ORANGE][1];
				ColorData[3][2] = COLOR_VECTOR[ORANGE][2];

				colorBuf = 4;
			break;

			// Multi
			case 0x1C:
				for (temp = 0; temp < 12; temp++)
				{
					ColorData[temp][0] = COLOR_VECTOR[temp][0];
					ColorData[temp][1] = COLOR_VECTOR[temp][1];
					ColorData[temp][2] = COLOR_VECTOR[temp][2];
				}

				colorBuf = 12;
			break;
		}
	}

  	return colorBuf;
}
