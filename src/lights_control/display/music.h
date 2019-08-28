/**********************************************************
	* FILE NAME		music.h
	* DATE			21 JAN 2019
	* AUTOR			Jacky.Chin
	* BRIEF			Music display function
***********************************************************/
#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <stdint.h>


typedef struct
{
	uint8_t 	enable_flag;
	uint8_t 	mode;
	uint8_t 	color_r;
	uint8_t 	color_g;
	uint8_t 	color_b;
	uint8_t 	reserved[3];
}MusicData_T;


extern MusicData_T	music_data;



#endif






