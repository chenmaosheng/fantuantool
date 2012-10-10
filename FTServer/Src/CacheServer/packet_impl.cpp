#include "cache_player_context.h"

#include "gate_client_recv.h"
#include "ftd_define.h"

void GateClientRecv::AvatarListReq(void* pClient)
{
	CachePlayerContext* pPlayerContext = (CachePlayerContext*)pClient;
	pPlayerContext->OnAvatarListReq();
}

void GateClientRecv::AvatarCreateReq(void *pClient, const ftdAvatarCreateData& data)
{
	CachePlayerContext* pPlayerContext = (CachePlayerContext*)pClient;
	prdAvatarCreateData prdData;
	int32 iRet = 0;

	iRet = ftdAvatarCreateData2prdAvatarCreateData(&data, &prdData);
	if (iRet != 0)
	{
		_ASSERT( false && "ftdAvatarCreateData2prdAvatarCreateData failed" );
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ftdAvatarCreateData2prdAvatarCreateData failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
		return;
	}

	pPlayerContext->OnAvatarCreateReq(prdData);
}