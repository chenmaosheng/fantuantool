#ifndef _H_SERVER_MESSAGE
#define _H_SERVER_MESSAGE

#include "common.h"

extern void PingReq(void* pClient, uint32 iVersion);
extern int32 PingAck(void* pServer, uint32 iVersion);

#endif
