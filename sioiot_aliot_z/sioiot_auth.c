#include "sioiot_auth.h"
#include "json_parser.h"
#include "http_short_connection.h"
#include "sioiot_tools.h"
#define auth_log(M, ...) custom_log("AUTH", M, ##__VA_ARGS__)

char* server_ssl_cert_str =
    "-----BEGIN CERTIFICATE-----\r\n\
MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n\
A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n\
b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n\
MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n\
YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n\
aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n\
jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n\
xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n\
1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n\
snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n\
U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n\
9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n\
BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n\
AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n\
yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n\
38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n\
AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n\
DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n\
HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n\
-----END CERTIFICATE-----";

#define AUTH_NUM_TOKENS 30

IOTStatus aioiot_mqtt_auth_info( sioiot_config_t * sioiot_mqtt_param, sioiot_info_t * sioiot_info )
{
    IOTStatus ret = IOTGeneralErr;
    uint8_t pass_array[32]={0};
     char device_id[DEVICE_ID_LEN];
    bzero( device_id, DEVICE_ID_LEN );
    static char device_buf[DEVICE_BUF_LEN];
    bzero( device_buf, DEVICE_BUF_LEN );

    ret = snprintf( sioiot_mqtt_param->mqtt_host, SIOIOT_HOST_LEN,
                    "%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",
                    sioiot_info->product_key );
    if ( (ret < 0) || (ret >= SIOIOT_HOST_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    sioiot_mqtt_param->mqtt_post=1883;

    ret = snprintf( device_id, DEVICE_ID_LEN, "%s.%s", sioiot_info->product_key, sioiot_info->device_name );
    if ( (ret < 0) || (ret >= DEVICE_ID_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( device_buf, DEVICE_BUF_LEN, "clientId%sdeviceName%sproductKey%stimestamp%d",
                       device_id,
                       sioiot_info->device_name, sioiot_info->product_key, activate_min_time );
    if ( (ret < 0) || (ret >= DEVICE_BUF_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }

    ret = snprintf( sioiot_mqtt_param->mqtt_id, SIOIOT_ID_LEN,
                    "%s|securemode=2,signmethod=hmacsha1,timestamp=%d|",
                    device_id,
                    activate_min_time );
    if ( (ret < 0) || (ret >= SIOIOT_ID_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }

    ret = snprintf( sioiot_mqtt_param->mqtt_name, SIOIOT_NAME_LEN, "%s&%s", sioiot_info->device_name,
                    sioiot_info->product_key );
    if ( (ret < 0) || (ret >= SIOIOT_NAME_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    //auth_log("1111111111111111111111111111:%s",device_buf);
    hmac(SHA1,( const unsigned char *)device_buf,strlen(device_buf),( const unsigned char *)sioiot_info->device_secret,strlen(sioiot_info->device_secret),pass_array);
    ret = sioiot_hex2str(pass_array,20,sioiot_mqtt_param->mqtt_pass,SIOIOT_PASS_LEN,true);

    //auth_log(":%s",sioiot_mqtt_param->mqtt_pass);
    if ( (ret < 0) || (ret >= SIOIOT_PASS_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }

    ret = snprintf( sioiot_mqtt_param->mqtt_pub_topic, SIOIOT_TOPIC_LEN, "/%s/%s/update",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_sub_topic, SIOIOT_TOPIC_LEN, "/%s/%s/get",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_pub_api_topic, SIOIOT_TOPIC_LEN, "/%s/%s/api/pub",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_sub_api_topic, SIOIOT_TOPIC_LEN, "/%s/%s/api/sub",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_shadow_update_topic, SIOIOT_TOPIC_LEN,
                    "/shadow/update/%s/%s",
                    sioiot_info->product_key, sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_shadow_get_topic, SIOIOT_TOPIC_LEN, "/shadow/get/%s/%s",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_ota_inform_topic, SIOIOT_TOPIC_LEN,
                    "/ota/device/inform/%s/%s",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_ota_progress_topic, SIOIOT_TOPIC_LEN,
                    "/ota/device/progress/%s/%s",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    ret = snprintf( sioiot_mqtt_param->mqtt_ota_upgrade_topic, SIOIOT_TOPIC_LEN,
                    "/ota/device/upgrade/%s/%s",
                    sioiot_info->product_key,
                    sioiot_info->device_name );
    if ( (ret < 0) || (ret >= SIOIOT_TOPIC_LEN) )
    {
        auth_log("set device info failed");
        return IOTGeneralErr;
    }
    return IOTNoErr;
}
