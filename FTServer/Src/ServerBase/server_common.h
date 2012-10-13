/*****************************************************************************************
	filename:	server_common.h
	created:	09/27/2012
	author:		chen
	purpose:	necessary defines in server

*****************************************************************************************/

#ifndef _H_SERVER_COMMON
#define _H_SERVER_COMMON

#include "starnet_common.h"

#define PEER_SERVER_MAX 32		// max of peer server connected
#define GATE_SERVER_MAX 64		// max of gate server started
#define SERVERNAME_MAX	32
#define REGIONSERVER_MAX 32

enum
{
	// all peer filters defined
	PEER_FILTER_SESSION,
	PEER_FILTER_LOGIN,
	PEER_FILTER_MASTER,
	PEER_FILTER_GATE,
	PEER_FILTER_CACHE,
};

#define CONFIG_PATH "..\\..\\Config"			// config files folder
#define COMMON_CONFIG_FILE "common_config"	// common config's filename

#define MEMORY_OBJECT_MIN 8
#define MEMORY_OBJECT_MAX 65536

#define SQL_STATEMENT_MAX 4096
#define SQL_ERROR_CODE 255

#endif
