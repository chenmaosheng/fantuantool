#include "cache_player_context.h"
#include "cache_db_event.h"
#include "cache_server.h"

#include "ftd_define.h"
#include "gate_server_send.h"
#include "session_peer_send.h"

#include "db_conn_pool.h"

void CachePlayerContext::OnPlayerEventGetAvatarListResult(PlayerDBEventGetAvatarList* pEvent)
{
	int32 iRet = 0;
	ftdAvatar arrayAvatar[AVATARCOUNT_MAX];

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARLISTACK) != PLAYER_STATE_AVATARLISTACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_DB, _T("acc=%s sid=%08x send avatar list"), m_strAccountName, m_iSessionId);

	// if success, save the data to context
	if (pEvent->m_iRet == 0)
	{
		m_iAvatarCount = pEvent->m_iAvatarCount;
		memcpy(m_arrayAvatar, pEvent->m_arrayAvatar, sizeof(pEvent->m_arrayAvatar));
	}

	for (uint8 i = 0; i < m_iAvatarCount; ++i)
	{
		iRet = prdAvatar2ftdAvatar(&pEvent->m_arrayAvatar[i], &arrayAvatar[i]);
		if (iRet != 0)
		{
			_ASSERT( false && "prdAvatar2ftdAvatar failed" );
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x prdAvatar2ftdAvatar failed"), m_strAccountName, m_iSessionId);
			return;
		}
	}

	// send avatar list to client
	iRet = GateServerSend::AvatarListAck(this, pEvent->m_iRet, pEvent->m_iAvatarCount, arrayAvatar);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send avatarlist to client failed"), m_strAccountName, m_iSessionId);
		return;
	}

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
	}
}

void CachePlayerContext::OnPlayerEventAvatarCreateResult(PlayerDBEventAvatarCreate* pEvent)
{
	int32 iRet = 0;
	ftdAvatar avatar;

	LOG_DBG(LOG_DB, _T("acc=%s sid=%08x avatar create result"), m_strAccountName, m_iSessionId);

	// if success, save the data to context
	if (pEvent->m_iRet == 0)
	{
		iRet = prdAvatar2ftdAvatar(&pEvent->m_Avatar, &avatar);
		if (iRet != 0)
		{
			_ASSERT( false && "prdAvatar2ftdAvatar failed" );
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x prdAvatar2ftdAvatar failed"), m_strAccountName, m_iSessionId);
			return;
		}

		// check if avatar is full
		if (m_iAvatarCount >= AVATARCOUNT_MAX)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x avatar is full"), m_strAccountName, m_iSessionId);
			return;
		}

		memcpy(&m_arrayAvatar[m_iAvatarCount], &pEvent->m_Avatar, sizeof(prdAvatar));
		++m_iAvatarCount;
	}

	// send new avatar to client
	iRet = GateServerSend::AvatarCreateAck(this, pEvent->m_iRet, avatar);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send avatarcreate failed"), m_strAccountName, m_iSessionId);
		return;
	}

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
	}
}