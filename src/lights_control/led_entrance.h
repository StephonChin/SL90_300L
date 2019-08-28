/****************************************************************** 
  *
  * FileName    led_entrance.h
  * Date        26 NOV 2018
  * Author      Desheng.Chin
  * Brief       the entrance file for led
  *
*******************************************************************/
#ifndef _LED_ENTRANCE_H_
#define _LED_ENTRANCE_H_


//include files
#include <Arduino.h>      
#include "user_interface.h"   /* os_timer */
#include <time.h>

extern "C" {
  #include "./leddrv/ws2812_i2s.h"
  #include "./display/display.h"
  #include "./process/data_process.h"
  #include "./key/key.h"
  #include "./com/mcu_com.h"
  #include "./process/countdown.h"
  #include "./flash/user_flash.h"
}



//exported function
void Led_Entrance(void);

//file functions
/**
  * FunctionNamw  Led_program_config
  * Brief         the configuration for led needs
  *               ==> change the print port to uart0
  *               ==> intialize the timer to 80ms period
  *               ==> initialize the i2s,uart,pio
  */
void Led_Program_Config(void);




#endif
