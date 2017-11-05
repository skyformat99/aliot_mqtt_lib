
#ifndef _SIOIOT_ACTIVATE_H_
#define _SIOIOT_ACTIVATE_H_

#include "sioiot_config.h"

extern mico_semaphore_t wait_sem;


#define NUM_TOKENS         (20)
#define SIOIOT_NAME_LEN    (64)
#define SIOIOT_PASS_LEN    (64)
#define SIOIOT_INFO_LEN    (20)
#define SIOIOT_ID_LEN      (128)
#define SIOIOT_TOPIC_LEN   (128)
#define SIOIOT_HOST_LEN    (128)
#define SIOIOT_DEVICE_NAME_LEN      (65)
#define SIOIOT_DEVICE_SECRET_LEN    (65)
#define SIOIOT_PRODUCT_KEY_LEN      (12)


typedef struct sioiot_config_t{


char mqtt_host[SIOIOT_HOST_LEN];
int  mqtt_post;

char mqtt_id[SIOIOT_ID_LEN];
char mqtt_name[SIOIOT_NAME_LEN];
char mqtt_pass[SIOIOT_PASS_LEN];

char mqtt_pub_topic[SIOIOT_TOPIC_LEN];
char mqtt_sub_topic[SIOIOT_TOPIC_LEN];

char mqtt_pub_api_topic[SIOIOT_TOPIC_LEN];
char mqtt_sub_api_topic[SIOIOT_TOPIC_LEN];

char mqtt_ota_inform_topic[SIOIOT_TOPIC_LEN];           //设备版本号上报
char mqtt_ota_upgrade_topic[SIOIOT_TOPIC_LEN];          //接收云端固件升级通知
char mqtt_ota_progress_topic[SIOIOT_TOPIC_LEN];         //设备升级进度上报

char mqtt_shadow_update_topic[SIOIOT_TOPIC_LEN];        //设备影子上发
char mqtt_shadow_get_topic[SIOIOT_TOPIC_LEN];           //设备影子下发

}sioiot_config_t;

typedef struct sioiot_info{

    uint32_t timestamp;
    char device_secret[SIOIOT_DEVICE_SECRET_LEN];
    char device_name[SIOIOT_DEVICE_NAME_LEN];
    char product_key[SIOIOT_PRODUCT_KEY_LEN];

    char sioiot_info[SIOIOT_INFO_LEN];
    bool device_activate ;


}sioiot_info_t;

IOTStatus sioiot_init_device( void );



#endif


