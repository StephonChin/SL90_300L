/*********************************************************

20191218: Change:   sta 模式 绿灯， 快闪表示 正在配置或者 升级固件，慢闪表示 断开外网  常亮表示连接上路由和外网服务器
				    AP 模式  蓝灯常亮
**  
*********************************************************/
#include <string.h>
#include "Arduino.h"
#include  <ESP8266WiFi.h>
#include "app_config.h"
#include "app_board.h"
#include "esm2m/system.h"
#include "esm2m.h"

#include "lights_control/led_entrance.h"
#include "lights_control/com/app_com.h"

#define DIFF(a,b) ((a>b)?(a-b):(b-a))

typedef enum _LED_COLOR{
	LED_R ,
	LED_G,
	LED_B
}LED_COLOR;

void board_gpio_init(void){

	randomSeed(analogRead(0));

	pinMode(LED_R_PIN, OUTPUT);
	pinMode(LED_G_PIN, OUTPUT);
	pinMode(LED_B_PIN, OUTPUT);
	pinMode(REST_PIN, INPUT);
	
	digitalWrite(LED_R_PIN, 1);
	digitalWrite(LED_G_PIN, 1);
	digitalWrite(LED_B_PIN, 1);
}

// void board_uart_report_status(int stat){
// 	static int old_stat = 0;
// 	char buf[5];
// 	int i;
	
// 	if( old_stat == stat )
// 		return;
	
// 	delay(200);
// 	//m2m_printf("\n\t ---> board_uart_report_status\n");
// 	buf[0] = VERSION;
// 	buf[1] = CMD;
// 	buf[2] = IDX;
// 	buf[3] = LEN;
// 	buf[4] = stat;
// 	for(i=0;i<5;i++)
// 		 Serial.print(buf[i]);
// 	delay(200);
// 	// todo 
// 	old_stat = stat;

// }

#ifdef FAC_TEST	
void board_fact_setup(void){

  pinMode( FAC_TEST_PIN,INPUT_PULLUP);
	//Serial.printf("GPIO12:%s\n",digitalRead(FAC_TEST_PIN)?"HIGHT":"LOW");
	
	if(digitalRead(FAC_TEST_PIN)==TEST_MODE)
	{
		testbit=1;
		pinMode(2,OUTPUT);
		digitalWrite(2,0);
		pinMode(TEST_IO,OUTPUT);
    digitalWrite(TEST_IO,HIGH);
		Serial.begin(9600);
		//Serial.printf("Test Mode!\n");
		WiFi.begin(TEST_SSID,TEST_PWD);
		 while (WiFi.status() != WL_CONNECTED)
		{
			delay(500);
		}
	}

}

int test_connect_router(void)
{
  WiFi.disconnect(true);
	while(WiFi.status() == WL_CONNECTED)
	{
		delay(100);
		WiFi.mode(WIFI_AP);
		WiFi.mode(WIFI_STA);
	}
	WiFi.mode(WIFI_STA);
  //printf("connect ssid:%s,pwd:%s\n",TEST_ROUTER_SSID,TEST_ROUTER_PWD);
	WiFi.begin(TEST_ROUTER_SSID,TEST_ROUTER_PWD);
  while (WiFi.status() != WL_CONNECTED)
	{
		delay(100);
	}
	local_ip_save();
	//printf("connect success!\n");
  pinMode(TEST_IO,OUTPUT);
  digitalWrite(TEST_IO,LOW);
  return 0;
}

