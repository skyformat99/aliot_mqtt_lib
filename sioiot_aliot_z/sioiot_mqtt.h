#ifndef _SIOIOT_MQTT_H_
#define _SIOIOT_MQTT_H_

#include "sioiot_activate.h"
#include "sioiot_config.h"

extern  mico_queue_t mqtt_send_queue;
extern  mico_queue_t mqtt_recv_queue;
extern uint8_t mqtt_reconnect_change;


#define MAX_MQTT_DATA_SIZE          (1024)
#define MQTT_YIELD_TIMEOUT          (100)
#define MQTT_KEEPALIVE_TIME         (60)
#define MQTT_PACKET_TIMEOUT_MS      (20000)
#define MQTT_COMMAND_TIMEOUT_MS     (20000)
#define TLS_HANDSHAKE_TIMEOUT_MS    (5000)

#define MAX_MQTT_SEND_QUEUE_SIZE    (4)
#define MQTT_RECONNECT_SLEEP_TIME   (1)

typedef enum {
    //mqtt连接正常     影子初始化结束
    MQTT_CONNECT_SHADOW_INIT_END = 0,
    //mqtt连接不正常     影子无初始化
    MQTT_DISCONNECT_SHADOW_INIT_NO = 1,
    //mqtt连接正常     影子无初始化
    MQTT_CONNECT_SHADOW_INIT_NO = 2,
    //mqtt连接正常     影子初始化开始
    MQTT_CONNECT_SHADOW_INIT_START = 3,

}mqtt_change_t;
typedef enum {
    MQTT_PUB_TOPIC_T=1,
    MQTT_SUB_TOPIC_T=2,

    MQTT_PUB_API_TOPIC_T=3,
    MQTT_SUB_API_TOPIC_T=4,

    MQTT_SHADOW_UPDATE_TOPIC_T=5,
    MQTT_SHADOW_GET_TOPIC_T=6,

    MQTT_OTA_INFORM_TOPIC_T=7,
    MQTT_OTA_PROGRESS_TOPIC_T=8,
    MQTT_OTA_UPGRADE_TOPIC_T=9,

}mqtt_topic_num_t;

typedef struct {

    uint8_t mqtt_topic_num;
    char mqtt_recv_data[MAX_MQTT_DATA_SIZE];

} mqtt_recv_data_t;

typedef struct {

    uint8_t mqtt_topic_num;
    char mqtt_send_data[MAX_MQTT_DATA_SIZE];

} mqtt_send_data_t;

extern OSStatus sioiot_start_mqtt_sub_pub( sioiot_info_t * app_config );

#endif
