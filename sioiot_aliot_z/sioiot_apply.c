#include "../sioiot_aliot_z/sioiot_apply.h"

#include "../sioiot_aliot_z/sioiot_fota.h"
#include "../sioiot_aliot_z/sioiot_shadow.h"
#include "json_parser.h"

#define apply_log(M, ...) custom_log("APPLY", M, ##__VA_ARGS__)
extern mico_queue_t mqtt_send_queue;
extern mico_queue_t mqtt_recv_queue;
extern uint8_t mqtt_reconnect_change;

static mqtt_recv_data_t * mqtt_recv_data = NULL;
static mqtt_send_data_t * mqtt_send_data = NULL;
static char ota_version[OTA_VERSION_LEN];

void wifi_module_ota_state_callback( SIOIOT_OTA_STATE_E state, uint32_t sys_args )
{
    //apply_log( "[wifi module] state:%d, sys_args:%ld", state, sys_args );
    if ( SIOIOT_OTA_STATE_LOADING == state )
    {
        memset( mqtt_send_data->mqtt_send_data, 0, MAX_MQTT_DATA_SIZE );
        mqtt_send_data->mqtt_topic_num = MQTT_OTA_PROGRESS_TOPIC_T;
        sprintf( mqtt_send_data->mqtt_send_data,
                 "{\"id\": 1,\"params\":{\"step\":\"%ld\",\"desc\":\"%d\"}}", sys_args, state );
        mico_rtos_push_to_queue( &mqtt_send_queue, mqtt_send_data, 0 );
    }
    if ( SIOIOT_OTA_STATE_MD5_SUCCESS == state )
    {
        memset( mqtt_send_data->mqtt_send_data, 0, MAX_MQTT_DATA_SIZE );
        mqtt_send_data->mqtt_topic_num = MQTT_OTA_INFORM_TOPIC_T;
        sprintf( mqtt_send_data->mqtt_send_data, "{\"id\": 1,\"params\":{\"version\":\"%s\"}}",
                 ota_version );
        mico_rtos_push_to_queue( &mqtt_send_queue, mqtt_send_data, 1000 );
        mico_rtos_thread_sleep( 2 );
        MicoSystemReboot( );
    }else if(SIOIOT_OTA_STATE_MD5_FAILED == state){
        memset( mqtt_send_data->mqtt_send_data, 0, MAX_MQTT_DATA_SIZE );
        mqtt_send_data->mqtt_topic_num = MQTT_OTA_INFORM_TOPIC_T;
        sprintf( mqtt_send_data->mqtt_send_data, "{\"id\": 1,\"params\":{\"version\":\"%s\"}}",
                 MCU_VERSION );
        mico_rtos_push_to_queue( &mqtt_send_queue, mqtt_send_data, 1000 );
        mico_rtos_thread_sleep( 2 );
        MicoSystemReboot( );
    }
}

