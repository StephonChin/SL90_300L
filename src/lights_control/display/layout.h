/*************************************************************
  *
  * FileName    layout.c
  * Brief       lay out information
  *
**************************************************************/
#ifndef _LAYEROUT_H_
#define _LAYEROUT_H_

//include standard libraries
#include <stdint.h>

#define     LAYER_MAX             	50
#define		STR_LAYER_MAX			20
#define		STR_LAYER_SEC			15

#define		INFO_STRUCT_TOTAL		404/* 1+1+100+301+1 */

//typedef
typedef struct LAYER_INFO
{
	uint8_t		en_flag;
	uint8_t 	layer_total;
	uint16_t 	head[LAYER_MAX];
	uint8_t		info[LED_TOTAL+1];
}Layer_T;


void Display_Layout_None_Init(void);


//exported paramters
extern Layer_T		vertical_layer;
extern Layer_T		triangle_layer;
extern Layer_T		fan_layer;


#endif
