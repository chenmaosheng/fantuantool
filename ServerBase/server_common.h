#ifndef _H_SERVER_COMMON
#define _H_SERVER_COMMON

#include "starnet_common.h"

#define PEER_SERVER_MAX 32

enum
{
	PEER_FILTER_SESSION,
	PEER_FILTER_LOGIN,
	PEER_FILTER_MASTER,
	PEER_FILTER_GATE,
};

enum
{
	SERVER_FILTER_LOGIN,
	SERVER_FILTER_GATE,
};

#define CONFIG_PATH "..\\Config"
#define COMMON_CONFIG_FILE "common_config"

#endif
