/*
 * esp_function.c
 * description: esp8266 function.
 *  Created on: 2018-6-13
 *      Author: skylli
 * Time list:
 */
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "Arduino.h"
#include <EEPROM.h>
#include <ArduinoOTA.h>

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "smartconfig.h"
#include "user_interface.h"
#include "lwip/inet.h"

}




#include <stdbool.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266httpUpdate.h>
#include "Ticker.h"

#include "m2mnet/include/m2m_type.h"
#include "m2mnet/include/util.h"
#include "m2mnet/include/m2m_log.h"
#include "m2mnet/include/m2m_api.h"
#include "m2mnet/include/m2m_app.h"
#include "m2mnet/include/m2m_port.h"

#include "system.h"
#include "../app_config.h"
#include "esm2m_handle.h"
#include "../config_hardware.h"
//#include "config.h"
#include <EEPROM.h>
//#include "esm2m.h"
#include "../app_handle.h"
#include "../app_board.h"
#define EEPROM_VER	(0X12)
#define DIFF(a,b) ((a>b)?(a-b):(b-a))
#define resttime 5000
Ticker myTicker; //建立一个需要定时调度的对象

static u8 *p_g_ota_url = NULL;
static u8 g_reboot = 0;
 SYS_status g_sys_cnn = SYS_LOST_CONNECT;
 u32 g_smartconfiging_time = 0;
static BOOL en_broadcast = TRUE;
unsigned int g_wifi_configing = 0;
/* c interface */
extern "C" void sys_setup(void);
extern "C" void sys_factory_reset(void);
extern "C" void sys_sta_smartconfig(void);
extern "C" int sys_ssid_pw_reset(LM2M_router_conf *p_router);

extern "C"  SYS_status sys_connect_status_hanle();
extern "C"	int sys_cmd_handle(size_t net,u8 cmd,u8*p_data,int recv_len,M2M_packet_T **pp_ack_data);

int sys_eeprom_factory_reset(void);
bool _sys_in_smartconfiging(void);
int sys_host_config(size_t net,u8 *p_data, int recv_len);

// function
// runing mode 
EEPROM_conf_T sys_conf;
/**********************************************
led_flash

********/
void led_flash()                        //中断处理函数，改变灯的状态
{                        
  static boolean output = HIGH;
  digitalWrite(LED_WIFI_CONN_PIN, output);
  output = !output;
}


int sys_eeprom_read(int address, u8 *p_buf, int len){
	int i;
	//EEPROM.begin(len);
	for(i=0;i<len;i++){
		p_buf[i] =  EEPROM.read(address+i);
	}
	return i;
}
int sys_eeprom_write(int address, u8 *p_buf, int len){
	int i;
	//EEPROM.begin(len);
	for(i=0;i<len;i++){
		EEPROM.write( address + i, p_buf[i]);
	}
	EEPROM.commit();
	return i;
}
int sys_wifi_mode_set(LESP_WIFI_MODE_T mod){
	sys_conf.wifi_mod = mod;
	sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf,  sizeof(EEPROM_conf_T));
}
static void sys_broadcast_set(BOOL status){
	en_broadcast = status;
}
BOOL sys_broadcast_enable(void){
	return en_broadcast;
}
/**********************************************
** description: 保存秘钥.
** args:    
**      
**          p_key;p_keylen;
********/
u32 esp8266_secretKey_write(u8 *p_key,u16 p_keylen)
{ 
	 if( sys_conf.ver_mak != EEPROM_VER )
     return 1;
    mcpy(sys_conf.p_key,p_key,p_keylen);
	sys_conf.keylen=p_keylen;
	sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf,  sizeof(EEPROM_conf_T));

	return 0;
}
u32 esp8266_secretKey_read(u8 *p_key,u16 *p_keylen)
{
 if( sys_conf.ver_mak != EEPROM_VER || sys_conf.keylen>32 || sys_conf.keylen<1 )
   return 1;
 mcpy(p_key,sys_conf.p_key,sys_conf.keylen);
 *p_keylen=sys_conf.keylen;
  return 0;

}
// todo remove 
void ssid_conf_print(struct station_config *p_conf){
	printf("ssid is : %s : password: %s \n", p_conf->ssid, p_conf->password);
	printf("bssid: %d bssid set  is [%0x][%0x][%0x][%0x][%0x][%0x]\n", p_conf->bssid_set, p_conf->bssid[0],
			p_conf->bssid[1],p_conf->bssid[2],p_conf->bssid[3],p_conf->bssid[4],p_conf->bssid[5]);	
}

