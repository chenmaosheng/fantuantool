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

enum
{
	// all peer filters defined
	PEER_FILTER_SESSION,
	PEER_FILTER_LOGIN,
	PEER_FILTER_MASTER,
	PEER_FILTER_GATE,
};

#define CONFIG_PATH "..\\Config"			// config files folder
#define COMMON_CONFIG_FILE "common_config"	// common config's filename

#define MEMORY_OBJECT_MIN 8
#define MEMORY_OBJECT_MAX 65536

#endif
