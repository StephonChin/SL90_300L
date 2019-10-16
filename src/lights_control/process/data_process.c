/*
 * FILENAME	Data_Process
 * BRIEF	process all data for mode, timer, color, communication(app & voice & mcu)
**/
#include "Data_Process.h"


#define	DATA_PROCESS_DEBUG		0


//global parameters
/* This value will be set to 0xaa5555aa if it's not the first time to write flash*/
uint32_t			flash_first_number;		
uint8_t           	ColorData[20][3];
_type_app_pack    	app_pack, app_ack_pack;
_type_voice_cmd		voice_cmd;



// File Parameters
uint8_t				name_chksum;
_type_mcu_uart  	mcu_rcv_pack;
bool				mode_change_flag;
uint16_t			mode_change_time;


uint8_t	  index_rcv_pre;
uint8_t	  index_rcv_time;



/**
  * enable_user_normal_flash_write
  */
static __inline__ void enable_user_normal_flash_write(void)
{
	user_normal_flash_write_clear_flag 	= true;
	user_normal_flash_write_flag 		= true;
}



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
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,254 
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
	bool 		user_rst = true;	

	//initialize key port 
	pinMode(MODE_PIN, INPUT);
	pinMode(COLOR_PIN, INPUT);
	pinMode(PWM_EN_PIN, OUTPUT);
	digitalWrite(PWM_EN_PIN, 1);

	/* Checksum the all paramters value
	 * Reset or restore all paramters
	 */

	//Get the parameters' data from user normal flash 
	read_user_normal_flash();
	
	//Yes - Confirm if it is the first time to write the user normal flash? 
	if (flash_first_number != 0x55aa5555)
	{
		flash_first_number = 0x55aa5555;
		
		display_data.mode_buf 	= STEADY;
		display_data.mode 		= display_data.mode_buf;
		display_data.init 		= true;

		timing_data.cntdwn_hour = 0;
		timing_data.en_flag = 0;
		for (uint8_t i = 0; i < TIMING_GRP_MAX; i++)
		{
			timing_data.timing_grp[i].on_hour 		= 0;
			timing_data.timing_grp[i].on_minute 	= 0;
			timing_data.timing_grp[i].off_hour 		= 0;
			timing_data.timing_grp[i].off_minute 	= 0;
		}

		music_data.enable_flag 	= false;
		music_data.mode 		= 0;

		vertical_layer.en_flag = false;
		triangle_layer.en_flag = false;
		fan_layer.en_flag = false;
		Display_Layout_None_Init();


		for (uint8_t i = 0; i < MODE_MAX; i++)
	  	{
			mode_para_data[i].Mode 		= i;
			mode_para_data[i].Speed 	= 3;
			mode_para_data[i].Bright 	= 4;
			mode_para_data[i].Other 	= 0;
			mode_para_data[i].ColorNum 	= 0;
			mode_para_data[i].ColorVal 	= COLOR_SELF;
			mode_para_data[i].Chksum 	= 0;
			mode_para_data[i].Reserve1 	= 0;
			for (uint8_t j = 0; j <= PARA_COLORNUM_MAX; j++)
	    	{
	    		mode_para_data[i].RcvColor[j].BufR = 0;
				mode_para_data[i].RcvColor[j].BufG = 0;
				mode_para_data[i].RcvColor[j].BufB = 0;
				mode_para_data[i].Color[j].BufR = 0;
				mode_para_data[i].Color[j].BufG = 0;
				mode_para_data[i].Color[j].BufB = 0;
			}
		}

		uint8_t i = 0;
		
		i = STEADY;
		mode_para_data[i].ColorVal = MUTICOLOR;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = SPARKLE;
		mode_para_data[i].Other = 10;
		mode_para_data[i].ColorVal = WINTER;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = RAINBOW;
		mode_para_data[i].Other = 4;

		i = FADE;
		mode_para_data[i].ColorVal = CHRISTMAS;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = SNOW;
		mode_para_data[i].Speed = 4;
		mode_para_data[i].ColorVal = WHITE;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = SNAKE;
		mode_para_data[i].Other  = 4;
		mode_para_data[i].ColorVal = INDEPENDENCE;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = TWINKLE;
		mode_para_data[i].Other  = 5;
		mode_para_data[i].ColorVal = ORANGE;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = FIREWORKS;
		mode_para_data[i].ColorVal = MUTICOLOR;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = HORIZONTAL;
		mode_para_data[i].Other  = 10;
		mode_para_data[i].ColorVal = HALLOWEEN;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = WAVES;
		mode_para_data[i].Other  = 5;
		mode_para_data[i].ColorVal = THANKSGIVING;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = UPDWN;
		mode_para_data[i].Other  = 5;
		mode_para_data[i].ColorVal = MUTICOLOR;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = GLOW;
		mode_para_data[i].ColorVal = SPRING;
		mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

		i = COLOR_RAND;
		mode_para_data[i].Other  = 10;
	}
		

	//No - Confirm if it is the first time to write the user normal flash? 
	else
	{
		if (display_data.mode_buf > CURRENT_MODE_MAX && display_data.mode_buf != CUSTOM_STEADY_1 
		&& display_data.mode_buf != CUSTOM_STEADY_2 && display_data.mode_buf != CUSTOM_STEADY_3
		&& display_data.mode_buf != CUSTOM_DYNAMIC_1)
		{
			display_data.mode_buf = STEADY;
			enable_user_normal_flash_write();
		}
		display_data.mode = display_data.mode_buf;
		display_data.init = true;

		if (timing_data.cntdwn_hour > CNTDWN_HOUR_MAX)
		{
			timing_data.cntdwn_hour = 0;
			enable_user_normal_flash_write();
		}

		for (uint8_t i = 0; i < TIMING_GRP_MAX; i++)
		{
			if (timing_data.timing_grp[i].on_hour >= 24 || timing_data.timing_grp[i].on_minute >= 60 || 
			timing_data.timing_grp[i].off_hour >= 24 || timing_data.timing_grp[i].off_minute >= 60)
			{
				timing_data.timing_grp[i].on_hour = 0;
				timing_data.timing_grp[i].on_minute = 0;
				timing_data.timing_grp[i].off_hour = 0;
				timing_data.timing_grp[i].off_minute = 0;
				setting_timing_flag((TimingEnFlag_E)i, 0);
				enable_user_normal_flash_write();
			}
		}

		

		//music data
		if (music_data.enable_flag > 1)		music_data.enable_flag = 0;
		if (music_data.mode > 1)			music_data.mode = 0;

		//vertical layer information
		if (vertical_layer.en_flag == false)
		{
			Display_Layout_None_Init();
		}
		else
		{
			uint8_t err_flag = false;

			if (vertical_layer.layer_total >= LAYER_MAX)
			{
				err_flag = true;
			}

			if (!err_flag)
			{
				for (uint8_t i = 0; i < LAYER_MAX; i++)
				{
					if (vertical_layer.head[i] >= LED_TOTAL)
					{
						err_flag = true;
						break;
					}
				}
			}

			if (!err_flag)
			{
				for (uint8_t i = 0; i < LED_TOTAL; i++)
				{
					if (vertical_layer.info[i] >= LAYER_MAX)
					{
						err_flag = true;
						break;
					}
				}
			}

			//if the layer information is wrong.
			if (err_flag)
			{
				vertical_layer.en_flag = false;
				enable_user_normal_flash_write();
			}
		}

		//triangle layer information
		if (triangle_layer.en_flag)
		{
			uint8_t err_flag = false;

			if (triangle_layer.layer_total >= LAYER_MAX)
			{
				err_flag = true;
			}

			if (!err_flag)
			{
				for (uint8_t i = 0; i < LAYER_MAX; i++)
				{
					if (triangle_layer.head[i] >= LED_TOTAL)
					{
						err_flag = true;
						break;
					}
				}
			}

			if (!err_flag)
			{
				for (uint8_t i = 0; i < LED_TOTAL; i++)
				{
					if (triangle_layer.info[i] >= LAYER_MAX)
					{
						err_flag = true;
						break;
					}
				}
			}

			//if the layer information is wrong.
			if (err_flag)
			{
				triangle_layer.en_flag = false;
				enable_user_normal_flash_write();
			}
		}

		//fan layer information
		if (fan_layer.en_flag)
		{
			uint8_t err_flag = false;

			if (fan_layer.layer_total >= LAYER_MAX)
			{
				err_flag = true;
			}

			if (!err_flag)
			{
				for (uint8_t i = 0; i < LAYER_MAX; i++)
				{
					if (fan_layer.head[i] >= LED_TOTAL)
					{
						err_flag = true;
						break;
					}
				}
			}

			if (!err_flag)
			{
				for (uint8_t i = 0; i < LED_TOTAL; i++)
				{
					if (fan_layer.info[i] >= LAYER_MAX)
					{
						err_flag = true;
						break;
					}
				}
			}

			//if the layer information is wrong.
			if (err_flag)
			{
				fan_layer.en_flag = false;
				enable_user_normal_flash_write();
			}
		}

		//mode status and mode data
		uint8_t i = 0;
		for (i = 0; i <= CURRENT_MODE_MAX; i++)
	  	{
	  		uint8_t err_flag = false;

			if (mode_para_data[i].Mode != i || mode_para_data[i].Speed > PARA_SPEED_MAX || mode_para_data[i].Bright > PARA_BRIGHT_MAX
			|| mode_para_data[i].Other > PARA_OTHER_MAX || mode_para_data[i].ColorNum > PARA_COLORNUM_MAX) 
	  		{
				err_flag = true;
				enable_user_normal_flash_write();
	  		}

	  		if (i == RAINBOW || i == COLOR_RAND || i == CARNIVAL || i == ALTERNATE)
	  		{
				mode_para_data[i].ColorNum = 0;
	  		}
	  		
	  		if (err_flag)
	  		{
				mode_para_data[i].Mode 		= i;
				mode_para_data[i].Speed 	= 3;
				mode_para_data[i].Bright 	= 4;
				mode_para_data[i].Other 	= 0;
				mode_para_data[i].ColorNum 	= 0;
				mode_para_data[i].ColorVal 	= COLOR_SELF;
				mode_para_data[i].Chksum 	= 0;
				mode_para_data[i].Reserve1 	= 0;
				for (uint8_t j = 0; j <= PARA_COLORNUM_MAX; j++)
		    	{
			    	mode_para_data[i].RcvColor[j].BufR = 0;
					mode_para_data[i].RcvColor[j].BufG = 0;
					mode_para_data[i].RcvColor[j].BufB = 0;
					mode_para_data[i].Color[j].BufR = 0;
					mode_para_data[i].Color[j].BufG = 0;
					mode_para_data[i].Color[j].BufB = 0;
				}

				if (i == STEADY)
				{
					mode_para_data[i].ColorVal = MUTICOLOR;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}


				if (i == SPARKLE)
				{
					mode_para_data[i].Other = 10;
					mode_para_data[i].ColorVal = WINTER;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == RAINBOW)
				{
					mode_para_data[i].Other = 4;
				}

				if (i == FADE)
				{
					mode_para_data[i].ColorVal = CHRISTMAS;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == SNOW)
				{
					mode_para_data[i].Speed = 4;
					mode_para_data[i].ColorVal = WHITE;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == SNAKE)
				{
					mode_para_data[i].Other  = 4;
					mode_para_data[i].ColorVal = INDEPENDENCE;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == TWINKLE)
				{
					mode_para_data[i].Other  = 5;
					mode_para_data[i].ColorVal = ORANGE;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == FIREWORKS)
				{
					mode_para_data[i].ColorVal = MUTICOLOR;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}


				if (i == HORIZONTAL)
				{
					mode_para_data[i].Other  = 10;
					mode_para_data[i].ColorVal = HALLOWEEN;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == WAVES)
				{
					mode_para_data[i].Other  = 5;
					mode_para_data[i].ColorVal = THANKSGIVING;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == UPDWN)
				{
					mode_para_data[i].Other  = 5;
					mode_para_data[i].ColorVal = MUTICOLOR;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == GLOW)
				{
					mode_para_data[i].ColorVal = SPRING;
					mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);
				}

				if (i == COLOR_RAND)
				{
					mode_para_data[i].Other  = 10;
				}
			}
		}
	}

	//get check sum
	for (uint8_t i = 0; i < MODE_MAX; i++)
	{
		mode_para_data[i].Chksum = 0;
		mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + mode_para_data[i].ColorNum * 3);
		mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;	//color val do not check sum

		if (mode_para_data[i].ColorVal == 0xff)
		{
			for (uint8_t j = 0; j < mode_para_data[i].ColorNum; j++)
			{
				mode_para_data[i].Color[j].BufR = mode_para_data[i].RcvColor[j].BufR;
				mode_para_data[i].Color[j].BufG = mode_para_data[i].RcvColor[j].BufG;
				mode_para_data[i].Color[j].BufB = mode_para_data[i].RcvColor[j].BufB;
			}
		}
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


	//clear the received index
	if (index_rcv_time < 50)
	{
		index_rcv_time++;
	}
	else	
	{
		index_rcv_pre = 0;
	}
}



