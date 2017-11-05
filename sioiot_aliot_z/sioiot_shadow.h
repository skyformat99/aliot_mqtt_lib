#ifndef _SIOIOT_SHADOW_H_
#define _SIOIOT_SHADOW_H_

#include "json_parser.h"
#include "sioiot_config.h"

extern char ticket_local[80];
extern char status_command_local[10];

#define COMMANDTMOUT 180
int shadow_parser_data( char* recv_buf, unsigned int buf_len );
int shadow_reported_set( char* recv_buf, unsigned int buf_len );
void shadow_reported_get( char* recv_buf, unsigned int buf_len );

IOTStatus shadow_get( char* send_buf, unsigned int buf_len );

void shadow_desired_update( char* send_buf, unsigned int buf_len );
void shadow_reported_update( char* send_buf, unsigned int buf_len, char *machine, char*ticket,
                             char* ticket_status,
                             uint32_t ticket_finish );

#endif

