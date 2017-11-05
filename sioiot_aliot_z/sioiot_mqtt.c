#include "sioiot_mqtt.h"

#include "mqtt_client_interface.h"
#include "sioiot_apply.h"
#include "sioiot_auth.h"

#define mqtt_log(M, ...) custom_log("mqtt", M, ##__VA_ARGS__)

mico_queue_t mqtt_send_queue = NULL;
mico_queue_t mqtt_recv_queue = NULL;
mico_mutex_t mqtt_recv_mutex = NULL;

uint8_t mqtt_reconnect_change = MQTT_DISCONNECT_SHADOW_INIT_NO;
sioiot_config_t *mqtt_config;

static mqtt_recv_data_t * mqtt_recv_data = NULL;
static mqtt_send_data_t * mqtt_send_data = NULL;


void disconnectCallbackHandler( MQTT_Client *pClient, void *data )
{
    IoT_Error_t rc = MQTT_FAILURE;
    mqtt_log("MQTT Disconnect");
    mqtt_reconnect_change = MQTT_DISCONNECT_SHADOW_INIT_NO;
    if ( NULL == pClient )
    {
        return;
    }
    aioiot_mqtt_auth_info(mqtt_config,data);

    if ( mqtt_is_autoreconnect_enabled( pClient ) )
    {
        mqtt_log("Auto Reconnect is enabled, Reconnecting attempt will start now");
    } else
    {
        mqtt_log("Auto Reconnect not enabled. Starting manual reconnect...");
        rc = mqtt_attempt_reconnect( pClient );
        if ( NETWORK_RECONNECTED == rc )
        {
            mqtt_log("Manual Reconnect Successful");
        } else
        {
            mqtt_log("Manual Reconnect Failed - %d", rc);
        }
    }
}
#ifdef SUB_TOPIC_EN
void iot_sub_callback_handler( MQTT_Client *pClient, char *topicName,
                               uint16_t topicNameLen,
                               IoT_Publish_Message_Params *params,
                               void *pData )
{

    int err;
    IOT_UNUSED( pData );
    IOT_UNUSED( pClient );
    err = mico_rtos_lock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex lock error");
    }
    mqtt_recv_data->mqtt_topic_num = MQTT_SUB_TOPIC_T;
    bzero( mqtt_recv_data->mqtt_recv_data, MAX_MQTT_DATA_SIZE );
    strncpy( mqtt_recv_data->mqtt_recv_data, (char*) params->payload, params->payloadLen );
    err = mico_rtos_push_to_queue( &mqtt_recv_queue, mqtt_recv_data, 0 );

    //mqtt_log("mqtt_recv_data_send_queue error=%d,data=%s",err,(char*) params->payload);
    if ( err != 0 )
    {
        mqtt_log("mqtt push to queue error");
    }
    err = mico_rtos_unlock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex unlock error");
    }
}
#endif
#ifdef SUB_API_TOPIC_EN
void iot_sub_api_callback_handler( MQTT_Client *pClient, char *topicName,
                                   uint16_t topicNameLen,
                                   IoT_Publish_Message_Params *params,
                                   void *pData )
{
    int err;
    IOT_UNUSED( pData );
    IOT_UNUSED( pClient );
    err = mico_rtos_lock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex lock error");
    }
    mqtt_recv_data->mqtt_topic_num = MQTT_SUB_API_TOPIC_T;
    bzero( mqtt_recv_data->mqtt_recv_data, MAX_MQTT_DATA_SIZE );
    strncpy( mqtt_recv_data->mqtt_recv_data, (char*) params->payload, params->payloadLen );
    err = mico_rtos_push_to_queue( &mqtt_recv_queue, mqtt_recv_data, 0 );

    //mqtt_log("mqtt_recv_data_send_queue error=%d,data=%s",err,(char*) params->payload);
    if ( err != 0 )
    {
        mqtt_log("mqtt push to queue error");
    }
    err = mico_rtos_unlock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex unlock error");
    }
}
#endif
#ifdef SHADOW_TOPIC_EN
void iot_shadow_get_callback_handler( MQTT_Client *pClient, char *topicName,
                                      uint16_t topicNameLen,
                                      IoT_Publish_Message_Params *params,
                                      void *pData )
{
    int err;
    IOT_UNUSED( pData );
    IOT_UNUSED( pClient );
    err = mico_rtos_lock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex lock error");
    }
    mqtt_recv_data->mqtt_topic_num = MQTT_SHADOW_GET_TOPIC_T;
    bzero( mqtt_recv_data->mqtt_recv_data, MAX_MQTT_DATA_SIZE );
    strncpy( mqtt_recv_data->mqtt_recv_data, (char*) params->payload, params->payloadLen );
    err = mico_rtos_push_to_queue( &mqtt_recv_queue, mqtt_recv_data, 0 );

    //mqtt_log("mqtt_recv_data_send_queue error=%d,data=%s",err,(char*) params->payload);
    if ( err != 0 )
    {
        mqtt_log("mqtt push to queue error");
    }
    err = mico_rtos_unlock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex unlock error");
    }
}
#endif
#ifdef OTA_TOPIC_EN

