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
bool			cntdwn_hour_setting_flag;
RealTime_T		real_time;
RealTime_T		cntdwn_time;
TimingData_T	timing_data;
TimingStatus_E	timing_status;


/*
 * Timer_Count_Down
 * Only one(count down or timing time) effectives
 * When setting the count down hour from 1 to 12,the real time will stop working
 * When setting timing on and off hour and minute, the count down hour will be setting to 0xff
 */
void timing_task(void)
{
	//real time
	real_time.second++;
	if (real_time.second >= 60)
	{
		real_time.second = 0;

		real_time.minute++;
		if (real_time.minute >= 60)
		{
			real_time.minute = 0;
			real_time.hour++;
			if (real_time.hour >= 24)
			{
				real_time.hour = 0;
			}
		}
	}


	if (getting_timing_flag(TIM_EN_CNTDWN) && (timing_data.cntdwn_hour > 0))
	{
		cntdwn_time.second++;
		if (cntdwn_time.second >= 60)
		{
			cntdwn_time.second = 0;

			cntdwn_time.minute++;
			if (cntdwn_time.minute >= 60)
			{
				cntdwn_time.minute = 0;
				cntdwn_time.hour++;
				if (cntdwn_time.hour >= 24)
				{
					cntdwn_time.hour = 0;
				}
			}
		}
		if (cntdwn_hour_setting_flag)
		{
			cntdwn_hour_setting_flag = false;
			cntdwn_time.msecond	= 0;
			cntdwn_time.second	= 0;
			cntdwn_time.hour	= 0;
			cntdwn_time.minute	= 0;
		}

	
		//turn off
		if (cntdwn_time.hour == timing_data.cntdwn_hour && cntdwn_time.minute == 0
		&& cntdwn_time.second == 0 && cntdwn_time.msecond == 0)
		{
			timing_status = TIMING_TURN_OFF;
		}

		//turn on
		if (cntdwn_time.hour == 0 && cntdwn_time.minute == 0
		&& cntdwn_time.second == 0 && cntdwn_time.msecond == 0)
		{
			timing_status = TIMING_TURN_ON;
		}
	}


	//timing on and off
	for (uint8_t i = 0; i < TIMING_GRP_MAX; i++)
	{

		if (getting_timing_flag((TimingEnFlag_E)i))
		{
			//turn off
			if (real_time.hour == timing_data.timing_grp[i].off_hour && real_time.minute == 
			timing_data.timing_grp[i].off_minute && real_time.second == 0)
			{
				timing_status = TIMING_TURN_OFF;
			}

			//turn on
			if (real_time.hour == timing_data.timing_grp[i].on_hour && real_time.minute == 
			timing_data.timing_grp[i].on_minute  && real_time.second == 0)
			{
				timing_status = TIMING_TURN_ON;
			}
		}
	}
}


void setting_timing_flag(TimingEnFlag_E grp, uint8_t en)
{
	if (en)
	{
		timing_data.en_flag |= (0x1 << (uint8_t)grp);
	}
	else
	{
		timing_data.en_flag &= ~(0x1 << (uint8_t)grp);
	}
}

uint8_t	getting_timing_flag(TimingEnFlag_E grp)
{
	if ((timing_data.en_flag & (0x1 << (uint8_t)grp)) == 0)
	{
		return 0;
	}
	return 1;
}


