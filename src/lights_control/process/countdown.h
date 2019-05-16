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
#define CNTDWN_IDLE			0
#define	CNTDWN_TURN_ON		1
#define	CNTDWN_TURN_OFF		2

//typedef
typedef struct
{
	uint8_t status;
	uint8_t pre_hour;
	uint8_t init;
	uint8_t real_hour;
	uint8_t real_min;
	uint8_t real_sec;
	uint8_t real_msec;
	uint8_t	cntdwn_hour;
	uint8_t	on_hour;
	uint8_t on_min;
	uint8_t off_hour;
	uint8_t off_min;
}cntdwn_T;


//global function
void Timer_Count_Down(void);



#endif
