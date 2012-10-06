#include "cache_player_context.h"
#include "cache_server_loop.h"
#include "cache_db_event.h"
#include "db_conn_pool.h"
#include "packet.h"

CacheServerLoop* CachePlayerContext::m_pMainLoop = NULL;

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
}

void CachePlayerContext::Shutdown()
{
}

void CachePlayerContext::OnLoginReq(uint32 iSessionId, TCHAR *strAccountName)
{
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGINREQ) != PLAYER_STATE_ONLOGINREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);
	m_iSessionId = iSessionId;
}

void CachePlayerContext::OnAvatarListReq()
{
	int32 iRet = 0;
	PlayerDBEventGetAvatarList* pDBEvent = NULL;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x query avatar list"), m_strAccountName, m_iSessionId);

	pDBEvent = m_pMainLoop->m_pDBConnPool->AllocateEvent<PlayerDBEventGetAvatarList>();
	if (!pDBEvent)
	{
		// todo:
		return;
	}

	pDBEvent->m_iSessionId = m_iSessionId;
	wcscpy_s(pDBEvent->m_strAccountName, _countof(pDBEvent->m_strAccountName), m_strAccountName);
	m_pMainLoop->m_pDBConnPool->PushSequenceEvent(m_iSessionId, pDBEvent);
}
