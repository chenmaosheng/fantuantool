#ifndef _H_TEST_PACKET_HANDLER
#define _H_TEST_PACKET_HANDLER

#include "common.h"

void LoginReq(void* pClient, const char* strNickname);

int32 LoginNtf(void* pClient, uint32 iSessionId, const char* strNickName);

#endif
