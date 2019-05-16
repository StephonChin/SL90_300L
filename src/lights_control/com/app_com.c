/****************************************************************
  *
  * FileName  app_com.c
  * Brief     communicate withe APP
  *
*****************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../include/m2m.h"
#include "../process/data_process.h"
#include "app_com.h"




/**
  * FunctionName  app_cmd_handler
  */
int app_cmd_handler(uint8_t *pdata, int rcv_len, M2M_packet_T **pp_ack_data)
{
    uint8_t *p = pdata;
    int ret = 0;
    
	//m2m_printf(" 01 len %d  p = %p\n", rcv_len, pdata);

	//printf("=====WARN==== app_cmd_handler enter!!!");
	
    //check the update of received data
    if(pdata && rcv_len && (rcv_len < APP_PACK_SIZE_MAX)){
        uint8_t *dst = (uint8_t *)&app_pack;

		//printf("=====WARN==== memcpy!!!");
		memcpy(dst, p, rcv_len);
    }

	
    //check the index to ack  data
    if(app_ack_pack.index){
        uint16_t len = app_ack_pack.len + 8;
        uint8_t *src = (uint8_t *)&app_ack_pack;

        M2M_packet_T *p_ack = (M2M_packet_T*)malloc( sizeof(M2M_packet_T) );
        if(!p_ack){
            return -1;
        }
        memset(p_ack, 0, sizeof(M2M_packet_T) );
        
        p_ack->len = (uint32_t)len;
        p_ack->p_data = (uint8_t *)malloc(len);
        if( p_ack->p_data){
            memset( p_ack->p_data, 0, len );
            memcpy(p_ack->p_data, src, len);
            ret = len;
            *pp_ack_data = p_ack;
        }

        
		
        printf("\n== ACK == ");
        uint8_t x = p_ack->len;
        uint8_t *p = (uint8_t *)&app_ack_pack;
        while(x--)
        {
			printf("0x%x,",*p);
			p++;
        }
        printf("\n");

        printf("==ppa_ack_data=%p\n",*pp_ack_data);

        app_ack_pack.index = 0;
       
    }
    return ret;
}


