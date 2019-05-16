/*
 * FILENAME		countdown.c
 * DESCRIPTION
 *				count down timer
 *				get the real time from APP after connected
 *				there are two mode to count down
 *					1. count down hour to turn off
 *					2. hour and minute to turn off
 */
#include "countdown.h"
#include <stdio.h>


//global parameters
cntdwn_T	cntdwn_data;

/*
 * FUNCTION NAME		Timer_Count_Down
 */
void Timer_Count_Down(void)
{
	#if 1
	
	if (cntdwn_data.cntdwn_hour == 0)
	{
		cntdwn_data.real_msec = 0;
		cntdwn_data.real_sec = 0;
		cntdwn_data.real_hour = 0;
		cntdwn_data.real_min = 0;
		return;
	}

	if (cntdwn_data.init)
	{
		cntdwn_data.init = false;
		cntdwn_data.real_msec = 0;
		cntdwn_data.real_sec = 0;
		cntdwn_data.real_hour = 0;
		cntdwn_data.real_min = 0;
	}

	//real time
	cntdwn_data.real_sec++;
	
	if (cntdwn_data.real_sec >= 60)//10
	{
		cntdwn_data.real_sec = 0;

		//uint32_t time_real = millis();
		//printf("10 second: %d\n",time_real);
		
		cntdwn_data.real_min++;
		if (cntdwn_data.real_min >= 60)//6
		{
			cntdwn_data.real_min = 0;
			cntdwn_data.real_hour++;
			if (cntdwn_data.real_hour >= 24)//12
			{
				cntdwn_data.real_hour = 0;
			}
		}
	}

	if (cntdwn_data.real_hour == cntdwn_data.cntdwn_hour && cntdwn_data.real_min == 0
	&& cntdwn_data.real_sec == 0 && cntdwn_data.real_msec == 0)
	{
		cntdwn_data.status = CNTDWN_TURN_OFF;
		//uint32_t time_real = millis();
		//printf("off: %d\n",time_real);
	}

	if (cntdwn_data.real_hour == 0 && cntdwn_data.real_min == 0
	&& cntdwn_data.real_sec == 0 && cntdwn_data.real_msec == 0)
	{
		cntdwn_data.status = CNTDWN_TURN_ON;
		//uint32_t time_real = millis();
		//printf("on: %d\n",time_real);
	}

	#else

	//real time
	cntdwn_data.real_msec++;
	if (cntdwn_data.real_msec >= 100)
	{
		cntdwn_data.real_msec = 0;
		cntdwn_data.real_sec++;
		
		if (cntdwn_data.real_sec >= 60)
		{
			cntdwn_data.real_sec = 0;
			cntdwn_data.real_min++;
			if (cntdwn_data.real_min >= 60)
			{
				cntdwn_data.real_min = 0;
				cntdwn_data.real_hour++;
				if (cntdwn_data.real_hour >= 24)
				{
					cntdwn_data.real_hour = 0;
				}
			}
		}
	}

	if (cntdwn_data.off_hour == cntdwn_data.real_hour && cntdwn_data.off_min == cntdwn_data.real_min
	&& cntdwn_data.real_sec == 0 && cntdwn_data.real_msec == 0)
	{
		cntdwn_data.status = CNTDWN_TURN_OFF;
	}

	if (cntdwn_data.on_hour == cntdwn_data.real_hour && cntdwn_data.on_min == cntdwn_data.real_min
	&& cntdwn_data.real_sec == 0 && cntdwn_data.real_msec == 0)
	{
		cntdwn_data.status = CNTDWN_TURN_ON;
	}
	#endif
}

