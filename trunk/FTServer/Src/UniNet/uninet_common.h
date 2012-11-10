/*****************************************************************************************
	filename:	uninet_common.h
	created:	11/10/2012
	author:		chen
	purpose:	necessary defines in uninet

*****************************************************************************************/

#ifndef _H_UNINET_COMMON
#define _H_UNINET_COMMON

#include "common.h"
#include "log.h"

typedef HANDLE				ConnID;

#define UN_LOG_DBG(Expression, ...) LOG_DBG(LOG_NET, Expression, __VA_ARGS__)
#define UN_LOG_WAR(Expression, ...) LOG_WAR(LOG_NET, Expression, __VA_ARGS__)
#define UN_LOG_ERR(Expression, ...) LOG_ERR(LOG_NET, Expression, __VA_ARGS__)
#define UN_LOG_STT(Expression, ...) LOG_STT(LOG_NET, Expression, __VA_ARGS__)

#define MAX_INPUT_BUFFER	1024	// max size from client to server
#define MAX_OUTPUT_BUFFER	65500	// max size from server to client

#endif
