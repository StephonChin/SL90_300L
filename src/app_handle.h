/*********************************************************
** 功能测试： token, key 设置,广播测试.
*********************************************************/
#ifndef  _APP_HANDLE_H
#define _APP_HANDLE_H

#ifdef __cplusplus
extern "C"{
#endif

typedef enum SYS_STATUS{

	SYS_CONFIGING_NONE=0,
    SYS_CONFIGING_STA=1,
    SYS_LOST_CONNECT,   
    SYS_OTAING, 
    SYS_ONLINE,
    SYS_CONFIGING_AP, 
	SYS_MAX
}SYS_status;

int app_cmd_handle(u8 cmd,u8*p_data,int recv_len,M2M_packet_T **pp_ack_data);

#ifdef __cplusplus
}
#endif

#include "esm2m.h"
void app_setup(void);
void app_loop(void);
void app_system_status_loop(SYS_status status);

#endif //_APP_HANDLE_H

