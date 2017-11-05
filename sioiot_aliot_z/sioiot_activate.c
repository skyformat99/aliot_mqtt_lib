#include "sioiot_activate.h"

#include "json_parser.h"
#include "http_short_connection.h"
#include "sioiot_auth.h"
#include "sioiot_mqtt.h"
#include "sioiot_tools.h"

#define activate_log(M, ...) custom_log("activate_log", M, ##__VA_ARGS__)

static IOTStatus parser_activate_data( mico_Context_t *context, sioiot_info_t * sioiot_info,
                                       char * recv_buf )
{   // Define an array of JSON tokens
    jsontok_t json_tokens[NUM_TOKENS];
    jobj_t jobj;
    long long timestamp = 0;

    OSStatus err = kGeneralErr;
    require_string( recv_buf != NULL, exit, "body is NULL!!!" );

    require_string( ((*recv_buf == '{') && (*(recv_buf + strlen( recv_buf ) - 1) == '}')), exit,
                    "http body JSON format error" );

    // Initialise the JSON parser and parse the given string
    err = json_init( &jobj, json_tokens, NUM_TOKENS, recv_buf, strlen( recv_buf ) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "parse error!" );

    err = json_get_val_int64( &jobj, "timestamp", &timestamp );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get timestamp error!" );
    uint32_t time_stamp = (timestamp / 1000);
    if ( time_stamp > aliot_my_time )
    {

        memset( sioiot_info->product_key, 0, sizeof(sioiot_info->product_key) );
        err = json_get_val_str( &jobj, "product_key", sioiot_info->product_key, sizeof(sioiot_info->product_key) );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                               "get product_key error!" );
        activate_log( "product_key: %s", sioiot_info->product_key );

        memset( sioiot_info->device_name, 0, sizeof(sioiot_info->device_name) );
        err = json_get_val_str( &jobj, "device_name", sioiot_info->device_name, sizeof(sioiot_info->device_name) );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                               "get device_name error!" );
        activate_log( "device_name: %s", sioiot_info->device_name );

        memset( sioiot_info->device_secret, 0, sizeof(sioiot_info->device_secret) );
        err = json_get_val_str( &jobj, "device_secret", sioiot_info->device_secret, sizeof(sioiot_info->device_secret) );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                               "get device_secret error!" );
        activate_log( "device_secret: %s", sioiot_info->device_secret );
        sioiot_info->timestamp=time_stamp;
        if ( sioiot_info->device_name != NULL )
        {
            if ( err != IOTNoErr )
            {
                activate_log( "aliot_auth_mqtt error" );
                sioiot_info->device_activate = false;
            } else
            {
                sioiot_info->device_activate = true;
            }
            mico_system_context_update( context );

        } else
        {
            if ( sioiot_info->device_activate != false )
            {
                sioiot_info->device_activate = false;
            }
            mico_system_context_update( context );

        }

    }
    else
    {
        activate_log( "https post recv errno:%d", errno );
    }
    exit:
    return err;
}

