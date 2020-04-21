/*****************************************************************************************
	filename:	starnet_common.h
	created:	09/27/2012
	author:		chen
	purpose:	necessary defines in starnet

*****************************************************************************************/

#ifndef _H_STARNET_COMMON
#define _H_STARNET_COMMON

#include "common.h"
#include "log.h"

typedef HANDLE				ConnID;

DECLARE_HANDLE(PEER_CLIENT);
DECLARE_HANDLE(PEER_SERVER);

#define SN_LOG_DBG(Expression, ...) LOG_DBG(LOG_NET, Expression, __VA_ARGS__)
#define SN_LOG_WAR(Expression, ...) LOG_WAR(LOG_NET, Expression, __VA_ARGS__)
#define SN_LOG_ERR(Expression, ...) LOG_ERR(LOG_NET, Expression, __VA_ARGS__)
#define SN_LOG_STT(Expression, ...) LOG_STT(LOG_NET, Expression, __VA_ARGS__)

#define MAX_INPUT_BUFFER	1024	// max size from client to server
#define MAX_OUTPUT_BUFFER	65500	// max size from server to client
#define MAX_PEER_BUFFER		65536	// max size from server to server

#endif
