#include "master_player_context.h"

#include "gate_client_recv.h"
#include "gate_server_recv.h"

void GateClientRecv::AvatarListReq(void* pClient)
{
	MasterPlayerContext* pPlayerContext = (MasterPlayerContext*)pClient;
	
	LOG_DBG(LOG_SERVER, _T("acc=%s, sid=%08x request avatar list"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	pPlayerContext->OnAvatarListReq();
}

void GateServerRecv::AvatarListAck(void* pClient, int32 iReturn, uint8 iAvatarCount, const ftdAvatar* arrayAvatar)
{
	int32 iRet = 0;
	MasterPlayerContext* pPlayerContext = (MasterPlayerContext*)pClient;
	prdAvatar arrayPrdAvatar[AVATARCOUNT_MAX];

	LOG_DBG(LOG_SERVER, _T("acc=%s, sid=%08x receive avatar list"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	
	for (uint8 i = 0; i < iAvatarCount; ++i)
	{
		iRet = ftdAvatar2prdAvatar(&arrayAvatar[i], &arrayPrdAvatar[i]);
		if (iRet != 0)
		{
			_ASSERT( false && "ftdAvatar2prdAvatar failed" );
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ftdAvatar2prdAvatar failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
			return;
		}
	}

	pPlayerContext->OnAvatarListAck(iReturn, iAvatarCount, arrayPrdAvatar);
}
