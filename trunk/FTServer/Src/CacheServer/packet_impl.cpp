#include "cache_player_context.h"
#include "gate_client_recv.h"

void GateClientRecv::AvatarListReq(void* pClient)
{
	CachePlayerContext* pPlayerContext = (CachePlayerContext*)pClient;
	pPlayerContext->OnAvatarListReq();
}