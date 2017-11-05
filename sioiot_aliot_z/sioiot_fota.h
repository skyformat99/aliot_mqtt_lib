#ifndef _SIOIOT_FOTA_H_
#define _SIOIOT_FOTA_H_

#include "../sioiot_aliot_z/sioiot_config.h"

#define MCU_V3_OTA_BIN_BUFF_LEN                     (2048)
#define MCU_OTA_URL_MAX_LEN                         (1024)
#define MCU_OTA_MD5_MAX_LEN                         (64)
#define MCU_OTA_DEBUG_ENABLE                        (1)
typedef enum {
    SIOIOT_OTA_STATE_START,
    SIOIOT_OTA_STATE_LOADING,
    SIOIOT_OTA_STATE_FAILED,
    SIOIOT_OTA_STATE_FAILED_AND_RETRY,
    SIOIOT_OTA_STATE_FILE_DOWNLOAD_SUCCESS,
    SIOIOT_OTA_STATE_MD5_SUCCESS,
    SIOIOT_OTA_STATE_MD5_FAILED,
    SIOIOT_OTA_STATE_MAX,
    SIOIOT_OTA_STATE_NONE
}SIOIOT_OTA_STATE_E; //ota state

typedef void (*SIOIOT_OTA_STATE_CB) (SIOIOT_OTA_STATE_E state, uint32_t sys_args);

/**
 * @brief  sioiot module ota function
 * @param  ota_url[in]: module ota url,sopport http and https.
 * @param  ota_file_md5[in]: ota file md5.
 * @param  sioiot_ota_state_callback[in]: sioiot ota state callback
 * @param  is_block[in]: is block until file download finish
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
OSStatus sioiot_module_ota( const char *ota_url, const char *ota_file_md5, SIOIOT_OTA_STATE_CB sioiot_ota_state_callback, bool is_block);

#endif

