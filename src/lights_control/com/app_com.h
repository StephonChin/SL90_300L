/*********************************************************
  *
  * FileName  app_com.h
  * Brief     communicate with APP
  *
**********************************************************/
#ifndef _APP_COM_H_
#define _APP_COM_H_

#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------
#ifndef WIFI_CMD_APP_UART_SEND_RQ
//#define WIFI_CMD_APP_UART_SEND_RQ     (uint8_t)0x52
#endif

/**
  * FunctionName  app_cmd_handler()
  * Input
  *       cmd
  *       *pdata          received data header adress
  *       rcv_len         received data length
  *       **pp_pack_data  ack data adress
  */
int app_cmd_handler(uint8_t *pdata, int rcv_len, M2M_packet_T **pp_ack_data);

#endif

#ifdef __cplusplus
}
#endif

