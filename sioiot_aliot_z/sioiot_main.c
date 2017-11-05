#include "../sioiot_aliot_z/sioiot_activate.h"
#include "mico.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)


extern OSStatus start_mqtt_sub_pub( void );

 mico_semaphore_t wait_sem = NULL;

/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback( void * const user_config_data, uint32_t size )
{
    memset( user_config_data, 0x0, size );
}

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            if( wait_sem != NULL ){
                mico_rtos_set_semaphore( &wait_sem );
            }
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}
static void sioiot_heap_main( mico_thread_arg_t arg ){
    micoMemInfo_t * heap;
    while ( 1 )
    {
        heap = MicoGetMemoryInfo( );
        app_log("allocted_memory:%d,free_memory:%d,num_of_chunks:%d,total_memory:%d",heap->allocted_memory,heap->free_memory,heap->num_of_chunks,heap->total_memory);
        mico_rtos_thread_sleep( 20 );
    }
}
int application_start( void )
{
    OSStatus err = kNoErr;

    mico_rtos_init_semaphore( &wait_sem, 1 );

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler,
                                       NULL );
    require_noerr( err, exit );

    /* mico system initialize */
    err = mico_system_init( mico_system_context_init( sizeof(sioiot_config_t) ) );
    require_noerr( err, exit );

    sioiot_init_device();

    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "heap", sioiot_heap_main,1024, (uint32_t)NULL );

    exit:
    mico_system_notify_remove( mico_notify_WIFI_STATUS_CHANGED,
                               (void *) micoNotify_WifiStatusHandler );
    mico_rtos_deinit_semaphore( &wait_sem );
    mico_rtos_delete_thread( NULL );
    return err;
}