int board_fact_cmd_handle(M2M_packet_T **pp_ack_data){
    Wifi_pkt msg;
    memset(&msg,0,sizeof(Wifi_pkt));
    msg.version= WIFI_S_VERSION;
    msg.cmd    = WIFI_CMD_FTST_MODLE;
    M2M_packet_T *p_ack_data=(M2M_packet_T*)malloc(sizeof(M2M_packet_T));
    if(p_ack_data)
    {
        p_ack_data->len=sizeof(Wifi_pkt);
        p_ack_data->p_data=(u8*)malloc(p_ack_data->len+1);
        if(p_ack_data->p_data)
        {
            memcpy(p_ack_data->p_data,&msg,p_ack_data->len);
        }
    }
    *pp_ack_data=p_ack_data;
		// Serial.end();
		// Serial.begin(9600);
#if 0

		char buffer[256]={0};
		Serial.flush();//清空缓存区
    Serial.printf("FTST_TEST\n");
		time_t itime =time(NULL)+5;//最大等待时间5s
    size_t isize=0;
		int sum=0;
    while(itime>time(NULL))
    {
    if(Serial.available())
    {
        isize+= Serial.read(&buffer[isize],256-isize);
        if(strstr(buffer,"FTST_TEST") || strstr(buffer,"ftst_test"))
          {
            pinMode(TEST_IO,OUTPUT);
            digitalWrite(TEST_IO,HIGH);
						for(sum=0;sum<5;sum++){
							Serial.printf(IO_STR"\n");
							Serial.write(IO_STR"\n");
							delay(1);
							yield();
						}
							break;
          } 
    }
		if(sum%500==0)
		Serial.printf("FTST_TEST");
		system_soft_wdt_feed();
    yield();
		delay(1);  
		sum++;
  }
#endif
    return  p_ack_data->len;
}

#endif

static void board_led_flash(LED_COLOR color){
	static u8 led_status = 0;
	
	led_status = (led_status)?0:1;
	switch(color){
		case LED_R:
			digitalWrite(LED_R_PIN, led_status);
			break;
		case LED_G:
			digitalWrite(LED_G_PIN, led_status);
			break;
		case LED_B:
			digitalWrite(LED_B_PIN, led_status);
			break;
		default:
			return;
	}

	
}
void board_led_color(LED_COLOR color, int status){
	int r = 1, g =1, b=1;
	switch(color){
		case LED_R:
			r = status;
			break;
		case LED_G:
			g = status;
			break;
		case LED_B:
			b = status;
			break;
		default:
			return;
	}
	digitalWrite(LED_R_PIN,r);
	digitalWrite(LED_G_PIN, g);
	digitalWrite(LED_B_PIN, b);
}
/**
**	系统状态函数.
**/
void board_led_status(SYS_status status){

	static u32 old_tm = m2m_current_time_get();
	//m2m_printf("<%d>\n",status);
	u32 c_time = m2m_current_time_get();

	// todo just for sl99.
	// board_uart_report_status(status);

	switch(status){
		
		case SYS_OTAING:
			if( DIFF(old_tm, c_time ) >= 300){
				old_tm = c_time;
				board_led_flash(LED_G);
			}
			break;
		
		case SYS_CONFIGING_STA://在STA模式下 快闪
			if( DIFF(old_tm, c_time ) >= 250){
				old_tm = c_time;
				board_led_flash(LED_G);
			}
			break;

		case SYS_CONFIGING_AP://在AP模式下 慢闪
			board_led_color(LED_B, 0);
			break;

		case SYS_LOST_CONNECT://没连接路由 灭
			if( DIFF(old_tm, c_time ) >= 1300){
					old_tm = c_time;
					board_led_flash(LED_G);
				}
			break;

		case SYS_ONLINE://在线常亮
			board_led_color(LED_G, 0);
			break;
	}

}

void board_setup(void){

	system_update_cpu_freq(SYS_CPU_160MHZ);
	
#ifdef RESET_TEST
    pinMode(FAC_TEST_PIN, OUTPUT);//M
    analogWrite(FAC_TEST_PIN, 1);
    pinMode(FAC_TEST_PIN, INPUT);
    pinMode(LED, OUTPUT);//
    digitalWrite(LED, 1);//
#endif
#ifdef FAC_TEST
	board_fact_setup();
#endif	
 if(testbit==0)
	board_gpio_init();

}
void board_loop(void){

	if(WiFi.isConnected() ){
	    digitalWrite(LED, 0);//
	}else{
	    digitalWrite(LED, 1);//
	}
}

