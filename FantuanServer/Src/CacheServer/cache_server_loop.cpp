#include "cache_server_loop.h"
#include "cache_player_context.h"
#include "cache_server.h"
#include "cache_logic_command.h"

CacheServerLoop::CacheServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN),
m_PlayerContextPool(5000)
{

}

int32 CacheServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	CachePlayerContext::m_pMainLoop = this;

	return 0;
}

void CacheServerLoop::Destroy()
{
	super::Destroy();
}

int32 CacheServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

void CacheServerLoop::Stop()
{
	super::Stop();
}

bool CacheServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

DWORD CacheServerLoop::_Loop()
{
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		m_iShutdownStatus = READY_FOR_SHUTDOWN;
	}

	return 100;
}

bool CacheServerLoop::_OnCommand(LogicCommand* pCommand)
{
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONLOGINREQ:
		_OnCommandOnLoginReq((LogicCommandOnLoginReq*)pCommand);
		break;

	default:
		break;
	}

	return true;
}

void CacheServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;
}

void CacheServerLoop::_OnCommandOnLoginReq(LogicCommandOnLoginReq* pCommand)
{
	CachePlayerContext* pPlayerContext = m_PlayerContextPool.Allocate();
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Allocate player context from pool failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Allocate player context success"), pCommand->m_strAccountName, pCommand->m_iSessionId);

	m_mPlayerContextBySessionId.insert(std::make_pair(pCommand->m_iSessionId, pPlayerContext));
	pPlayerContext->OnLoginReq(pCommand->m_iSessionId, pCommand->m_strAccountName);

}