/*
 * FUNCTION NAME	Key_Process
 * DESCRIPTION      process the key status
**/
void Key_Process(void)
{
	uint8_t i = 0;
	uint8_t	j = 0;
	uint8_t	k = 0;


	//short press mode key
	//change the mode
	if (KeyMode == KEY_SHORT)
	{
		KeyMode = KEY_IDLE;

		if (display_data.mode== POWER_OFF)
		{
			display_data.mode= POWER_ON;
		}
		else
		{
			display_data.mode_buf++;
			if (display_data.mode_buf > CURRENT_MODE_MAX)
			{
			display_data.mode_buf = 0;
			display_data.mode= POWER_OFF;
			}
			else
			{
			display_data.mode= display_data.mode_buf;
			}
		}

		display_data.init = true;

		mode_change_flag = true;
		mode_change_time = 0;

		//load to user buffer
		enable_user_normal_flash_write();
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

		i = display_data.mode;

		mode_change_flag = true;
		mode_change_time = 0;

		if ((i != POWER_OFF) && (i != RAINBOW) && (i != COLOR_RAND) && (i != CARNIVAL) && (i != ALTERNATE))
		{
			display_data.init = true;

			mode_para_data[i].ColorVal++;
			if (display_data.mode== SNOW)
			{
				if (mode_para_data[i].ColorVal > COLOR_VAL_MAX)    mode_para_data[i].ColorVal = 0;
			}
			else
			{
				if (mode_para_data[i].ColorVal > COLOR_VAL_MAX + THEME_VAL_MAX)    mode_para_data[i].ColorVal = 0;
			}


			mode_para_data[i].ColorNum = setting_mode_preset_color(i, mode_para_data[i].ColorVal);

			//Clear the old check sum
			mode_para_data[i].Chksum = 0;
			mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + j * 3);
			mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;

			enable_user_normal_flash_write();
		}
	}

	//long press the color key
	//change the count down timer
	else if (KeyColor == KEY_LONG)
	{
		KeyColor = KEY_IDLE;

		timing_data.cntdwn_hour++;
		if (timing_data.cntdwn_hour > 8)	timing_data.cntdwn_hour = 0;
		cntdwn_hour_setting_flag = true;
		setting_timing_flag(TIM_EN_CNTDWN, 1);
		enable_user_normal_flash_write();
	}
}


