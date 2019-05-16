/*
 * port_esp.c
 * description: esp socket
 *  Created on: 2018-6-9
 *      Author: skylli
 * Time list:
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config_hardware.h"

# if 0
#include "Arduino.h"
#include "osapi.h"
#include "ets_sys.h"
#include "lwip/inet.h"

#include <stdbool.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#endif

#ifndef _FUNCTION_H
#define _FUNCTION_H

//#define FAC_TEST
#ifdef  FAC_TEST
#define TEST_MODE  (0)
#define TEST_SSID  "nielink_factory_test_9527"
#define TEST_PWD   "##__9527"
#define TEST_IO    16
#define IO_VAL     1
#define STR1(R)  #R
#define STR2(A,R)  A STR1(R)
#define IO_STR     STR2(STR2("io-",TEST_IO), IO_VAL) 
#endif
#ifdef __cplusplus
extern "C"{
#endif
#include "m2mnet/include/m2m_type.h"
#include "m2mnet/include/m2m.h"
#include "app_handle.h"

typedef struct SYS_HOST_INFO{
	u16 port;
	u16 len;
	M2M_id_T s_id;
	u8 cname[0];
}SYS_Host_info_t;
typedef enum LESP_WIFI_MODE_T{
	WIFI_MODE_RST_SMT = 0,
	WIFI_MODE_RST_AP,
	WIFI_MODE_STA_SMT,
	WIFI_MODE_STA_PW,
	WIFI_MODE_MAX

}LESP_WIFI_MODE_T;

typedef struct EEPROM_CONF_T{
	u8 ver_mak;	// version and mark that identify if the eeprom have been init. 
	u8 wifi_mod;
	u8 reset_cnt;	// 重启计数器.
	u8 keylen;
	u8 p_key[32];
	SYS_Host_info_t host;

}EEPROM_conf_T;

u32 esp8266_secretKey_read(u8 *p_key,u16 *p_keylen);
u32 esp8266_secretKey_write(u8 *p_key,u16 p_keylen);
void sys_setup(void);
void sys_factory_reset(void);
void sys_sta_smartconfig(void);
void local_ip_save(void);
int sys_ssid_pw_reset(LM2M_router_conf *p_router);
SYS_status sys_connect_status_hanle();
int sys_cmd_handle(size_t net,u8 cmd,u8*p_data,int recv_len,M2M_packet_T **pp_ack_data);
SYS_Host_info_t *sys_host_alloc(M2M_id_T *p_id);
void system_loop(void);
BOOL sys_broadcast_enable(void);
BOOL sys_in_ap_mode(void);
int sys_eeprom_read(int address, u8 *p_buf, int len);
int sys_eeprom_write(int address, u8 *p_buf, int len);
extern u8 testbit;

#ifdef __cplusplus
}
#endif


#endif // _FUNCTION_H

