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


//typedef
typedef struct LAYER_BRIEF
{
	uint8_t	vertical_flag;
	uint8_t vertical_total;
	uint8_t triangle_flag;
	uint8_t triangle_total;
	uint8_t fan_flag;
	uint8_t fan_total;
	uint8_t reserved1;
	uint8_t reserved2;
}LayerBrief_T;

typedef struct 
{
	uint16_t	head;
	uint16_t	tail;
}LayerData_T;


//exported paramters
extern LayerBrief_T		layer_brief;
extern LayerData_T		vertical_layer[];
extern LayerData_T		triangle_layer[];
extern LayerData_T		fan_layer[];


#endif
