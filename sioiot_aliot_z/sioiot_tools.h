#ifndef _STOIOT_TOOLS_H_
#define _SIOIOT_TOOLS_H_

#include "sioiot_config.h"

#define BOOL_IS_TRUE(X)     ((X) == true ? "true" : "false")

/*typedef enum {
    SIOIOT_HTTP_POST,
    FOG_HTTP_GET,
    FOG_HTTP_PUT,
    SIOIOT_HTTP_METHOD_MAX,
    FOG_HTTP_METHOD_NONE
} SIOIOT_HTTP_METHOD_E;*/
bool sioiot_hex2str( uint8_t *hex, uint32_t hex_len, char *str, uint32_t str_buff_len,
                     bool is_capitals );
IOTStatus sioiot_get_uint_number();


IOTStatus sioiot_activate_name_get( char clientid[30],char *vendor_id,char *device_mac );
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
IOTStatus generate_http_common_request(char *requset_buff, uint32_t request_buff_len, int method, const char *uri, const char *host, const char *type,const char *http_body);

#endif
