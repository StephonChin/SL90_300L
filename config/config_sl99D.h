/****
 *  产品层面的配置， 设置 产品 id， 品类， 默认服务器地址以及端口， 本地设备端口， 默认会话秘钥
 * ***/
#ifndef H_PRODUCT_CONFIG_H
#define H_PRODUCT_CONFIG_H

/*品类*/
#define PRODUCT_CLASS (0x02)
/* 产品 id */
#define PRODUCT_ID (0X02)
/*服务器 ID*/
#define PRODUCT_SERVER_ID ("01100000000000000000000000000000")
/*服务器 地址*/
#define PRODUCT_SERVER_CNAME  ("m2mrelay.nielink.com")
/*服务器端口*/
#define PRODUCT_SERVER_PORT   (9527)
/*本地端口*/
#define PRODUCT_LOCAL_PORT  (9529)
/*会话秘钥*/
#define PRODUCT_KEY ("123")
/*AP 模式下，热点的名称*/
#define SFAP_SSID_	"ShowTree_"
/* 版本 */
#define VERSION_MAJOR   0
#define VERSION_MINOR   2
#define VERSION_PATCH   8
                                                                 
#endif
