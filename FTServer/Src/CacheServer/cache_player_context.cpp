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
	m_bFinalizing = false;
	m_iRegionServerId = 0;
	m_AvatarContext.Clear();
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
	bool bNeedSave = true;

	switch(m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONLOGINREQ:
	case PLAYER_STATE_ONAVATARLISTREQ:
	case PLAYER_STATE_AVATARLISTACK:
	case PLAYER_STATE_ONAVATARSELECTREQ:
	case PLAYER_STATE_AVATARSELECTACK:
	case PLAYER_STATE_ONREGIONENTERREQ:
		bNeedSave = false;
		break;

	default:
		break;
	}

	if (!bNeedSave)
	{
		m_pMainLoop->AddPlayerToFinalizingQueue(this);
		return;
	}

	// todo: save other data

	PlayerDBEventAvatarFinalize* pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventAvatarFinalize>();
	if (!pDBEvent)
	{
		_ASSERT(false && _T("failed to allocate event"));
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x failed to allocate event"), m_strAccountName, m_iSessionId);
		return;
	}
	pDBEvent->m_iSessionId = m_iSessionId;
	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}

void CachePlayerContext::OnLoginReq(uint32 iSessionId, TCHAR *strAccountName)
{
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGINREQ) != PLAYER_STATE_ONLOGINREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x OnLoginReq"), strAccountName, iSessionId);

	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);
	m_iSessionId = iSessionId;
}

void CachePlayerContext::OnLogoutReq()
{
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGOUTREQ) != PLAYER_STATE_ONLOGOUTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x OnLogoutReq"), m_strAccountName, m_iSessionId);

	m_pMainLoop->ShutdownPlayer(this);
}

void CachePlayerContext::OnRegionEnterReq(uint8 iServerId, TCHAR* strAvatarName)
{
	int32 iRet = 0;
	PlayerDBEventAvatarEnterRegion* pDBEvent = NULL;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONENTERREQ) != PLAYER_STATE_ONREGIONENTERREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive region enter request"), m_strAccountName, m_iSessionId);

	pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventAvatarEnterRegion>();
	if (!pDBEvent)
	{
		_ASSERT(false && _T("failed to allocate event"));
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x failed to allocate event"), m_strAccountName, m_iSessionId);

		// todo:
		return;
	}

	// record region server id
	m_iRegionServerId = iServerId;

	pDBEvent->m_iSessionId = m_iSessionId;
	wcscpy_s(pDBEvent->m_strAvatarName, _countof(pDBEvent->m_strAvatarName), strAvatarName);
	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}

void CachePlayerContext::OnAvatarListReq()
{
	int32 iRet = 0;
	PlayerDBEventGetAvatarList* pDBEvent = NULL;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARLISTREQ) != PLAYER_STATE_ONAVATARLISTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x query avatar list"), m_strAccountName, m_iSessionId);

	pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventGetAvatarList>();
	if (!pDBEvent)
	{
		_ASSERT(false && _T("failed to allocate event"));
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
		_ASSERT(false && _T("failed to allocate event"));
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
	wcscpy_s(pDBEvent->m_Avatar.m_strAvatarName, AVATARNAME_MAX+1, data.m_strAvatarName);

	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}

void CachePlayerContext::OnAvatarSelectReq(const TCHAR* strAvatarName)
{
	PlayerDBEventAvatarSelectData* pDBEvent = NULL;
	int32 iRet = 0;
	bool bExist = false;
	uint64 iSelectAvatarId = 0;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARSELECTREQ) != PLAYER_STATE_ONAVATARSELECTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x name=%s select avatar"), m_strAccountName, m_iSessionId, strAvatarName);

	// check if avatarname is valid
	for (uint8 i = 0; i < m_iAvatarCount; ++i)
	{
		if (wcscmp(m_arrayAvatar[i].m_strAvatarName, strAvatarName) == 0)
		{
			bExist = true;
			iSelectAvatarId = m_arrayAvatar[i].m_iAvatarId;
			break;
		}
	}

	if (!bExist)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d can't find avatar name=%s"), m_strAccountName, m_iSessionId, strAvatarName);
		return;
	}

	pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventAvatarSelectData>();
	if (!pDBEvent)
	{
		_ASSERT(false && _T("failed to allocate event"));
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x failed to allocate event"), m_strAccountName, m_iSessionId);

		// send avatar new to client
		ftdAvatarSelectData data;
		iRet = GateServerSend::AvatarSelectAck(this, -1, data);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send avatar select data to client failed"), m_strAccountName, m_iSessionId);
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
	wcscpy_s(pDBEvent->m_strAvatarName, _countof(pDBEvent->m_strAvatarName), strAvatarName);
	pDBEvent->m_iAvatarId = iSelectAvatarId;
	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}




























int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((CachePlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}