static IOTStatus sioiot_https_activate( mico_Context_t *context, sioiot_config_t * sioiot_info )
{
    OSStatus err = kGeneralErr;
    HTTP_REQ_S https_req = HTTP_REQ_INIT_PARAM;
    const char *http_body_str = "{\"product_name\":\"%s\",\"mac\":\"%s\",\"batch_number\":\"%s\"}";
    char http_body_buff[512] = { 0 };
    char device_name[30] = { '\0' };
    char device_mac[20] = { '\0' };
    sioiot_activate_name_get( device_name, VENDOR_ID, device_mac );
    memset( &https_req, 0, sizeof(https_req) );

    https_req.domain_name = activate_host;
    https_req.port = activate_port;
    https_req.is_success = false;
    https_req.timeout_ms = activate_timeout;

    https_req.http_req = malloc( activate_buf_len );
    require_string( https_req.http_req != NULL, exit, "malloc error" );
    memset( https_req.http_req, 0, activate_buf_len );

    https_req.http_res = malloc( activate_buf_len );
    require_string( https_req.http_res != NULL, exit, "malloc error" );
    memset( https_req.http_res, 0, activate_buf_len );
    https_req.res_len = activate_buf_len;

    sprintf( http_body_buff, http_body_str, product_name, device_mac, batch_number );

    generate_http_common_request( https_req.http_req, activate_buf_len, activate_mode,
                                  activate_url,
                                  activate_host,
                                  activate_type,
                                  http_body_buff );

    https_req.req_len = strlen( https_req.http_req );
    activate_log( "http request:\r\n%s", https_req.http_req );

    err = http_short_connection_ssl( &https_req );
    require_noerr_string( err, exit, "sioiot_activate_after_entry error!" );

    require_action_string( https_req.is_success == true, exit, err = kGeneralErr,
                           "https request error!" );

    activate_log( "http request:\r\n%s", https_req.http_res );

    parser_activate_data( context, sioiot_info, https_req.http_res );

    exit:
    //free request buff
    if ( https_req.http_req != NULL )
    {
        free( https_req.http_req );
        https_req.http_req = NULL;
    }

    //free response buff
    if ( https_req.http_res != NULL )
    {
        free( https_req.http_res );
        https_req.http_res = NULL;
    }
    return err;
}

static IOTStatus sioiot_init_info( mico_Context_t *context, sioiot_info_t * sioiot_info )
{
    IOTStatus err = IOTGeneralErr;

    sioiot_info->device_activate = false;
    sioiot_info->timestamp=0;
    bzero( sioiot_info->sioiot_info, SIOIOT_INFO_LEN );
    bzero( sioiot_info->device_secret, SIOIOT_DEVICE_SECRET_LEN );
    bzero( sioiot_info->device_name, SIOIOT_DEVICE_NAME_LEN );
    bzero( sioiot_info->product_key, SIOIOT_PRODUCT_KEY_LEN );
    strcpy( sioiot_info->sioiot_info, VENDOR_ID );

    err = mico_system_context_update( context );
    if ( err == IOTNoErr )
    {
        MicoSystemReboot( );
    }
    return err;
}
void ceshi( mico_Context_t *context, sioiot_info_t * sioiot_info )
{
    IOTStatus err = IOTGeneralErr;
    strcpy( sioiot_info->device_name, "lqq00001" );//设备名
    strcpy( sioiot_info->device_secret, "4JD1sqhwABh4OyY7NAM5vxB9EVhvYZhH" );//密码
    strcpy( sioiot_info->product_key, "l9B3yP5fiP1" );//产品好
    sioiot_info->timestamp=1502694490;
    sioiot_info->device_activate = true;
    mico_system_context_update( context );
    if ( err == IOTNoErr )
    {
        MicoSystemReboot( );
    }
}

IOTStatus sioiot_init_device( void )
{
    mico_Context_t *context = NULL;
    sioiot_info_t * sioiot_info = NULL;
    context = mico_system_context_get( );
    sioiot_info = mico_system_context_get_user_data( context );

    activate_log("%d",sioiot_info->device_activate);
    activate_log("%s",sioiot_info->sioiot_info);

    if ( strcmp( sioiot_info->sioiot_info, VENDOR_ID ) == 0 )
    {
        /* Wait for wlan connection*/
        mico_rtos_get_semaphore( &wait_sem, MICO_WAIT_FOREVER );
        activate_log("wifi connected successful");

        if ( sioiot_info->device_activate == true )
        {


            sioiot_start_mqtt_sub_pub( sioiot_info );
        }
        else
        {
            //默认为测试模式，如果配置了激活模式可以开启下面的激活demo、

            ceshi( context, sioiot_info );
            //sioiot_https_activate( context, sioiot_info );

            mico_rtos_thread_sleep( 1 );
            MicoSystemReboot( );

        }
    }
    else
    {
        sioiot_init_info( context, sioiot_info );
    }
    return IOTNoErr;
}
