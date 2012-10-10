#include "master_player_context.h"

#include "gate_client_recv.h"
#include "gate_server_recv.h"
#include "ftd_define.h"

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

void GateClientRecv::AvatarCreateReq(void *pClient, const ftdAvatarCreateData& data)
{
	int32 iRet = 0;
	MasterPlayerContext* pPlayerContext = (MasterPlayerContext*)pClient;
	prdAvatarCreateData prdData;
	
	LOG_DBG(LOG_SERVER, _T("acc=%s, sid=%08x create avatar request"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	
	iRet = ftdAvatarCreateData2prdAvatarCreateData(&data, &prdData);
	if (iRet != 0)
	{
		_ASSERT( false && "ftdAvatarCreateData2prdAvatarCreateData failed" );
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ftdAvatarCreateData2prdAvatarCreateData failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
		return;
	}
	
	pPlayerContext->OnAvatarCreateReq(prdData);
}

void GateServerRecv::AvatarCreateAck(void* pClient, int32 iReturn, const ftdAvatar& newAvatar)
{
	int32 iRet = 0;
	MasterPlayerContext* pPlayerContext = (MasterPlayerContext*)pClient;
	prdAvatar prdData;

	LOG_DBG(LOG_SERVER, _T("acc=%s, sid=%08x create avatar request"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	
	iRet = ftdAvatar2prdAvatar(&newAvatar, &prdData);
	if (iRet != 0)
	{
		_ASSERT( false && "ftdAvatar2prdAvatar failed" );
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ftdAvatar2prdAvatar failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
		return;
	}
	
	pPlayerContext->OnAvatarCreateAck(iReturn, prdData);
}
