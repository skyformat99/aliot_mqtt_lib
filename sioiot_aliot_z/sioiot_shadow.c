#include "../sioiot_aliot_z/sioiot_shadow.h"
#define shadow_log(M, ...) custom_log("shadow", M, ##__VA_ARGS__)

#define NUM_TOKENSs 180
uint32_t timestamp = 0;
int shadow_parser_data( char* recv_buf, unsigned int buf_len )
{
    jsontok_t ajson_tokens[NUM_TOKENSs];
    jobj_t ajobj;
    OSStatus err = kGeneralErr;
    char method[10] = { '\0' };
    char status[10] = { '\0' };
    char errorcode[5] = { '\0' };
    int version = 0;
    long long timestamp64 = 0;

    err = json_init( &ajobj, ajson_tokens, NUM_TOKENSs, recv_buf, strlen( recv_buf ) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "init recv error!" );
    shadow_log( "json_str:%s", recv_buf );

    memset( method, 0, sizeof(method) );
    err = json_get_val_str( &ajobj, "method", method, sizeof(method) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get method error!" );
    if ( strncmp( method, "reply", strlen( "reply" ) ) == 0 )
    {
        err = json_get_composite_object( &ajobj, "payload" );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get payload error!" );

        memset( status, 0, sizeof(status) );
        err = json_get_val_str( &ajobj, "status", status, sizeof(status) );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get status error!" );
        if ( strncmp( status, "success", strlen( "success" ) ) == 0 )
        {
            err = json_get_composite_object( &ajobj, "state" );
            require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                   "get state error!" );
            if ( err == WM_SUCCESS )
            {
                err = json_get_composite_object( &ajobj, "desired" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get state desired error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "111111111");
                    json_release_composite_object( &ajobj );
                }

                err = json_get_composite_object( &ajobj, "reported" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get state reported error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "2222222");
                    json_release_composite_object( &ajobj );
                }

                json_release_composite_object( &ajobj );
            }
            err = json_get_composite_object( &ajobj, "metadata" );
            require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                   "get metadata error!" );
            if ( err == WM_SUCCESS )
            {
                err = json_get_composite_object( &ajobj, "desired" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get metadata desired error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "3333333333");
                    json_release_composite_object( &ajobj );
                }

                err = json_get_composite_object( &ajobj, "reported" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get metadata reported error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "444444444");
                    json_release_composite_object( &ajobj );
                }
                json_release_composite_object( &ajobj );
            }
        }
        else if ( strncmp( status, "error", strlen( "error" ) ) == 0 )
        {
            err = json_get_composite_object( &ajobj, "content" );
            require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                                   "get content error!" );
            memset( errorcode, 0, sizeof(errorcode) );
            err = json_get_val_str( &ajobj, "errorcode", errorcode, sizeof(errorcode) );
            require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr,
                                   "get errorcode error!" );
            if ( strncmp( errorcode, "407", strlen( "407" ) ) == 0 )
            {
                version = 0;
            }
            json_release_composite_object( &ajobj );
        }

        json_release_composite_object( &ajobj );
    }
    else if ( strncmp( method, "control", strlen( "control" ) ) == 0 )
    {
        err = json_get_composite_object( &ajobj, "payload" );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get payload error!" );

        memset( status, 0, sizeof(status) );
        err = json_get_val_str( &ajobj, "status", status, sizeof(status) );
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get status error!" );
        if ( strncmp( status, "success", strlen( "success" ) ) == 0 )
        {
            err = json_get_composite_object( &ajobj, "state" );
            require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                   "get state error!" );
            if ( err == WM_SUCCESS )
            {
                err = json_get_composite_object( &ajobj, "desired" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get state desired error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "111111111");
                    json_release_composite_object( &ajobj );
                }

                err = json_get_composite_object( &ajobj, "reported" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get state reported error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "2222222");
                    json_release_composite_object( &ajobj );
                }

                json_release_composite_object( &ajobj );
            }
            err = json_get_composite_object( &ajobj, "metadata" );
            require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                   "get metadata error!" );
            if ( err == WM_SUCCESS )
            {
                err = json_get_composite_object( &ajobj, "desired" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get metadata desired error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "3333333333");
                    json_release_composite_object( &ajobj );
                }

                err = json_get_composite_object( &ajobj, "reported" );
                require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                                       "get metadata reported error!" );
                if ( err == WM_SUCCESS )
                {
                    shadow_log( "444444444");
                    json_release_composite_object( &ajobj );
                }
                json_release_composite_object( &ajobj );
            }
        }

        json_release_composite_object( &ajobj );

    }
    err = json_get_val_int( &ajobj, "version", &version );
    require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                           "get version error!" );
    err = json_get_val_int64( &ajobj, "timestamp", &timestamp64 );
    require_action_string( err == WM_SUCCESS||err==-1006, exit, err = kGeneralErr,
                           "get timestamp error!" );
    if(err == WM_SUCCESS){
        timestamp = timestamp64;
    }

    shadow_log("%s|%s|%s",method,status,errorcode);

    shadow_log( "%d::%d",version,err);
    printf( "%ld",timestamp);

    exit:
    return err;
}

//影子数据获取命令
IOTStatus shadow_get( char* send_buf, unsigned int buf_len )
{

    bzero( send_buf, buf_len );
    char * shadow_get_buf = "{\"method\":\"get\"}";
    if ( buf_len > strlen( shadow_get_buf ) )
    {
        strcpy( send_buf, shadow_get_buf );
        return IOTNoErr;
    } else
    {
        shadow_log( "shadow get error");
        return IOTGeneralErr;
    }
}
