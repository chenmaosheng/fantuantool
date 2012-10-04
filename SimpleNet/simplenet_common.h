/*****************************************************************************************
	filename:	simplenet_common.h
	created:	10/03/2012
	author:		chen
	purpose:	necessary defines in simplenet

*****************************************************************************************/

#ifndef _H_SIMPLENET_COMMON
#define _H_SIMPLENET_COMMON

#include "common.h"
#include "log.h"

typedef HANDLE				ConnID;

#define SN_LOG_DBG(Expression, ...) LOG_DBG(LOG_NET, Expression, __VA_ARGS__)
#define SN_LOG_WAR(Expression, ...) LOG_WAR(LOG_NET, Expression, __VA_ARGS__)
#define SN_LOG_ERR(Expression, ...) LOG_ERR(LOG_NET, Expression, __VA_ARGS__)
#define SN_LOG_STT(Expression, ...) LOG_STT(LOG_NET, Expression, __VA_ARGS__)

#define MAX_INPUT_BUFFER	1024	// max size from client to server
#define MAX_OUTPUT_BUFFER	65500	// max size from server to client

#endif