/*
 * FUNCTION NAME	Count_Down_Process
 * DESCRIPTION		Action when time count down to turned on or turned off status
 */
void Count_Down_Process(void)
{
	if (timing_status == TIMING_TURN_ON)
	{
		timing_status = TIMING_IDLE;
		
		if (display_data.mode== POWER_OFF)
		{
			display_data.mode= POWER_ON;
			display_data.init = true;
		}
	}

	else if (timing_status == TIMING_TURN_OFF)
	{
		timing_status = TIMING_IDLE;
		
		if (display_data.mode!= POWER_OFF)
		{
			display_data.mode= POWER_OFF;
			display_data.init = true;
		}
	}
}



/**
  * FunctionName  Mcu_com_process
  */
void Mcu_com_process(void)
{
	static uint8_t    	len_pre;
	static uint8_t*   	dst = (uint8_t *)&mcu_rcv_pack;

	static uint8_t		mode_pre = 0xff;
	static uint8_t		snd_index = 0x1;

	static uint8_t 		cntdwn_hour_pre = 0x0;

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
	if ((display_data.mode!= mode_pre) && (display_data.mode < MODE_MAX || display_data.mode== POWER_OFF))
	{
		mode_pre = display_data.mode;
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

	else if (timing_data.cntdwn_hour != cntdwn_hour_pre)
	{
		cntdwn_hour_pre = timing_data.cntdwn_hour;
		
		uint8_t	snd_data[4];
		snd_data[0] = MCU_PROTOCOL_VER;
		snd_data[1] = snd_index;
		snd_index++;
		if (snd_index == 255)
		{
			snd_index = 1;
		}
		snd_data[2] = MCU_TIMER_CMD;
		snd_data[3] = timing_data.cntdwn_hour;
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
					if (display_data.mode!= MUSIC_MODE  && display_data.mode < MODE_MAX)
					{
						display_data.mode= MUSIC_MODE;
						display_data.init = true;
					}

					MusicUpdateFlag = true;

					LayerStep = (mcu_rcv_pack.cmd - 0x10);
					if (LayerStep > vertical_layer.layer_total)	LayerStep = vertical_layer.layer_total;
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
					if (voice_cmd.payload[0] && display_data.mode== POWER_OFF)
					{
						display_data.mode  = POWER_ON;
						display_data.init = true;
					}
				}
			}break;

			case VOICE_OFF_NEW:
			{
				if (voice_cmd.len == 1)
				{
					if (!voice_cmd.payload[0] && display_data.mode!= POWER_OFF)
					{
						display_data.mode= POWER_OFF;
						display_data.init = true;
					}
				}
			}break;

			case VOICE_MODE_NEW:
			{
				if (voice_cmd.len == 1)
				{
					uint8_t i = voice_cmd.payload[0];		//get the mode value
					if (i > CURRENT_MODE_MAX)		break;
					display_data.mode= i;
					display_data.mode_buf = i;
					display_data.init = true;

					mode_change_flag = true;
					mode_change_time = 0;

					enable_user_normal_flash_write();
				}
			}break;

			case VOICE_COLOR_NEW:
			{
				if (voice_cmd.len != 1)	break;
				uint8_t i = voice_cmd.payload[0];	//get the color value
				if (i > COLOR_VAL_MAX + THEME_VAL_MAX)		break;

				mode_change_flag = true;
				mode_change_time = 0;

				//save the color values to mode_para_data
				uint8_t j = display_data.mode;
				uint8_t k = Color_Value_Get(i);
				mode_para_data[j].ColorVal = i;
				
				mode_para_data[j].ColorNum = setting_mode_preset_color(j, i);

				//caculate the new checksum
				mode_para_data[i].Chksum = 0;
				mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + j * 3);
				mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;

				//intialize the mode
				display_data.init = true;

				//enable the function save the data to flash
				enable_user_normal_flash_write();
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
				/*[0] = mode, [1][2][3]=reserved
				 *[4] = count down hour, [5][6][7]=reserved,
				 *[8] = timer 1 on hour, [9] = timer 1 on minute, [10] = timer 1 off hour, [11] = timer 1 off minute,  
				 *...
				 *[24] = timer 5 on hour, [25] = timer 5 on minute, [26] = timer 5 off hour, [27] = timer 5 off minute, 
				 *[28] = music enable flag,[29]=music mode value,[30]=music r,[31]=music g,[32]=music b,[33][34][35]=reserved,
				 *[36] = vertical layer flag, [37]= vertical total,[38] = triangle layer flag,[39]=triangle total, [40] = fan layer flag,
				 *[41]=fan total,[4243] = reserved, 
				 *[44] = mode 1 status, [45] = mode 2 status
				 *…
				*/
				case CHECK_ALL_STATUS_CMD:
				{
					//get the real time
					uint16_t len = app_pack.len;
					if (len != 3)		break;	//exit when the length is mismath

					if (app_pack.payload[0] >= 24 || app_pack.payload[1] >= 60 || app_pack.payload[2] >= 60)
					{
						break;
					}

					real_time.hour = app_pack.payload[0];
					real_time.minute = app_pack.payload[1];
					real_time.second = app_pack.payload[2];
					real_time.msecond = 0;

					uint8_t	*reply_status;
					uint8_t *reply_status_base;
					uint8_t	size = 44 + CURRENT_MODE_MAX + 1;

					reply_status_base = mmalloc(size);
					reply_status = reply_status_base;

					if (reply_status == 0)	break;

					*reply_status++ = display_data.mode;
					*reply_status++ = display_data.mode_buf;
					for (uint8_t i = 0; i < 2; i++)
					{
						*reply_status++ = 0;		//reserved
					}

					uint8_t *src = (uint8_t *)&timing_data.cntdwn_hour;
					for (uint8_t i = 0; i < 24; i++)
					{
						*reply_status++ = *src++;
					}

					src = (uint8_t *)&music_data.enable_flag;
					for (uint8_t i = 0; i < 8; i++)
					{
						*reply_status++ = *src++;
					}

					*reply_status++ = vertical_layer.en_flag;
					*reply_status++ = vertical_layer.layer_total;
					*reply_status++ = triangle_layer.en_flag;
					*reply_status++ = triangle_layer.layer_total;
					*reply_status++ = fan_layer.en_flag;
					*reply_status++ = fan_layer.layer_total;
					*reply_status++ = 0;
					*reply_status++ = 0;

					for(uint8_t i = 0; i <= CURRENT_MODE_MAX; i++)
					{
						*reply_status++ = mode_para_data[i].Chksum;
					}

					res_to_app(ALL_STATUS_ACK, (const uint8_t *)reply_status_base, size);

					mfree(reply_status_base);
					reply_status = 0;
					reply_status_base = 0;

				} break;

				case CHECK_MODE_STATUS_CMD:
				{
					uint8_t i = app_pack.payload[0];
					//exit if the inquire mode value is more than upper limiting value
					if (i > CURRENT_MODE_MAX)   break;

					uint16_t len = mode_para_data[i].ColorNum * 3 + PARA_PACK_HEADRE_BYTE;
					uint8_t *src = (uint8_t *)&mode_para_data[i];
					res_to_app(MODE_STATUS_ACK, (const uint8_t *)src, len);
				}break;

				//0 - turn off
				//1 - turn on
				case SET_ON_OFF_CMD:
				{
					if (app_pack.len == 1)
					{
						if (app_pack.payload[0] && display_data.mode== POWER_OFF)
						{
							display_data.mode	= POWER_ON;
							display_data.init 	= true;
							mode_change_flag = true;
							mode_change_time = 0;
						}
						else if (!app_pack.payload[0] && display_data.mode!= POWER_OFF)
						{
							display_data.mode= POWER_OFF;
							display_data.init = true;
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

					display_data.init = true;
					display_data.mode_buf = i;
					display_data.mode= i;
					mode_para_data[i].Speed = j;
					mode_para_data[i].Bright = k;
					mode_para_data[i].Other = l;

					mode_para_data[i].Chksum = 0;
					mode_para_data[i].Chksum = chksum_cal((uint8_t *)&mode_para_data[i], 8 + mode_para_data[i].ColorNum * 3);
					mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;

					//enable the function save the data to flash
					enable_user_normal_flash_write();
				}break;

				//set color
				//the maxium colors number is PARA_COLORNUM_MAX
				case SET_COLOR_CMD:
				{
					uint8_t   i = app_pack.payload[0];  //mode
					uint8_t   j = app_pack.payload[1];  //colornum
					if (i > MODE_MAX && i != MUSIC_MODE)        break;    //overflow
					if (j > PARA_COLORNUM_MAX)                  break;    //overflow
					if (i != display_data.mode)                 break;    //mismath with the current mode

					mode_change_flag = true;
					mode_change_time = 0;

					mode_para_data[i].ColorNum = j;
					for (uint8_t k = 0; k < j; k++)
					{
						if (k < j)
						{
							uint8 l = k * 3 + 2;      //RGB->R inedex
							mode_para_data[i].RcvColor[k].BufR = app_pack.payload[l];
							mode_para_data[i].RcvColor[k].BufG = app_pack.payload[l+1];
							mode_para_data[i].RcvColor[k].BufB = app_pack.payload[l+2];
							Color_Caculate(&mode_para_data[i].Color[k].BufR, &mode_para_data[i].Color[k].BufG, &mode_para_data[i].Color[k].BufB,
										mode_para_data[i].RcvColor[k].BufR, mode_para_data[i].RcvColor[k].BufG,	mode_para_data[i].RcvColor[k].BufB);
						}
						else
						{
							mode_para_data[i].RcvColor[k].BufR = 0;
							mode_para_data[i].RcvColor[k].BufG = 0;
							mode_para_data[i].RcvColor[k].BufB = 0;
							mode_para_data[i].Color[k].BufR = 0;
							mode_para_data[i].Color[k].BufG = 0;
							mode_para_data[i].Color[k].BufB = 0;
						}
					}

					mode_para_data[i].Chksum = 0;
					mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + j * 3);
					mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;

					display_data.init = true;

					//enable the function save the data to flash
					enable_user_normal_flash_write();

					//printf("=RGB=(%d, %d, %d)\n", mode_para_data[i].Color[0].BufR, mode_para_data[i].Color[0].BufG, mode_para_data[i].Color[0].BufB);
				}break;

				case SET_MODE_COLOR_CMD:
				{
					uint8_t   	i = app_pack.payload[0];  //mode
					uint8_t   	speed = app_pack.payload[1];  //
					uint8_t		bright = app_pack.payload[2];
					uint8_t		other = app_pack.payload[3];
					if (i > MODE_MAX && i != MUSIC_MODE)    break;    //overflow
					if (speed > PARA_SPEED_MAX)             break;    //overflow
					if (bright > PARA_BRIGHT_MAX)			break;
					if (other > PARA_OTHER_MAX)				break;

					mode_change_flag = true;
					mode_change_time = 0;

					uint8_t j = app_pack.payload[4];
					mode_para_data[i].ColorNum = j;
					for (uint8_t k = 0; k < j; k++)
					{
						if (k < j)
						{
							uint8 l = k * 3 + 5;      //RGB->R inedex
							mode_para_data[i].RcvColor[k].BufR = app_pack.payload[l];
							mode_para_data[i].RcvColor[k].BufG = app_pack.payload[l+1];
							mode_para_data[i].RcvColor[k].BufB = app_pack.payload[l+2];
							Color_Caculate(&mode_para_data[i].Color[k].BufR, &mode_para_data[i].Color[k].BufG, &mode_para_data[i].Color[k].BufB,
										mode_para_data[i].RcvColor[k].BufR, mode_para_data[i].RcvColor[k].BufG,	mode_para_data[i].RcvColor[k].BufB);
						}
						else
						{
							mode_para_data[i].RcvColor[k].BufR = 0;
							mode_para_data[i].RcvColor[k].BufG = 0;
							mode_para_data[i].RcvColor[k].BufB = 0;
							mode_para_data[i].Color[k].BufR = 0;
							mode_para_data[i].Color[k].BufG = 0;
							mode_para_data[i].Color[k].BufB = 0;
						}
					}

					display_data.init = true;
					display_data.mode_buf = i;
					display_data.mode= i;
					mode_para_data[i].Speed = speed;
					mode_para_data[i].Bright = bright;
					mode_para_data[i].Other = other;

					mode_para_data[i].Chksum = 0;
					mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + j * 3);
					mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;

					//enable the function save the data to flash
					enable_user_normal_flash_write();
				}break;

				case SET_CNTDWN_HOUR_CMD:
				{
					uint16_t len = app_pack.len;
					if (len != 2)	break;

					if (app_pack.payload[1] > 0 && app_pack.payload[1] <= 12)
					{
						timing_data.cntdwn_hour = app_pack.payload[1];
						cntdwn_hour_setting_flag = true;
						setting_timing_flag(TIM_EN_CNTDWN, app_pack.payload[0]);

						//reply
						res_to_app(CNTDWN_HOUR_STATUS_ACK, (const uint8_t *)&app_pack.payload[0], 2);

						//enable the function save the data to flash
						enable_user_normal_flash_write();
					}
				}break;

				case SET_REALTIME_CMD:
				{
					uint16_t len = app_pack.len;
					if (len != 3)	break;

					if (app_pack.payload[0] >= 24 || app_pack.payload[1] >= 60 || app_pack.payload[2] >= 60)
					{
						break;
					}
					else
					{
						real_time.hour = app_pack.payload[0];
						real_time.minute = app_pack.payload[1];
						real_time.second = app_pack.payload[2];
						real_time.msecond = 0;
						//reply
						res_to_app(REALTIME_STATUS_ACK, (const uint8_t *)&app_pack.payload[0], 3);
					}
				}break;

				case SET_CNTDWN_TIME_CMD:
				{
					uint16_t len = app_pack.len;
					if (len != 6)	break;		//mismatch

					uint8_t grp = app_pack.payload[1];
					if (grp >= TIMING_GRP_MAX)		break;
					
					uint8_t on_hour = app_pack.payload[2];
					uint8_t on_min = app_pack.payload[3];
					uint8_t off_hour = app_pack.payload[4];
					uint8_t off_min = app_pack.payload[5];
					
					//wrong time - instead the data by 0xff in order to reply to APP
					if (on_hour >= 24 || on_min >= 60 || off_hour >= 24 || off_min >= 60)
					{
						break;
					}
					//correct time
					else
					{
						timing_data.timing_grp[grp].on_hour = on_hour;
						timing_data.timing_grp[grp].on_minute = on_min;
						timing_data.timing_grp[grp].off_hour = off_hour;
						timing_data.timing_grp[grp].off_minute = off_min;

						setting_timing_flag((TimingEnFlag_E)grp, app_pack.payload[0]);
						
						//enable the function save the data to flash
						enable_user_normal_flash_write();
					}

					//reply
					res_to_app(CNTDWN_TIME_STATUS_ACK, (const uint8_t *)&app_pack.payload[0], 6);
				}break;

	//--------------------------------------------------------------------------------
	//                        LAYOUT
	//--------------------------------------------------------------------------------
	
				case LAYOUT_ENTER_CMD:
				{
					uint8_t reply = 0;
					
					if (app_pack.len == 1)
					{
						if (app_pack.payload[0] == 0 && (display_data.mode== LAYOUT_ENTER || display_data.mode== LAYOUT_TEST))
						{
							display_data.mode= LAYOUT_CANCEL;
							display_data.init = true;
							reply = 1;
						}

						else if (display_data.mode!= LAYOUT_ENTER)
						{
							display_data.mode= LAYOUT_ENTER;
							display_data.init = true;
							reply = 1;
						}
					}

					res_to_app(LAYOUT_ENTER_ACK, (const uint8_t *)&reply, 1);
				}break;

				case LAYOUT_TEST_CMD:
				{

					#if 0
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
						&& (display_data.mode== LAYOUT_ENTER || display_data.mode== LAYOUT_TEST))
						{
							LayerTemp[LayerTest].Head = head;
							LayerTemp[LayerTest].Tail = tail;
							display_data.mode= LAYOUT_TEST;
							display_data.init = true;
							reply = 1;
						}
					}

					res_to_app(LAYOUT_TEST_ACK, (const uint8_t *)&reply, 1);
					#endif
				}break;

				case LAYOUT_SAVE_CMD:
				{
					#if 0
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
								display_data.mode= LAYOUT_SAVE;
								display_data.init = true;
								//display_data.LayoutNum = app_pack.payload[1];
								reply = 1;
							}
						}
					}

					//exit the layout mode if error occur
					if (reply == 0)
					{
						display_data.mode= LAYOUT_CANCEL;
						display_data.init = true;
					}

					//save the layer information to user flash
					else
					{
						#if USER_DATA_EN==1
						sys_eeprom_write(LAYOUT_OFFSET, (uint8_t *)&display_data.LayoutNum, LAYOUT_LEN);
						uint8_t len = LayerTest * 4;
						sys_eeprom_write(LAYER_NUM_OFFSET, (uint8_t *)&len, LAYER_NUM_LEN);
						sys_eeprom_write(LAYER_OFFSET, (uint8_t *)&LayerTemp, len);
						#endif
					}

					res_to_app(LAYOUT_SAVE_ACK, (const uint8_t *)&reply, 1);
					#endif
				}break;


				case LAYOUT_INFO_CMD:
				{
					uint16_t len = app_pack.len;
					if (len == 303)
					{
						Layer_T *dst = 0;
						switch (app_pack.payload[0])
						{
							//vertical
							case 0:
							{
								dst = &vertical_layer;
							}break;

							//triangle
							case 1:
							{
								dst = &triangle_layer;
							}break;

							//fan
							case 3:
							{
								dst = &fan_layer;
							}break;

							default:
							{

							}break;
						}

						uint8_t total = app_pack.payload[1];
						if (total < LAYER_MAX)
						{
							dst->en_flag = true;
							dst->layer_total = total;


							for (uint8_t i = 0; i < LED_TOTAL; i++)
							{
								dst->info[i] = app_pack.payload[i+2];
							}

							for (uint8_t i = 0; i < total; i++)
							{
								dst->head[i] = 0xffff;
								for (uint8_t j = 0; j < LED_TOTAL; j++)
								{
									if (dst->head[i] == 0xffff && dst->info[j] == i)
									{
										dst->head[i] = j;
										break;
									}
								}
							}

							enable_user_normal_flash_write();

							uint8_t *src = &app_pack.payload[0];
							res_to_app(LAYOUT_INFOR_ACK, (const uint8_t *)src, 2);
						}
						//the layer total is wrong
						else
						{
							
						}
					}
					//The data length is wrong
					else
					{
			
					}
				}break;


				case LAYOUT_CUSTOM_STEADY_CMD:
				{
					switch (app_pack.payload[0])
					{
						case 0:
						{
							if (app_pack.len == 2)
							{
								display_data.init = true;
								if (app_pack.payload[1] == 0)			display_data.mode_buf = CUSTOM_STEADY_1;
								else if (app_pack.payload[1] == 1)		display_data.mode_buf = CUSTOM_STEADY_2;
								else if (app_pack.payload[1] == 2)		display_data.mode_buf = CUSTOM_STEADY_3;
								else 									display_data.init = false;	

								if (display_data.init)		
								{
									enable_user_normal_flash_write();

									uint8_t *src = &app_pack.payload[0];
									res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 2);
								}
							}
						}break;

						case 1:
						{
							display_data.mode = LAYOUT_PHOTO_CTRL;
							if (app_pack.len == 906)
							{
								for (uint16_t i = 0; i < LED_TOTAL; i++)
								{
									uint16_t j = i * 3 + 3;
									LedData[i].DutyR = GAMMA_TABLE[app_pack.payload[j]];
									LedData[i].DutyG = GAMMA_TABLE[app_pack.payload[j+1]];
									LedData[i].DutyB = GAMMA_TABLE[app_pack.payload[j+2]];
								}
								uint8_t *src = &app_pack.payload[0];
								res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 3);
							}
						}break;

						case 2:
						{
							display_data.mode = display_data.mode_buf;
							display_data.init = true;
							uint8_t *src = &app_pack.payload[0];
								res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 2);
						}break;

						case 3:
						{
							if (app_pack.len == 2)
							{
								display_data.init = true;
								if (app_pack.payload[1] == 0)			display_data.mode_buf = CUSTOM_STEADY_1;
								else if (app_pack.payload[1] == 1)		display_data.mode_buf = CUSTOM_STEADY_2;
								else if (app_pack.payload[1] == 2)		display_data.mode_buf = CUSTOM_STEADY_3;
								else 									display_data.init = false;	

								if (display_data.init)
								{
									write_custom_steady_flash(app_pack.payload[1], (uint8_t *)LedData);
									uint8_t *src = &app_pack.payload[0];
									res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 2);
								}
							}
						}break;
					}
				}break;

				case LAYOUT_CUSTOM_DYNAMIC_CMD:
				{
					switch (app_pack.payload[0])
					{
						case 0:
						{
							if (app_pack.len == 2)
							{
								display_data.init = true;
								if (app_pack.payload[1] == 0)			display_data.mode_buf = CUSTOM_DYNAMIC_1;
								else 									display_data.init = false;	

								if (display_data.init)		
								{
									enable_user_normal_flash_write();

									uint8_t *src = &app_pack.payload[0];
									res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 2);
								}
							}
						}break;

						case 1:
						{
							
							if (app_pack.len == 906)
							{
								uint8_t time = app_pack.payload[2];
								if (display_data.mode != CUSTOM_DYNAMIC_1)
								{
									display_data.mode = CUSTOM_DYNAMIC_1;
								}

								if (time == 0)
								{
									for (uint8_t i = 0; i < DYNAMIC_MAX_TIME; i++)
									{
										DynamicTimeFlag[i] = 0;
									}
								}

								DynamicTimeFlag[time] = true;

								for (uint16_t i = 0; i < LED_TOTAL; i++)
								{
									uint16_t j = i * 3 + 3;
									LedData[i].DutyR = GAMMA_TABLE[app_pack.payload[j]];
									LedData[i].DutyG = GAMMA_TABLE[app_pack.payload[j+1]];
									LedData[i].DutyB = GAMMA_TABLE[app_pack.payload[j+2]];
								}


								write_custom_dynamic_flash(time, (uint8_t *)LedData);
								uint8_t *src = &app_pack.payload[0];
								res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 3);
							}
						}break;

						case 2:
						{
							display_data.mode = display_data.mode_buf;
							display_data.init = true;
							uint8_t *src = &app_pack.payload[0];
							res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 2);
						}break;

						case 3:
						{
							if (app_pack.len == 2)
							{
								display_data.init = true;
								if (app_pack.payload[1] == 0)			display_data.mode_buf = CUSTOM_DYNAMIC_1;
								else 									display_data.init = false;	

								if (display_data.init)
								{
									enable_user_normal_flash_write();
									uint8_t *src = &app_pack.payload[0];
									res_to_app(LAYOUT_CUSTOM_STEADY_ACK, (const uint8_t *)src, 2);
								}
							}
						}break;
					}
				}break;


				

				case LAYOUT_SEC_CTRL:
				{
					uint8_t		reply = 0;
					uint16_t	cnt = app_pack.len / 7;
					display_data.mode= LAYOUT_PHOTO_CTRL;

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

					display_data.mode= LAYOUT_PHOTO_CTRL;

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
					display_data.mode= LAYOUT_PHOTO_CTRL;

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

					display_data.mode= LAYOUT_PHOTO_CTRL;

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

					display_data.mode= LAYOUT_PHOTO_CTRL;

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
							if (app_pack.payload[0] && display_data.mode== POWER_OFF)
							{
								display_data.mode  = POWER_ON;
								display_data.init = true;
							}
							else if (!app_pack.payload[0] && display_data.mode!= POWER_OFF)
							{
								display_data.mode= POWER_OFF;
								display_data.init = true;
							}
						}
					}break;

				case VOICE_SET_MODE_CMD:
					{
						if (app_pack.len == 1)
						{
							uint8_t i = app_pack.payload[0];		//get the mode value
							if (i > MODE_MAX)		break;
							display_data.mode= i;
							display_data.mode_buf = i;
							display_data.init = true;

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

						//save the color values to mode_para_data
						uint8_t j = display_data.mode;
						uint8_t k = Color_Value_Get(i);
						mode_para_data[j].ColorVal = i;
						mode_para_data[j].ColorNum = k;
						for (i = 0; i < k; i++)
						{
							mode_para_data[j].Color[i].BufR = ColorData[i][R];
							mode_para_data[j].Color[i].BufG = ColorData[i][G];
							mode_para_data[j].Color[i].BufB = ColorData[i][B];
						}

						//caculate the new checksum
						mode_para_data[i].Chksum = 0;
						mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + j * 3);

						//intialize the mode
						display_data.init = true;
					}break;

				case VOICE_SET_THEME_CMD:
					{
						if (app_pack.len != 1)	break;
						uint8_t i = app_pack.payload[0];	//get the theme value
						if (i > THEME_VAL_MAX)		break;

						mode_change_flag = true;
						mode_change_time = 0;

						//save the color values to mode_para_data
						i = i + COLOR_VAL_MAX + 1;		//0x0f + 1
						uint8_t j = display_data.mode;
						uint8_t k = Color_Value_Get(i);
						mode_para_data[j].ColorVal = i;
						mode_para_data[j].ColorNum = k;
						for (i = 0; i < k; i++)
						{
							mode_para_data[j].Color[i].BufR = ColorData[i][R];
							mode_para_data[j].Color[i].BufG = ColorData[i][G];
							mode_para_data[j].Color[i].BufB = ColorData[i][B];
						}

						//caculate the new checksum
						mode_para_data[i].Chksum = 0;
						mode_para_data[i].Chksum = chksum_cal((const uint8_t *)&mode_para_data[i], 8 + j * 3);
						mode_para_data[i].Chksum ^= mode_para_data[i].ColorVal;

						//initialize the mode
						display_data.init = true;
					}break;

				case VOICE_SET_CNTDWN_HOUR_CMD:
					{

					}break;

				case VOICE_SET_CNTDWN_TIME_CMD:
					{

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

	if (index_snd_pre == 255) index_snd_pre = 1;
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


/**
  * FunctionName setting_mode_preset_color
  * Input
  *       mode
  *		  color
  * Output
  *			the color number
  * Brief
  *		  To set the mode's color buffer 
  */
uint8_t	setting_mode_preset_color(uint8_t mode, uint8_t color)
{
	uint8_t j = Color_Value_Get(color);

	for (uint8_t k = 0; k < PARA_COLORNUM_MAX+1; k++)
	{
		if (k < j)
		{
			mode_para_data[mode].RcvColor[k].BufR = ColorData[k][R];
			mode_para_data[mode].RcvColor[k].BufG = ColorData[k][G];
			mode_para_data[mode].RcvColor[k].BufB = ColorData[k][B];
			mode_para_data[mode].Color[k].BufR = ColorData[k][R];
			mode_para_data[mode].Color[k].BufG = ColorData[k][G];
			mode_para_data[mode].Color[k].BufB = ColorData[k][B];
		}
		else
		{
			mode_para_data[mode].RcvColor[k].BufR = 0;
			mode_para_data[mode].RcvColor[k].BufG = 0;
			mode_para_data[mode].RcvColor[k].BufB = 0;
			mode_para_data[mode].Color[k].BufR = 0;
			mode_para_data[mode].Color[k].BufG = 0;
			mode_para_data[mode].Color[k].BufB = 0;
		}
	}
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




















