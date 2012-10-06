#include "cache_server_loop.h"
#include "cache_player_context.h"
#include "cache_server.h"
#include "cache_logic_command.h"
#include "cache_server_config.h"

#include "db_conn_pool.h"

CacheServerLoop::CacheServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN),
m_PlayerContextPool(5000),
m_pDBConnPool(new DBConnPool)
{
}

int32 CacheServerLoop::Init()
{
	int32 iRet = 0;

	LOG_STT(LOG_SERVER, _T("Initialize cache server loop"));

	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	CachePlayerContext::m_pMainLoop = this;

	// initialize db pool
	iRet = m_pDBConnPool->Init(g_pServerConfig->m_strDBName, g_pServerConfig->m_strDBHost, g_pServerConfig->m_iDBPort,
		g_pServerConfig->m_strDBUser, g_pServerConfig->m_strDBPassword, g_pServerConfig->m_iDBConnCount);
	if (iRet != 0)
	{
		LOG_ERR(LOG_DB, _T("Initialize DB Pool failed"));
		return iRet;
	}

	return 0;
}

void CacheServerLoop::Destroy()
{
	// destroy db pool
	m_pDBConnPool->Destroy();

	super::Destroy();

	LOG_STT(LOG_SERVER, _T("Destroy cache server loop"));
}

int32 CacheServerLoop::Start()
{
	int32 iRet = 0;

	LOG_STT(LOG_SERVER, _T("Start cache server loop"));
	
	// start db pool
	m_pDBConnPool->Start();

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

	// stop db pool
	m_pDBConnPool->Stop();

	LOG_STT(LOG_SERVER, _T("Stop cache server loop"));
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