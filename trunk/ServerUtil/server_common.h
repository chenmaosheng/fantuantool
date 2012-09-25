#ifndef _H_SERVER_COMMON
#define _H_SERVER_COMMON

#include "starnet_common.h"

#define PEER_SERVER_MAX 32

extern uint16 server_port[];

#define MASTER_SERVER_ID 0
#define LOGIN_SERVER_ID 1

enum
{
	PEER_FILTER_LOGIN,
	PEER_FILTER_MASTER,
};

#endif