void	sys_wifi_handle_event_cb(System_Event_t	*evt){
	struct station_config conf, def_conf;

	 printf("event	%x\n",	evt->event);

	 switch	(evt->event)	{

					 case	EVENT_STAMODE_CONNECTED:

					 	 printf("connect	to	ssid	%s,	channel	%d\n",	

	 	 	 	 	 evt->event_info.connected.ssid,	

	 	 	 	 	 evt->event_info.connected.channel);

					 	 break;

					 case	EVENT_STAMODE_DISCONNECTED:

					 	 printf("disconnect	from	ssid	%s,	reason	%d\n",	

	 	 	 	 	evt->event_info.disconnected.ssid,	
	 	 	 	 	evt->event_info.disconnected.reason);
					//wifi_station_get_config(&conf);
					//wifi_station_disconnect();
					//conf.bssid_set = 0;
					//wifi_station_set_config_current(&conf);
					//wifi_station_connect();
					 	 break;

					 case	EVENT_STAMODE_AUTHMODE_CHANGE:

					 				printf("mode:	%d	->	%d\n",	

	 	 	 	 	 evt->event_info.auth_change.old_mode,	

	 	 	 	 	 evt->event_info.auth_change.new_mode);

					 				break;

					 case	EVENT_STAMODE_GOT_IP:

					 	 printf("ip:"	IPSTR	",mask:"	IPSTR	",gw:"	IPSTR,

					 	 	 	 	 		IP2STR(&evt->event_info.got_ip.ip),

					 	 	 	 	 		IP2STR(&evt->event_info.got_ip.mask),

					 	 	 	 	 		IP2STR(&evt->event_info.got_ip.gw));

					 	 printf("\n");
					 	 break;

					 case	EVENT_SOFTAPMODE_STACONNECTED:

					 				printf("station:	"	MACSTR	"join,	AID	=	%d\n",	

	 	 	 	 MAC2STR(evt->event_info.sta_connected.mac),	

	 	 	 	 evt->event_info.sta_connected.aid);

					 				break;

	 				case	EVENT_SOFTAPMODE_STADISCONNECTED:

	 			printf("station:	"	MACSTR	"leave,	AID	=	%d\n",	

	 	 	 	 MAC2STR(evt->event_info.sta_disconnected.mac),	

	 	 	 	 evt->event_info.sta_disconnected.aid);

					 				break;

					 default:

					 	 break;

	 }

}

