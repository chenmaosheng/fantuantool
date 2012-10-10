#include "cache_player_context.h"
#include "cache_server_loop.h"
#include "cache_db_event.h"
#include "cache_server.h"
#include "db_conn_pool.h"

#include "gate_server_send.h"
#include "session_peer_send.h"
#include "packet.h"

CacheServerLoop* CachePlayerContext::m_pMainLoop = NULL;
uint16 CachePlayerContext::m_iDelayTypeId = 0;
uint16 CachePlayerContext::m_iDelayLen = 0;
char CachePlayerContext::m_DelayBuf[MAX_INPUT_BUFFER] = {0};

CachePlayerContext::CachePlayerContext() :
m_StateMachine(PLAYER_STATE_NONE)
{
	Clear();
	_InitStateMachine();
}

CachePlayerContext::~CachePlayerContext()
{
	Clear();
}

void CachePlayerContext::Clear()
{
	m_iSessionId = 0;
	m_strAccountName[0] = _T('\0');
	m_StateMachine.SetCurrState(PLAYER_STATE_NONE);
	m_iAvatarCount = 0;
	memset(m_arrayAvatar, 0, sizeof(m_arrayAvatar));
	m_bFinalizing = false;
}

int32 CachePlayerContext::DelaySendData(uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	m_iDelayTypeId = iTypeId;
	m_iDelayLen = iLen;
	memcpy(m_DelayBuf, pBuf, iLen);

	return 0;
}

void CachePlayerContext::Shutdown()
{
	bool bNeedSave = false;

	switch(m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONLOGINREQ:
	case PLAYER_STATE_AVATARLISTREQ:
	case PLAYER_STATE_AVATARLISTACK:
		bNeedSave = false;
		break;

	default:
		break;
	}

	if (!bNeedSave)
	{
		m_pMainLoop->AddPlayerToFinalizingQueue(this);
	}
}

void CachePlayerContext::OnLoginReq(uint32 iSessionId, TCHAR *strAccountName)
{
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGINREQ) != PLAYER_STATE_ONLOGINREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x OnLoginReq"), m_strAccountName, m_iSessionId);

	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);
	m_iSessionId = iSessionId;
}

void CachePlayerContext::OnLogoutReq()
{
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGOUTREQ) != PLAYER_STATE_ONLOGOUTREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x OnLogoutReq"), m_strAccountName, m_iSessionId);

	m_pMainLoop->ShutdownPlayer(this);
}

void CachePlayerContext::OnAvatarListReq()
{
	int32 iRet = 0;
	PlayerDBEventGetAvatarList* pDBEvent = NULL;

	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARLISTREQ) != PLAYER_STATE_AVATARLISTREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x query avatar list"), m_strAccountName, m_iSessionId);

	pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventGetAvatarList>();
	if (!pDBEvent)
	{
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x failed to allocate event"), m_strAccountName, m_iSessionId);

		// send avatar list to client
		iRet = GateServerSend::AvatarListAck(this, 0, 0, NULL);
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

		return;
	}

	pDBEvent->m_iSessionId = m_iSessionId;
	wcscpy_s(pDBEvent->m_strAccountName, _countof(pDBEvent->m_strAccountName), m_strAccountName);
	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}

void CachePlayerContext::OnAvatarCreateReq(prdAvatarCreateData& data)
{
	int32 iRet = 0;
	PlayerDBEventAvatarCreate* pDBEvent = NULL;
	ftdAvatar avatar;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x create avatar"), m_strAccountName, m_iSessionId);

	pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventAvatarCreate>();
	if (!pDBEvent)
	{
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x failed to allocate event"), m_strAccountName, m_iSessionId);
		
		// send avatar new to client
		iRet = GateServerSend::AvatarCreateAck(this, 0, avatar);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send new avatar to client failed"), m_strAccountName, m_iSessionId);
			return;
		}

		iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
		}
		
		return;
	}

	pDBEvent->m_iSessionId = m_iSessionId;
	wcscpy_s(pDBEvent->m_strAccountName, _countof(pDBEvent->m_strAccountName), m_strAccountName);
	wcscpy_s(pDBEvent->m_Avatar.m_strAvatarName, _countof(pDBEvent->m_Avatar.m_strAvatarName), data.m_strAvatarName);

	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((CachePlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}