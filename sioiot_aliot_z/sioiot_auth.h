#ifndef _SIOIOT_AUTH_H_
#define _SIOIOT_AUTH_H_

#include "sioiot_activate.h"
#include "sioiot_config.h"


#define  DEVICE_ID_LEN       (64)
#define  DEVICE_BUF_LEN          (128)
#define  SIGN_SOURCE_LEN     (256)
#define  HTTP_POST_MAX_LEN   (1024)

extern char* server_ssl_cert_str;



IOTStatus aioiot_mqtt_auth_info( sioiot_config_t * sioiot_mqtt_param, sioiot_info_t * sioiot_info );
IOTStatus aliot_auth_mqtt( sioiot_config_t * app_config,char *device_secret, char *device_name, char *product_key, unsigned int timestamp );



#endif

