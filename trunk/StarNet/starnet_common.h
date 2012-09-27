#ifndef _H_STARNET_COMMON
#define _H_STARNET_COMMON

#include "common.h"
#include "log.h"

typedef HANDLE				ConnID;

DECLARE_HANDLE(PEER_CLIENT);
DECLARE_HANDLE(PEER_SERVER);

#define MAX_INPUT_BUFFER	1024
#define MAX_OUTPUT_BUFFER	65500
#define MAX_PEER_BUFFER		65536

#endif