IOTStatus sioiot_ota_parser_data( char *ota_buf )
{
    IOTStatus err = kGeneralErr;

    char ota_message_status[20];
    char ota_url[MCU_OTA_URL_MAX_LEN];
    char ota_md5[MCU_OTA_MD5_MAX_LEN];

    jsontok_t ota_json_tokens[OTA_NUM_TOKENS];
    jobj_t ota_jobj;

    err = json_init( &ota_jobj, ota_json_tokens, OTA_NUM_TOKENS, ota_buf, strlen( ota_buf ) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "init ota_buf error!" );

    err = json_get_composite_object( &ota_jobj, "data" );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                           "get data error!" );
    memset( ota_url, 0, MCU_OTA_URL_MAX_LEN );
    err = json_get_val_str( &ota_jobj, "url", ota_url, sizeof(ota_url) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get url error!" );

    memset( ota_version, 0, sizeof(ota_version) );
    err = json_get_val_str( &ota_jobj, "version", ota_version, sizeof(ota_version) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get ota_version error!" );

    memset( ota_md5, 0, MCU_OTA_MD5_MAX_LEN );
    err = json_get_val_str( &ota_jobj, "md5", ota_md5, sizeof(ota_md5) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get md5 error!" );
    json_release_composite_object( &ota_jobj );

    memset( ota_message_status, 0, sizeof(ota_message_status) );
    err = json_get_val_str( &ota_jobj, "message", ota_message_status, sizeof(ota_message_status) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                           "get ota_message_status error!" );

    if ( strcmp( ota_message_status, "success" ) == IOTNoErr )
    {
        err = sioiot_module_ota( ota_url, ota_md5, wifi_module_ota_state_callback, true );
        if ( err != IOTNoErr )
        {
            apply_log("sioiot module ota error");
        }
    }
    exit:
    return err;
}

static void sioiot_mqtt_pub_apply( mico_thread_arg_t arg )
{
    while ( 1 )
    {
        //正常发送数据
        if ( mqtt_reconnect_change == MQTT_CONNECT_SHADOW_INIT_END )
        {
            mico_rtos_thread_msleep( 100 );
        }
        //关闭发送数据
        else if ( mqtt_reconnect_change == MQTT_DISCONNECT_SHADOW_INIT_NO )
        {
            mico_rtos_thread_msleep( 500 );
        }
        //发送get初始化影子
        else if ( mqtt_reconnect_change == MQTT_CONNECT_SHADOW_INIT_NO )
        {
            //发送影子初始化数据
            bzero( mqtt_send_data->mqtt_send_data, MAX_MQTT_DATA_SIZE );
            shadow_get( mqtt_send_data->mqtt_send_data, MAX_MQTT_DATA_SIZE );
            mqtt_send_data->mqtt_topic_num = MQTT_SHADOW_UPDATE_TOPIC_T;
            mico_rtos_push_to_queue( &mqtt_send_queue, mqtt_send_data, 0 );
            //发送OTA状态数据
            bzero( mqtt_send_data->mqtt_send_data, MAX_MQTT_DATA_SIZE );
            mqtt_send_data->mqtt_topic_num = MQTT_OTA_INFORM_TOPIC_T;
            sprintf( mqtt_send_data->mqtt_send_data, "{\"id\": 1,\"params\":{\"version\":\"%s\"} }",
                     MCU_VERSION );
            mico_rtos_push_to_queue( &mqtt_send_queue, mqtt_send_data, 0 );
            mqtt_reconnect_change = MQTT_CONNECT_SHADOW_INIT_START;

        }
        //等待影子初始化完成
        else if ( mqtt_reconnect_change == MQTT_CONNECT_SHADOW_INIT_START )
        {
            mico_rtos_thread_msleep( 100 );
        }
    }
}
static void sioiot_mqtt_sub_apply( mico_thread_arg_t arg )
{
    while ( 1 )
    {

        bzero( mqtt_recv_data->mqtt_recv_data, MAX_MQTT_DATA_SIZE );
        mico_rtos_pop_from_queue( &mqtt_recv_queue, mqtt_recv_data, MICO_WAIT_FOREVER );
        apply_log("[%d]:%s",mqtt_recv_data->mqtt_topic_num,mqtt_recv_data->mqtt_recv_data);
        switch ( mqtt_recv_data->mqtt_topic_num )
        {
                 //接收的数据
            case MQTT_SUB_TOPIC_T:

                break;
            case MQTT_SHADOW_GET_TOPIC_T:

                if ( mqtt_reconnect_change == MQTT_CONNECT_SHADOW_INIT_END )
                {

                }
                //关闭接收数据
                else if ( mqtt_reconnect_change == MQTT_DISCONNECT_SHADOW_INIT_NO )
                {
                    mico_rtos_thread_msleep( 100 );
                }
                //等待影子数据，数据接收成功后，置位标识
                else if ( mqtt_reconnect_change == MQTT_CONNECT_SHADOW_INIT_START )
                {
                    if ( shadow_parser_data(mqtt_recv_data->mqtt_recv_data,strlen(mqtt_recv_data->mqtt_recv_data))== IOTNoErr )
                    {
                        mqtt_reconnect_change = MQTT_CONNECT_SHADOW_INIT_END;
                    }
                }
                break;
            case MQTT_SUB_API_TOPIC_T:
                break;
            case MQTT_OTA_UPGRADE_TOPIC_T:
                sioiot_ota_parser_data( mqtt_recv_data->mqtt_recv_data );
                break;
        }
    }
}

OSStatus sioiot_apply_shread_init( sioiot_config_t * mqtt_config )
{

    OSStatus err;
    uint32_t stack_size = 0x2000;
    if ( NULL == (mqtt_send_data = malloc( sizeof(mqtt_send_data_t) )) )
    {
        apply_log("Error mqtt send data malloc");

    }
    if ( NULL == (mqtt_recv_data = malloc( sizeof(mqtt_recv_data_t) )) )
    {
        apply_log("Error mqtt recv data malloc");
    }

    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "sioiot_mqtt_pub_apply",
                                   sioiot_mqtt_pub_apply,
                                   stack_size,
                                   (uint32_t) mqtt_config );
    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "sioiot_mqtt_sub_apply",
                                   sioiot_mqtt_sub_apply,
                                   stack_size,
                                   (uint32_t) mqtt_config );
    return err;
}