void sys_wifi_init(void){

	u8 ssid[24] = {0};
	u8 mac[12] ={0};
	WiFi.macAddress( mac );
	sprintf((char*)ssid, "%s%02x%02x%02x", "SHM", mac[3], mac[4], mac[5]);
	WiFi.hostname((const char*)ssid);
	
	switch( sys_conf.wifi_mod ){
		
		case WIFI_MODE_RST_SMT:
		
			m2m_log_debug("WIFI_MODE_RST_SMT");
			sys_sta_smartconfig();
			// sys_conf.wifi_mod = WIFI_MODE_STA_SMT;
			// sys_eeprom_write(EEPROM_CONF_ADDRESS, (u8*)&sys_conf, sizeof(EEPROM_conf_T));
			break;
		
		case WIFI_MODE_RST_AP:
			{
				u8 mac[8];
				char ssid[128];
 				memset(ssid, 0, 128);			
				memset( mac, 0, 8);
			
				m2m_log_debug("WIFI_MODE_RST_AP");
				WiFi.mode( WIFI_AP );
				WiFi.macAddress( mac );
				sprintf(ssid, "%s%02x%02x", SOFTAP_SSID_,mac[4],mac[5]);
				m2m_log_debug("ssid is : %s", ssid);
				
				sys_wifi_mode_set(WIFI_MODE_RST_SMT);	
				delay(50);
				sys_conf.wifi_mod = WIFI_MODE_RST_AP;
				if(WiFi.softAP(ssid)){
					m2m_log_debug("Ready");
				}else m2m_log_debug("failt");

				m2m_log_debug("start ap mode.");
			}
			break;
		case WIFI_MODE_STA_SMT:
			m2m_log_debug("WIFI_MODE_STA_SMT");
			WiFi.mode(WIFI_STA);

			WiFi.setAutoConnect(false);
			if (!wifi_station_set_auto_connect(false)) {
				printf("disable auto connect failed");
			}

			WiFi.begin();
			break;
		default:
			m2m_printf("unknow mod, system will be rewrit flash and reboot!!");
			sys_eeprom_factory_reset();
			sys_wifi_mode_set(WIFI_MODE_RST_SMT);
			delay(100);
			ESP.restart();
			break;
	}
	
}


static void sys_cnn_status_set(SYS_status status){
	g_sys_cnn = status;
}
BOOL sys_in_ap_mode(void){
	if (WiFi.getMode() == WIFI_AP)
		return TRUE;
	else
		return FALSE;

}

SYS_status sys_connect_status_hanle(){
    //static bool s_fag = 0;
	static u32 old_tm = m2m_current_time_get()+5000;
	//m2m_printf("<%d>",status);
	u32 c_time = m2m_current_time_get();
	SYS_status ret_status =SYS_MAX;
	switch(g_sys_cnn){
		case SYS_LOST_CONNECT:
			//if(WiFi.getMode() == WIFI_STA && WiFi.status() !=  WL_CONNECTED)
			//	break;		
			//g_sys_cnn = SYS_ONLINE;
			ret_status = SYS_LOST_CONNECT;
		case SYS_ONLINE:
             if(WiFi.getMode() == WIFI_STA && WiFi.status() !=  WL_CONNECTED){
				g_sys_cnn = SYS_LOST_CONNECT;
				ret_status =  SYS_LOST_CONNECT;
            }
                break;
		case SYS_CONFIGING_AP:
                //g_sys_cnn = SYS_CONFIGING_AP;
                ret_status = SYS_CONFIGING_AP;
		      	g_sys_cnn = SYS_LOST_CONNECT;
				break;

	}
    if(sys_in_ap_mode()){
        g_sys_cnn = SYS_CONFIGING_AP;
        ret_status = SYS_CONFIGING_AP;
    }
	app_system_status_loop((SYS_status)g_sys_cnn);
	return ret_status;
}
int sys_eeprom_factory_reset(void){
	u8 *p = NULL;
	int len =  sizeof( SYS_Host_info_t ) + strlen(TST_SERVER_HOST);
	SYS_Host_info_t *p_host = (SYS_Host_info_t*) mmalloc( len +1 );
	
	if(!p_host)
		return  M2M_ERR_NULL;
	// todo
	sys_conf.ver_mak = EEPROM_VER;
	memcpy(sys_conf.p_key,"123",sizeof("123")); 
	sys_conf.keylen = strlen("123");
	sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf,  sizeof(EEPROM_conf_T));
	
	// todo add aes aeskey
	// host reset to factory configure.
	p_host->port = TST_SERVER_PORT;
	p_host->len = strlen(TST_SERVER_HOST);
	mcpy( (u8*)p_host->cname, (u8*)TST_SERVER_HOST, strlen(TST_SERVER_HOST) );
	p = (u8*)PRODUCT_SERVER_ID;
	STR_2_HEX_ARRAY_INV( p_host->s_id.id, ID_LEN, p, strlen(PRODUCT_SERVER_ID));
	//byte_printf((u8*)"---freset id: ", (u8*)p_host->s_id.id, ID_LEN );
	//m2m_printf("\nreset host %s\n", p_host->cname);
	sys_host_config(NULL, (u8*)p_host, len);
	mfree(p_host);
	return 0;
}
void  sys_factory_reset(void){
	static u32 last_tm = 0 ; 
    
    #ifdef RESET_TEST
	if(0 == digitalRead(FAC_TEST_PIN))//m
      digitalWrite(LED, 0);//
    #endif
    
	if(0 == digitalRead(REST_PIN)){
		u32 curr_tm = m2m_current_time_get();
		if( DIFF( curr_tm,last_tm ) > (resttime - 400) ){

		//	WiFi.disconnect();
		//	WiFi.setAutoConnect(false);
			sys_eeprom_factory_reset();
	//		mmemset( sys_conf.p_ssid_pw, 0,  64);
			if( _sys_in_smartconfiging() && sys_conf.wifi_mod == WIFI_MODE_RST_SMT ){
				sys_wifi_mode_set(WIFI_MODE_RST_AP);
			}else{
				sys_wifi_mode_set(WIFI_MODE_RST_SMT);
				// do not auto connect 
				wifi_station_set_auto_connect(0);
			}
			// reset wifi system data.
			system_restore();
			delay(100);
			system_restart();
			delay(200);
			ESP.restart();
			}    
	}else 	
		last_tm = millis(); 
			   
} 