void iot_ota_upgrade_callback_handler( MQTT_Client *pClient, char *topicName,
                                       uint16_t topicNameLen,
                                       IoT_Publish_Message_Params *params,
                                       void *pData )
{
    int err;
    IOT_UNUSED( pData );
    IOT_UNUSED( pClient );
    err = mico_rtos_lock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex lock error");
    }
    mqtt_recv_data->mqtt_topic_num = MQTT_OTA_UPGRADE_TOPIC_T;
    bzero( mqtt_recv_data->mqtt_recv_data, MAX_MQTT_DATA_SIZE );
    strncpy( mqtt_recv_data->mqtt_recv_data, (char*) params->payload, params->payloadLen );
    err = mico_rtos_push_to_queue( &mqtt_recv_queue, mqtt_recv_data, 0 );

    //mqtt_log("mqtt_recv_data_send_queue error=%d,data=%s",err,(char*) params->payload);
    if ( err != 0 )
    {
        mqtt_log("mqtt push to queue error");
    }
    err = mico_rtos_unlock_mutex( &mqtt_recv_mutex );
    if ( err != kNoErr )
    {
        mqtt_log("mqtt_recv_mutex unlock error");
    }
}
#endif

static IoT_Error_t sioiot_mqtt_subscribe( MQTT_Client * client, sioiot_config_t *mqtt_config )
{

    IoT_Error_t rc = MQTT_FAILURE;
#ifdef SUB_TOPIC_EN
    rc = mqtt_subscribe( client, mqtt_config->mqtt_sub_topic, strlen( mqtt_config->mqtt_sub_topic ),
                         QOS1,
                         iot_sub_callback_handler,
                         NULL );
    require_string( MQTT_SUCCESS == rc, exit, "sioiot mqtt subscribe sub error!" );
#endif
#ifdef SUB_API_TOPIC_EN
    rc = mqtt_subscribe( client, mqtt_config->mqtt_sub_api_topic,
                         strlen( mqtt_config->mqtt_sub_api_topic ),
                         QOS1,
                         iot_sub_api_callback_handler,
                         NULL );
    require_string( MQTT_SUCCESS == rc, exit, "sioiot mqtt subscribe sub api error!" );
#endif
#ifdef SHADOW_TOPIC_EN
    rc = mqtt_subscribe( client, mqtt_config->mqtt_shadow_get_topic,
                         strlen( mqtt_config->mqtt_shadow_get_topic ),
                         QOS1,
                         iot_shadow_get_callback_handler,
                         NULL );
    require_string( MQTT_SUCCESS == rc, exit, "sioiot mqtt subscribe sub error!" );
#endif
#ifdef OTA_TOPIC_EN
    rc = mqtt_subscribe( client, mqtt_config->mqtt_ota_upgrade_topic,
                         strlen( mqtt_config->mqtt_ota_upgrade_topic ),
                         QOS1,
                         iot_ota_upgrade_callback_handler,
                         NULL );
    require_string( MQTT_SUCCESS == rc, exit, "sioiot mqtt subscribe sub error!" );
#endif
    exit:
    return rc;
}
static void sioiot_mqtt_sub_pub_main( mico_thread_arg_t arg )
{
    IoT_Error_t rc = MQTT_FAILURE;

    sioiot_info_t * sioiot_info = (sioiot_info_t *) arg;


    if ( NULL == (mqtt_config = malloc( sizeof(sioiot_config_t) )) )
    {
        mqtt_log("Error mqtt_config malloc");
        goto exit;
    }
    aioiot_mqtt_auth_info(mqtt_config,sioiot_info);
    sioiot_apply_shread_init( mqtt_config );
    mqtt_log("%s",mqtt_config->mqtt_id);
    mqtt_log("%s",mqtt_config->mqtt_name);
    mqtt_log("%s",mqtt_config->mqtt_pass);
    mqtt_log("%s",mqtt_config->mqtt_host);
    mqtt_log("%d",mqtt_config->mqtt_post);
    mqtt_log("%s",mqtt_config->mqtt_pub_topic);
    mqtt_log("%s",mqtt_config->mqtt_sub_topic);
    mqtt_log("%s",mqtt_config->mqtt_pub_api_topic);
    mqtt_log("%s",mqtt_config->mqtt_sub_api_topic);
    mqtt_log("%s",mqtt_config->mqtt_ota_inform_topic);
    mqtt_log("%s",mqtt_config->mqtt_ota_progress_topic);
    mqtt_log("%s",mqtt_config->mqtt_ota_upgrade_topic);
    mqtt_log("%s",mqtt_config->mqtt_shadow_get_topic);
    mqtt_log("%s",mqtt_config->mqtt_shadow_update_topic);

    if ( NULL == (mqtt_send_data = malloc( sizeof(mqtt_send_data_t) )) )
    {
        mqtt_log("Error mqtt send data malloc");
        goto exit;
    }
    if ( NULL == (mqtt_recv_data = malloc( sizeof(mqtt_recv_data_t) )) )
    {
        mqtt_log("Error mqtt recv data malloc");
        goto exit;
    }

    MQTT_Client client;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
    IoT_Publish_Message_Params paramsQOS1;

    mqttInitParams.enableAutoReconnect = false;
    mqttInitParams.pHostURL = mqtt_config->mqtt_host;
    mqttInitParams.port = mqtt_config->mqtt_post;
    mqttInitParams.mqttPacketTimeout_ms = MQTT_PACKET_TIMEOUT_MS;
    mqttInitParams.mqttCommandTimeout_ms = MQTT_COMMAND_TIMEOUT_MS;
    mqttInitParams.tlsHandshakeTimeout_ms = TLS_HANDSHAKE_TIMEOUT_MS;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = sioiot_info;

    mqttInitParams.pRootCALocation = server_ssl_cert_str;
    mqttInitParams.pDeviceCertLocation = NULL;
    mqttInitParams.pDevicePrivateKeyLocation = NULL;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.isClientnameVerify = false;
    mqttInitParams.isUseSSL = true;

    rc = mqtt_init( &client, &mqttInitParams );
    if ( MQTT_SUCCESS != rc )
    {
        mqtt_log("ali_iot_mqtt_init returned error : %d ", rc);

        goto exit;
    }
    connectParams.keepAliveIntervalInSec = MQTT_KEEPALIVE_TIME;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = mqtt_config->mqtt_id;
    connectParams.clientIDLen = (uint16_t) strlen( mqtt_config->mqtt_id );
    connectParams.isWillMsgPresent = false;
    connectParams.pUsername = mqtt_config->mqtt_name;
    connectParams.usernameLen = strlen( mqtt_config->mqtt_name );
    connectParams.pPassword = mqtt_config->mqtt_pass;
    connectParams.passwordLen = strlen( mqtt_config->mqtt_pass );

    mqtt_log("Connecting...");

    while ( 1 )
    {
        rc = mqtt_connect( &client, &connectParams );
        if ( MQTT_SUCCESS == rc )
        {
            break;
        }
        mqtt_log("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
        mico_rtos_thread_sleep( MQTT_RECONNECT_SLEEP_TIME * 5 );
    }
    mqtt_log("OK!");

    mqtt_log("Subscribing...");
    while ( 1 )
    {
        rc = sioiot_mqtt_subscribe( &client, mqtt_config );
        if ( MQTT_SUCCESS == rc )
        {
            break;
        }
        mqtt_log("Error subscribing : %d ", rc);
        mico_rtos_thread_sleep( MQTT_RECONNECT_SLEEP_TIME * 2 );
    }
    mqtt_log("OK!");

    mqtt_reconnect_change = MQTT_CONNECT_SHADOW_INIT_NO;

    while ( 1 )
    {
        //Max time the yield function will wait for read messages
        rc = mqtt_yield( &client, MQTT_YIELD_TIMEOUT );
        if ( NETWORK_ATTEMPTING_RECONNECT == rc )
        {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            mico_rtos_thread_sleep( MQTT_RECONNECT_SLEEP_TIME );
            continue;
        } else if ( NETWORK_RECONNECTED == rc )
        {
            mqtt_log("Reconnect Successful");
            mqtt_reconnect_change = MQTT_CONNECT_SHADOW_INIT_NO;
        }
        while ( mico_rtos_is_queue_empty( &mqtt_send_queue ) == false )
        {

            bzero( mqtt_send_data->mqtt_send_data, MAX_MQTT_DATA_SIZE );
            rc = mico_rtos_pop_from_queue( &mqtt_send_queue, mqtt_send_data, 0 );
 //           mqtt_log("publish...");
//            mqtt_log("[%d]:%s",mqtt_send_data->mqtt_topic_num,mqtt_send_data->mqtt_send_data);
            paramsQOS1.qos = QOS1;
            paramsQOS1.payload = (void *) mqtt_send_data->mqtt_send_data;
            paramsQOS1.payloadLen = strlen( mqtt_send_data->mqtt_send_data );
            paramsQOS1.isRetained = false;
            switch ( mqtt_send_data->mqtt_topic_num )
            {
#ifdef SUB_TOPIC_EN
                case MQTT_PUB_TOPIC_T:
                    rc = mqtt_publish( &client, mqtt_config->mqtt_pub_topic,
                                       strlen( mqtt_config->mqtt_pub_topic ),
                                       &paramsQOS1 );
                    break;
#endif
#ifdef SUB_API_TOPIC_EN
                case MQTT_PUB_API_TOPIC_T:
                    rc = mqtt_publish( &client, mqtt_config->mqtt_pub_api_topic,
                                       strlen( mqtt_config->mqtt_pub_api_topic ),
                                       &paramsQOS1 );
                    break;
#endif
#ifdef SHADOW_TOPIC_EN
                case MQTT_SHADOW_UPDATE_TOPIC_T:
                    rc = mqtt_publish( &client, mqtt_config->mqtt_shadow_update_topic,
                                       strlen( mqtt_config->mqtt_shadow_update_topic ),
                                       &paramsQOS1 );
                    break;
#endif
#ifdef OTA_TOPIC_EN
                case MQTT_OTA_INFORM_TOPIC_T:
                    rc = mqtt_publish( &client, mqtt_config->mqtt_ota_inform_topic,
                                       strlen( mqtt_config->mqtt_ota_inform_topic ),
                                       &paramsQOS1 );
                    break;
                case MQTT_OTA_PROGRESS_TOPIC_T:
                    rc = mqtt_publish( &client, mqtt_config->mqtt_ota_progress_topic,
                                       strlen( mqtt_config->mqtt_ota_progress_topic ),
                                       &paramsQOS1 );
                    break;
                default:
                    break;
#endif
            }
            if ( rc == MQTT_REQUEST_TIMEOUT_ERROR )
            {
                mqtt_log("sioiot publish ack not received");
            }
        }

    }
    exit:
    mqtt_disconnect( &client );
    mico_rtos_deinit_mutex( &mqtt_recv_mutex );
    mico_rtos_deinit_queue( &mqtt_send_queue );
    mico_rtos_deinit_queue( &mqtt_recv_queue );
    mico_rtos_delete_thread( NULL );
}

OSStatus sioiot_start_mqtt_sub_pub( sioiot_info_t * device_info )
{


    OSStatus err;
    uint32_t stack_size = 0x3000;
    err = mico_rtos_init_mutex( &mqtt_recv_mutex );

    err = mico_rtos_init_queue( &mqtt_send_queue, "mqtt_send_queue", sizeof(mqtt_send_data_t),
    MAX_MQTT_SEND_QUEUE_SIZE );

    err = mico_rtos_init_queue( &mqtt_recv_queue, "mqtt_recv_queue", sizeof(mqtt_send_data_t),
    MAX_MQTT_SEND_QUEUE_SIZE );

    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "mqtt", sioiot_mqtt_sub_pub_main,
                                   stack_size,
                                   (uint32_t) device_info );

    return err;
}
