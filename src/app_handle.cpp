	/*********************************************************
** sample 
*********************************************************/
#include <string.h>
#include "Arduino.h"

#include "esm2m/m2mnet/include/m2m_app.h"
#include "esm2m.h"
#include "esm2m/system.h"
#include "app_board.h"

#include "lights_control/com/app_com.h"
#include "lights_control/led_entrance.h"
#ifdef __cplusplus
extern "C" {
#endif
void m2m_bytes_dump(u8 *p_shd,u8 *p,int len);

#ifdef __cplusplus
}
#endif



extern "C" int app_cmd_handle(u8 cmd,u8*p_data,int recv_len,M2M_packet_T **pp_ack_data);

typedef struct IOCTL_T{
    u8 mod;
    u8 nouse;
    u8 pin;
    u8 value;
}Ioctl_t;

int io_write(u8 mode,u8 pin,u8 val){
	pinMode(pin,mode);
    digitalWrite(pin,val);
	return 0;
}

Ioctl_t *io_stat_ack(u8 mode,u8 pin){
    Ioctl_t* p = (Ioctl_t*)malloc(sizeof(Ioctl_t));
    mmemset((u8*)p,0,sizeof(Ioctl_t));

	pinMode(pin,mode);
	
    p->mod   = mode;
    p->pin   = pin;
    p->value = digitalRead(pin);

	return p;
}

int io_cmd_handle(u8 mode,u8 pin,u8 val,M2M_packet_T **pp_ack_data){
	int ret = 0;
    if(1 == mode){//1  output
        io_write(mode,pin,val);
    }else{
        u8 *p_v = (u8*)io_stat_ack(mode,pin);
        byte_printf((u8*)"----->>p_v %s", (u8*)p_v, sizeof(Ioctl_t));
        M2M_packet_T *p_ack  = (M2M_packet_T*)malloc( sizeof(M2M_packet_T) );
        if(p_v && p_ack){
            memset(p_ack, 0, sizeof( M2M_packet_T) );
            p_ack->len = sizeof(Ioctl_t);//strlen( (const char*)p_v );
            m2m_log("ack %d", p_ack->len);
            p_ack->p_data = (u8*)malloc( p_ack->len + 1 );
            if( p_ack->p_data ){
                memset( p_ack->p_data, 0, p_ack->len + 1);
                mcpy(p_ack->p_data,  p_v, p_ack->len);
				ret =  sizeof(M2M_packet_T) + sizeof(Ioctl_t);
            }
            m2m_log_debug("---> ack %s", p_v );
            *pp_ack_data = p_ack;
        }

    }
	
	return ret;
}

int app_cmd_handle(u8 cmd,u8*p_data,int recv_len,M2M_packet_T **pp_ack_data){
	int ret = 0;
	Lm2m_ota_data putota;
	Lm2m_ota_data getota;
    //m2m_printf("-------------->>>>>>>>>>>>cmd is %d\n",cmd);
	switch (cmd){

#ifdef FAC_TEST

		case WIFI_CMD_FTST_MODLE:
				ret=board_fact_cmd_handle(pp_ack_data);
				break;

		case WIFI_CMD_FTST_RESULT: //心跳包
				{
					M2M_packet_T *p_ack_data=(M2M_packet_T*)malloc(sizeof(M2M_packet_T));
					p_ack_data->len=sizeof("OK");
					p_ack_data->p_data=(u8*)malloc(sizeof("OK"));
					memcpy(p_ack_data->p_data,"OK",sizeof("OK"));
					*pp_ack_data=p_ack_data;
				}
				break;
		case WIFI_CMD_FTST_ROUTER : 
				{		
                test_connect_router();
				}
				break;
#endif

		// case WIFI_CMD_APP_UART_SEND_RQ:
		// 	if(p_data && recv_len)
		// 		ret = Serial.write(p_data,recv_len);
		// 	break;
		case WIFI_CMD_APP_IO_CTL_RQ:
			if(recv_len == sizeof( Ioctl_t) && p_data ){
				ret = io_cmd_handle(p_data[0],p_data[2], p_data[3], pp_ack_data);
				}
			break;
		case WIFI_CMD_APP_UART_SEND_RQ:
    	case WIFI_CMD_APP_LED_TWINKLE_RQ:
    
			printf("===%d===",recv_len);	
			m2m_bytes_dump((u8*)"-----receive data:", p_data, recv_len);
			printf("====WARN==== enter app_cmd_handle!!!");
			ret = app_cmd_handler( p_data, recv_len, pp_ack_data);
			break;
		default:
			printf("error: unknow command!!\n");
			break;
	}
	
	return ret;
}


/**
**	系统状态函数.
**/
void app_system_status_loop(SYS_status status){
	board_led_status(status);
}

void app_setup(void){
	board_setup(); 
}

void app_loop(void){
	board_loop();
	Led_Entrance();
}
