#include "sioiot_tools.h"

//param: method + uri + host + body_len + body
#define COMMON_HTTP_REQUSET      \
("%s %s HTTP/1.1\r\n\
Host: %s\r\n\
Content-Type: %s\r\n\
Connection: Close\r\n\
Content-Length: %d\r\n\r\n%s")

const char *http_method_string[3] =
{
    "POST",
    "GET",
    "PUT",
};

/**
 * @brief  generate http common request
 * @param  requset_buff[in/out]: user buff
 * @param  request_buff_len[in]: user buff length
 * @param  method[in]: http method
 * @param  uri[in]: http uri
 * @param  host[in]: http host
 * @param  http_body[in]: http body string
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
IOTStatus generate_http_common_request(char *requset_buff, uint32_t request_buff_len, int method, const char *uri, const char *host, const char *type,const char *http_body)
{
    IOTStatus err = IOTGeneralErr;

    require_string(requset_buff != NULL && uri != NULL && host != NULL && http_body != NULL&&type !=NULL, exit, "param error");
    require_string(method < 3, exit, "method error");
    require_string((strlen(http_body) + 384) < request_buff_len, exit, "param error");

    memset(requset_buff, 0, request_buff_len);

    sprintf(requset_buff, COMMON_HTTP_REQUSET, http_method_string[method], uri, host,type, strlen(http_body), http_body);

    err = IOTNoErr;

    exit:
    return err;
}
bool sioiot_hex2str( uint8_t *hex, uint32_t hex_len, char *str, uint32_t str_buff_len,
                     bool is_capitals )
{
    uint32_t i = 0;

    if ( hex_len * 2 > str_buff_len )
    {
        return false;
    }

    memset( str, 0, str_buff_len );

    for ( i = 0; i < hex_len; i++ )
    {
        if ( is_capitals == true )
        {
            sprintf( str + i * 2, "%02X", hex[i] );
        } else
        {
            sprintf( str + i * 2, "%02x", hex[i] );
        }
    }

    return true;
}
IOTStatus sioiot_get_uint_number()
{
    uint32_t rand_number,err;
    err = MicoRandomNumberRead( &rand_number, 3);
    if(err == IOTNoErr){
       return rand_number*mico_rtos_get_time()%1000000000;
    }
    else{
        return 0;
    }

}
IOTStatus sioiot_activate_name_get( char clientid[],char *vendor_id,char *device_mac )
{
    uint8_t mac[6];

    mico_wlan_get_mac_address( mac );
    sprintf( device_mac, "%02X%02X%02X%02X%02X%02X",
             mac[0],
             mac[1], mac[2], mac[3], mac[4], mac[5] );
    sprintf( clientid, "%s-%s",vendor_id, device_mac );

    return IOTNoErr;
}


