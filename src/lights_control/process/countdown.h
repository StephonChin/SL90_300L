/*
 * FILENAME		countdown.h
 * DESCRIPTION
 *				count down timer
 *				get the real time from APP after connected
 *				there are two mode to count down
 *					1. count down hour to turn off
 *					2. hour and minute to turn off
 */
#ifndef _COUNTDOWN_H_
#define _COUNTDOWN_H_

//include files
#include <stdint.h>
#include <stdbool.h>

//define
#define		CNTDWN_HOUR_MAX			12


//typdef 
typedef enum
{
	TIMING_IDLE,
	TIMING_TURN_ON,
	TIMING_TURN_OFF
}TimingStatus_E;



typedef struct
{
	uint8_t	hour;
	uint8_t minute;
	uint8_t second;
	uint8_t msecond;
}RealTime_T;


#define		TIMING_GRP_MAX		5
typedef struct
{
	uint8_t cntdwn_hour;
	uint8_t en_flag;
	uint8_t reserved2;
	uint8_t reserved3;
	struct TIMING
	{
		uint8_t on_hour;
		uint8_t on_minute;
		uint8_t off_hour;
		uint8_t off_minute;
	}timing_grp[TIMING_GRP_MAX];
}TimingData_T;

typedef enum
{
	TIM_EN_GRP_0 = 0,
	TIM_EN_GRP_1,
	TIM_EN_GRP_2,
	TIM_EN_GRP_3,
	TIM_EN_GRP_4,
	TIM_EN_CNTDWN = 7
}TimingEnFlag_E;



//global function
void timing_task(void);
void setting_timing_flag(TimingEnFlag_E grp, uint8_t en);

uint8_t	getting_timing_flag(TimingEnFlag_E grp);


//exported parameters
extern bool				cntdwn_hour_setting_flag;
extern RealTime_T		real_time;
extern TimingData_T		timing_data;
extern TimingStatus_E	timing_status;




#endif
