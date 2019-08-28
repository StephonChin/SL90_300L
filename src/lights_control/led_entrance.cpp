/*****************************************************************
  *
  * FunctionName  led_entrance.c
  * Date          26 NOV 2018
  * Author        Desheng.Chin
  * Brief         Process the program code of led part
  *
*******************************************************************/
#include "led_entrance.h"
#include <EEPROM.h>


#define EEPROM_CONF_SIZE		2048
#define DIFF(a,b) ((a>b)?(a-b):(b-a))



//file parameters
os_timer_t    MyTimer;
bool          TimerUpdateFlag;



/**
  * FunctionName    Timer_Call_Back
  */
void Timer_Call_Back(void *pArg)
{
  TimerUpdateFlag = true;
}


/**
  * FunctionNamw  Led_program_config
  * Brief         the configuration for led needs
  *               ==> change the print port to uart0
  *               ==> intialize the timer to 80ms period
  *               ==> initialize the i2s,uart,pio
  */
void Led_Program_Config(void)
{
  //Timer init
  os_timer_setfn(&MyTimer, Timer_Call_Back, NULL);
  os_timer_arm(&MyTimer, 1000, true); // 1000 milliseconds

  Uart_Init();

  EEPROM.begin(EEPROM_CONF_SIZE);

  //Led dma init
  Ws2812_Init();

  //Display data init
  User_Data_Init();

}



/**
  * FunctionName    Led_entrance
  * Date            26 NOV. 2018
  * Author          DS.Chin
  * Input           None
  * Output          None
  * Brief           the entrace of led code
  */
void Led_Entrance(void)
{
	#if 1

	static uint32_t 	time_10ms_pre = millis();
	static uint32_t		time_50ms_pre = time_10ms_pre;

	uint32_t time_now = millis();
	if (DIFF(time_now, time_10ms_pre) >= 10)
	{
		time_10ms_pre = time_now;
		
		Key_Scan();
		Data_Process();
		Ws2812_Show();
	}

	if (DIFF(time_now, time_50ms_pre) >= 50)
	{
		time_50ms_pre = time_now;
		Display_Control();

		write_user_normal_flash();
	}

	
	if (TimerUpdateFlag == true)
	{
		TimerUpdateFlag = false;
		timing_task();
	}

	#else

	static uint8_t  ModeStep;
	if (TimerUpdateFlag == true){
		TimerUpdateFlag = false;

		Key_Scan();

		Data_Process();
		
		Timer_Count_Down();

		ModeStep++;
		if (ModeStep >= 5){
			ModeStep = 0;
			Display_Control();
		}

		Ws2812_Show();
	}
	#endif
}









