/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "esm2m/esm2m_handle.h"
#include "esm2m/system.h"
#include "app_handle.h"
#include <EEPROM.h>

void setup()
{
	app_setup();
	delay(200);
	sys_setup();
}


void loop()
{
	system_loop();
	if(testbit==0)
		app_loop();
	else
	{
		delay(100);
	}
	
	// todo 
	yield();
}
