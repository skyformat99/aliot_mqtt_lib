#ifndef _STOIOT_CONFIG_H_
#define _SIOIOT_CONFIG_H_

#include "mico.h"

/**************************************自主配置  ******************************************/
/*阿里物联网套件的配置*/
#define MCU_VERSION         "1.0.0" //设备目前版本号，对应阿里套件的版本号，用于ota标识
#define aliot_my_time        (1502694460) //加盐时间
#define VENDOR_ID           ("www_sioiot_com")  //产品标识，用于初始化系统存储的信息时很方便!   Maximum 20 bytes

//默认的设备激活demo，这个需要跟自己的服务器配合。
//如果仅仅测试，可以到sioiot_activate里面找到ceshi函数，修改对应参数
#define product_name        ("inthebeginning")  //
#define batch_number        ("00001")
#define activate_host       ("api.sioiot.com")
#define activate_url        ("/device/activate")
#define activate_type       ("application/json;charset=UTF-8")
#define activate_port       (443)
#define activate_mode       (0)
#define activate_timeout    (10000)
#define activate_buf_len    (2048)
#define activate_min_time   (1502694460)

//功能使能。注释取消功能
#define SUB_TOPIC_EN
//#define SUB_API_TOPIC_EN// apitopic是我自己加，目的实现自主的系统命令
#define SHADOW_TOPIC_EN
#define OTA_TOPIC_EN
/**************************************************************************************/
#define aliot_auth_host      ("iot-auth.cn-shanghai.aliyuncs.com")//阿里套件的https认证连接
#define aliot_auth_url       ("/auth/devicename")
#define aliot_auth_type      ("application/x-www-form-urlencoded")
#define aliot_mode           (0)
#define aliot_port           (443)
#define aliot_timeout        (4000)    //4s
#define aliot_buf_len        (2048)
#define  ALI_MQTT_PORT       (1883)
#define  ALI_MQTT_HOST       "iot-as-mqtt.cn-shanghai.aliyuncs.com"



typedef int         IOTStatus;

#define IOTNoErr                      0       //! No error occurred.
#define IOTGeneralErr                -1       //! General error.
#define IOTInProgressErr              1       //! Operation in progress.


#endif
