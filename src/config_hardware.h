/*
 * port_esp.c
 * description: esp socket
 *  Created on: 2018-6-9
 *      Author: skylli
 * Time list:
 */
#ifndef _CONFIG_HARDWARE_H
#define _CONFIG_HARDWARE_H



#ifdef __cplusplus
extern "C"{
#endif

#include "../config/product_config.h"
// product config

/* board define ***/
#define BOARD_ESP_8266
// led
#define LED_WIFI_CONN_PIN  16//16//5
// factory rest key.
#define REST_PIN	5// 5
#define FAC_TEST_PIN	12
#define LED         2
//#define RESET_TEST    //to open led
#define FAC_TEST
#define SOFTAP_SSID_	SFAP_SSID_
#define EEPROM_CONF_SIZE_MX (2*512)
// todo 
#define EEPROM_CONF_ADDRESS (0)

#define RESTADDR  (400)
#define SMARTADDR  (350)
#define AUTOADDR  (300)
#ifdef __cplusplus
}
#endif


#endif // _CONFIG_HARDWARE_H