/*
* Start smart configuration mode by sniffing for special packets 
*  that contain SSID and password of desired Access Point. Depending on result either true or 
*  false is returned.
*/
void sys_sta_smartconfig(void) {
  int ret = 0;

  g_sys_cnn= SYS_CONFIGING_STA;
  //Init WiFi as Station, start SmartConfig
  //printf("dhcp status : %d \n", wifi_station_dhcpc_status() );
  ret = smartconfig_set_type(SC_TYPE_ESPTOUCH);

  WiFi.mode( WIFI_STA );
  WiFi.beginSmartConfig();
  g_smartconfiging_time = m2m_current_time_get();
  
}
void sys_m2m_setup(void){
	static u8 m2m_setup_flag = 0;


	if ( WiFi.isConnected() || WiFi.getMode() & WIFI_AP ){	

		if( !m2m_setup_flag ){
			local_ip_save();
			m2m_setup();
			m2m_setup_flag = 1;
		}
	}else{
		delay(5);
	}
}
void sys_m2m_loop(void){
	if ( WiFi.isConnected() || WiFi.getMode() & WIFI_AP )
		m2m_loop();
}
void sys_sta_smartconf_end(void){

	if( sys_conf.wifi_mod ==  WIFI_MODE_RST_SMT){
		if(WiFi.isConnected()){
			g_sys_cnn = SYS_ONLINE;
			sys_conf.wifi_mod = WIFI_MODE_STA_SMT;
			sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf, sizeof(EEPROM_conf_T));
			

		}else{
						// import..
			if(g_wifi_configing){
				delay(200);		
			}
			// if( g_sys_cnn == SYS_CONFIGING_STA && WiFi.smartConfigDone()){
			// 	g_sys_cnn = SYS_LOST_CONNECT;
			// }
		}
	}	
}

