#include "region_client_recv.h"
#include "region_player_context.h"

void RegionClientRecv::ClientTimeReq(void* pClient, uint32 iClientTime)
{
	RegionPlayerContext* pPlayerContext = (RegionPlayerContext*)pClient;
	pPlayerContext->OnClientTimeReq(iClientTime);
}

void RegionClientRecv::RegionChatReq(void* pClient, const char* strMessage)
{
	RegionPlayerContext* pPlayerContext = (RegionPlayerContext*)pClient;

	pPlayerContext->OnRegionChatReq(strMessage);
}