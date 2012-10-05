#include "cache_player_context.h"
#include "cache_server_loop.h"
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

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((CachePlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}