bool _sys_in_smartconfiging(void){
	u32 curr_tm = m2m_current_time_get();
	if(DIFF(curr_tm, g_smartconfiging_time) < MAX_SMARTCONFIG_TIME)
		return TRUE;
	else
		return FALSE;
}
void sys_sta_smartconf_stop(void){
	u32 curr_tm = m2m_current_time_get();
	if(g_sys_cnn == SYS_CONFIGING_STA && curr_tm > g_smartconfiging_time && DIFF(curr_tm, g_smartconfiging_time) > MAX_SMARTCONFIG_TIME ){
			WiFi.stopSmartConfig();
      g_sys_cnn = SYS_LOST_CONNECT;
			g_wifi_configing = 0;
	}
}
void sys_conf_printf(void){
	m2m_printf("version %d ", sys_conf.ver_mak);
	m2m_printf("wifi mode %d", sys_conf.wifi_mod);
	m2m_printf("sys restart counter: %d", sys_conf.reset_cnt );
//	m2m_log("ssi_password %s", sys_conf.p_ssid_pw);
	m2m_printf("server port  %d cname %s", sys_conf.host.port, sys_conf.host.cname);
	byte_printf((u8*)"server host id is ", (u8*)sys_conf.host.s_id.id, ID_LEN);
}
void system_info_printf(void){
	char* flsh_size[]={
        "FLASH_SIZE_4M_MAP_256_256",

	 "FLASH_SIZE_2M",

	 "FLASH_SIZE_8M_MAP_512_512",

	 "FLASH_SIZE_16M_MAP_512_512",

	 "FLASH_SIZE_32M_MAP_512_512",

	 "FLASH_SIZE_16M_MAP_1024_1024",

	 "FLASH_SIZE_32M_MAP_1024_1024",

	 "FLASH_SIZE_64M_MAP_1024_1024",

	 "FLASH_SIZE_128M_MAP_1024_1024",
    };
		Serial.println(system_get_sdk_version()) ;
    enum	flash_size_map size=system_get_flash_size_map();
    Serial.println(flsh_size[size]);
}
// system 


u8 testbit=0;
void sys_debug_info(void){
	if( M2M_LOG > m2m_record_level_get()   ){
		
		struct station_config conf, def_conf;
		
		wifi_set_event_handler_cb(sys_wifi_handle_event_cb);
		WiFi.macAddress();
		Serial.print("\n ----> MAC: ");
		Serial.println(WiFi.macAddress());
		printf("sdk version: %s\n", system_get_sdk_version() );
		printf("physical mode %d \n", wifi_get_phy_mode() );
		printf("smart config version: %s\n", smartconfig_get_version());
		system_info_printf();
		sys_conf_printf();
		wifi_station_get_config(&conf);
		ssid_conf_print(&conf);
	}

}
void sys_setup(void){

	g_reboot = 0;
	
	system_set_os_print(0);
	Serial.setDebugOutput(FALSE);
	
	if(!testbit){
		EEPROM.begin( EEPROM_CONF_SIZE_MX );
		sys_eeprom_read( EEPROM_CONF_ADDRESS, (u8*)&sys_conf, sizeof(EEPROM_conf_T) );
		if( sys_conf.ver_mak != EEPROM_VER ){
			sys_eeprom_factory_reset();
			sys_wifi_mode_set(WIFI_MODE_RST_SMT);
		}
		// reset counter.·
		sys_conf.reset_cnt++;
		sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf,  sizeof(EEPROM_conf_T));
		sys_wifi_init();

	}else
	{
		m2m_setup();
		g_sys_cnn = SYS_LOST_CONNECT;
	}
	
	
}

int sys_ssid_pw_reset(LM2M_router_conf *p_router){

	u8 *p = NULL;
	u8 old_wifi_mod = sys_conf.wifi_mod;
	if(!p_router  || !p_router->ssidlen || ( p_router->ssidlen + p_router->passwordlen )  > 64 )
		return -1;
	
	u8 flag = 0;
    u32 last_tm = 0;

  	m2m_log("set ssid and pwd!ssidlen=%d,pwdlen=%d,pwd=%.*s\r\n",\
  			p_router->ssidlen,p_router->passwordlen,p_router->passwordlen+p_router->ssidlen,p_router->p_ssid_pw);
	
	char *ssid=(char*)malloc(p_router->ssidlen+1);
	mmemset((u8*)ssid,0,p_router->ssidlen);
	memcpy(ssid,p_router->p_ssid_pw,p_router->ssidlen);
	ssid[p_router->ssidlen]='\0';
	
	char *pwd=(char*)malloc(p_router->passwordlen+1);
	mmemset((u8*)pwd,0,p_router->passwordlen);
	memcpy(pwd,&p_router->p_ssid_pw[p_router->ssidlen],p_router->passwordlen);
	pwd[p_router->passwordlen]='\0';
	m2m_log("ssid:%s,pwd:%s,\r\n",ssid,pwd);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid,pwd);
	//m2m_printf("\n--->ssid and password is %s\n", p_router->p_ssid_pw);
