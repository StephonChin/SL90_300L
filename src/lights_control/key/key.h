#ifndef _KEY_H_
#define _KEY_H_

//include files
#include <Arduino.h>
#include <stdint.h>

//define
#define     MODE_PIN        5  
#define     COLOR_PIN       4

//typedef
typedef enum _KEY_TYPE{
  KEY_IDLE,
  KEY_SHORT,
  KEY_LONG
}_TypeKey;

//exported functions
void Key_Scan(void);

//exported parameters
extern _TypeKey   KeyMode;
extern _TypeKey   KeyColor;


#endif

