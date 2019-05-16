/*********************************************************
** 功能测试： token, key 设置,广播测试.
*********************************************************/
#ifndef  _APP_BOARD_H
#define _APP_BOARD_H

#ifdef __cplusplus
extern "C"{
#endif



#ifdef __cplusplus
}
#endif

#include "esm2m.h"
#include "app_handle.h"
#include "config_hardware.h"

#define TEST_ROUTER_SSID  "test_router" 
#define TEST_ROUTER_PWD   "test_#123"
void board_led_status(SYS_status status);
int board_fact_cmd_handle(M2M_packet_T **pp_ack_data);
void board_setup(void);
void board_loop(void);

int test_connect_router(void);
#endif //_APP_BOARD_H