#if 1
	m2m_log("connect successs!\r\n");
	//WiFi.disconnect();
	//WiFi.setAutoConnect(false); 
	
	 // light up led.
  	app_system_status_loop((SYS_status)SYS_LOST_CONNECT);  
	sys_conf.wifi_mod = WIFI_MODE_STA_SMT;
	sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf, sizeof(EEPROM_conf_T));
	free(ssid);
	free(pwd);
	g_reboot = 1;
//	sys_conf.wifi_mod = old_wifi_mod;
#endif

	return 0;
}

int sys_host_config(size_t net,u8 *p_data, int recv_len){
	SYS_Host_info_t host;

	_RETURN_EQUAL_0( p_data,  M2M_ERR_INVALID);
	_RETURN_EQUAL_0( recv_len,  M2M_ERR_INVALID);

	
	mcpy( (u8*)&sys_conf.host, (u8*)p_data, sizeof(SYS_Host_info_t));
	
	sys_eeprom_write( EEPROM_CONF_ADDRESS, (u8*)&sys_conf, sizeof(EEPROM_conf_T));
	
	sys_eeprom_write( EEPROM_CONF_ADDRESS + sizeof(EEPROM_conf_T), (u8*)(p_data +  sizeof(SYS_Host_info_t) ), recv_len - sizeof(SYS_Host_info_t) );
	m2m_log_debug("Host cname %s have been write to eeprom",  &p_data[ sizeof(SYS_Host_info_t)] );

	
	u8 *p = &p_data[sizeof(SYS_Host_info_t)];
	mcpy( (u8*)&host, (u8*)p_data, sizeof(SYS_Host_info_t));
	if(net)
		m2m_net_host_update(net, &host.s_id, p, host.port);

	
	return recv_len;
}
/**
**  读取 eeprom 获取 host， 注意 活的的 host 必须 调用 mfree 进行销毁。
****/
SYS_Host_info_t *sys_host_alloc(M2M_id_T *p_id    ){

	SYS_Host_info_t *p_host = NULL;
	_RETURN_EQUAL_0( sys_conf.host.len,  NULL);

	p_host = (SYS_Host_info_t*) mmalloc( sizeof(SYS_Host_info_t) + sys_conf.host.len + 1);
	_RETURN_EQUAL_0(p_host,  NULL);
	mcpy( (u8*)p_host, (u8*)&sys_conf.host, (int)sizeof( SYS_Host_info_t));

	if(p_host->len){
		sys_eeprom_read( EEPROM_CONF_ADDRESS + sizeof(EEPROM_conf_T),  p_host->cname, p_host->len);
	}
	if(p_id){
		mcpy((u8*)p_id, (u8*)sys_conf.host.s_id.id, ID_LEN);
	}

	m2m_log_debug( "host cname is %s",  p_host->cname);
	
	return p_host;
}
int sys_ota_update(const u8 *p_url){

	if(p_url && strstr( (const char*)p_url, "http:") ){

        board_led_status(SYS_OTAING);
        myTicker.attach(0.2, led_flash); //初始化调度任务，每0.5秒执行一次led_flash()
        //SYS_status_set(SYS_CNN_OTAING);
		t_httpUpdate_return  ret = ESPhttpUpdate.update((const char*)p_url);
		sys_cnn_status_set(SYS_OTAING);
			switch(ret) {
				case HTTP_UPDATE_FAILED:
					m2m_printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
					break;
			
				case HTTP_UPDATE_NO_UPDATES:
					m2m_printf("HTTP_UPDATE_NO_UPDATES\n");
					break;
			
				case HTTP_UPDATE_OK:
					m2m_printf("HTTP_UPDATE_OK\n");
					break;
			}
	}
	
}
int sys_cmd_handle(size_t net,u8 cmd,u8*p_data,int recv_len, M2M_packet_T **pp_ack_data){
	int ret = 0;
	Lm2m_ota_data putota;
	Lm2m_ota_data getota;

	switch (cmd){	
		case WIFI_CMD_SYS_VERSION_RQ:
			   {
				   u8 *p_v = m2m_version();
		
				   //M2M_packet_T *p_ack  = ( M2M_packet_T*)mmalloc( sizeof(M2M_packet_T) );
				   M2M_packet_T *p_ack	= (M2M_packet_T*)malloc( sizeof(M2M_packet_T) );
				   if(p_v && p_ack){
					   memset(p_ack, 0, sizeof( M2M_packet_T) );
					   p_ack->len = strlen( (const char*)p_v );
					   m2m_log("version %d", p_ack->len);
					   p_ack->p_data = (u8*)malloc( p_ack->len + 1 );
					   if( p_ack->p_data ){
						   memset( p_ack->p_data, 0, p_ack->len + 1);
						   mcpy(p_ack->p_data,	p_v, p_ack->len);
					   }
					   m2m_log_debug("---> version %s", p_v );
					   *pp_ack_data = p_ack;
				   }
				   break;
			   }

		case WIFI_CMD_SYS_RELAYHOST_SET_RQ:
		{
			if( p_data && recv_len)
				ret = sys_host_config(net,p_data, recv_len);
		}
			break;
		case WIFI_CMD_SYS_REBOOT_RQ:
			// todo ack 
			g_reboot = 1;	
			break;
		case WIFI_CMD_ROUTER_CONF:
			if(p_data  && recv_len > sizeof(LM2M_router_conf)){
				// todo 
				ret = sys_ssid_pw_reset( (LM2M_router_conf*)p_data );
			}
			break;			
		case WIFI_CMD_SYS_OTA_UPDATE_RQ:
			//m2m_printf("----->>WIFI_CMD_SYS_OTA_UPDATE_RQ \n");
			if(p_data && recv_len > 0){
				p_g_ota_url = (u8*)mmalloc(strlen((char*)p_data) +1);
				if(p_g_ota_url)
					mcpy(p_g_ota_url, p_data, strlen((char*)p_data));
			}		
			break;
		case WIFI_CMD_SYS_BROADCASTSETTING_RQ:
			if(p_data && recv_len > 0){
#ifdef CONF_BROADCAST_ENABLE
				if(p_data[0]){
					m2m_broadcast_enable(net);
				}else{					
					m2m_broadcast_disable(net);
				}
#endif // CONF_BROADCAST_ENABLE

			}
		case WIFI_CMD_FACT_REST: // 7
			{				
				sys_eeprom_factory_reset();
				sys_wifi_mode_set(WIFI_MODE_RST_SMT);
				g_reboot = 1;
				break;
			}

     }
	return ret;
}
void _on_off_line_handle(){
	// online offline check 
	if(sys_conf.wifi_mod == WIFI_MODE_STA_SMT){
		if(net_offline()){
			g_sys_cnn=SYS_LOST_CONNECT;
		}
		else{
			g_sys_cnn=SYS_ONLINE;	
			}
	}

}
void _cnn_status(){
	static u32 tt = 0;
	if(DIFF(tt,m2m_current_time_get()) > 1000){
		printf("<<%d>> \n", WiFi.status());
		tt = m2m_current_time_get();
	}
	
}
void system_loop(void){
 // _cnn_status();
	sys_m2m_loop();
  if(testbit)
	   return;
	if(p_g_ota_url){
		
		m2m_printf("----->>p_g_ota_url %s \n", p_g_ota_url);
		sys_ota_update(p_g_ota_url);
		mfree(p_g_ota_url);
		p_g_ota_url = NULL;
	}
	sys_sta_smartconf_stop();
	sys_sta_smartconf_end();
	
	//指示灯控制.
	//if(testbit==0)
	sys_connect_status_hanle();
	// 恢复出厂按键探测.
	sys_factory_reset();
	_on_off_line_handle();
	
	sys_m2m_setup();

	//if reset 
	if(g_reboot){
		if(g_reboot++ > 5){
			g_reboot = 0;
            ESP.restart();
        }
	}
}
