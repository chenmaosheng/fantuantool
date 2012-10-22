#include "cache_player_context.h"

#include "gate_client_recv.h"
#include "ftd_define.h"

void GateClientRecv::AvatarListReq(void* pClient)
{
	CachePlayerContext* pPlayerContext = (CachePlayerContext*)pClient;
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x avatar list request"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);

	pPlayerContext->OnAvatarListReq();
}

void GateClientRecv::AvatarCreateReq(void *pClient, const ftdAvatarCreateData& data)
{
	CachePlayerContext* pPlayerContext = (CachePlayerContext*)pClient;
	prdAvatarCreateData prdData;
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x create avatar request"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);

	iRet = ftdAvatarCreateData2prdAvatarCreateData(&data, &prdData);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ftdAvatarCreateData2prdAvatarCreateData failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
		_ASSERT( false && "ftdAvatarCreateData2prdAvatarCreateData failed" );
		return;
	}

	pPlayerContext->OnAvatarCreateReq(prdData);
}

void GateClientRecv::AvatarSelectReq(void* pClient, const char* strAvatarName)
{
	CachePlayerContext* pPlayerContext = (CachePlayerContext*)pClient;
	TCHAR strTAvatarName[AVATARNAME_MAX+1] = {0};
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x select avatar request"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);

	iRet = Char2WChar(strAvatarName, strTAvatarName, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Char2WChar failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
		_ASSERT( false && "Char2WChar failed" );
		return;
	}
	strTAvatarName[iRet] = _T('\0');

	pPlayerContext->OnAvatarSelectReq(strTAvatarName);
}

void GateClientRecv::ChannelSelectReq(void* pClient, const char* strChannelName)
{
}

void GateClientRecv::ChannelLeaveReq(void* pClient)
{
}