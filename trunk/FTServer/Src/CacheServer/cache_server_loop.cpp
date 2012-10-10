#include "cache_server_loop.h"
#include "cache_player_context.h"
#include "cache_server.h"
#include "cache_logic_command.h"
#include "cache_server_config.h"

#include "db_conn_pool.h"

#include "packet.h"

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

void CacheServerLoop::ShutdownPlayer(CachePlayerContext* pPlayerContext)
{
	if (pPlayerContext->m_bFinalizing)
	{
		LOG_WAR(LOG_SERVER, _T("acc=%s sid=%08x is finalizing"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	}
	else
	{
		pPlayerContext->Shutdown();
	}
}

void CacheServerLoop::AddPlayerToFinalizingQueue(CachePlayerContext* pPlayerContext)
{
	pPlayerContext->m_bFinalizing = true;
	m_PlayerFinalizingQueue.push(pPlayerContext);
}

void CacheServerLoop::DeletePlayer(CachePlayerContext* pPlayerContext)
{
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x delete player"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);

	stdext::hash_map<uint32, CachePlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pPlayerContext->m_iSessionId);
	if (mit != m_mPlayerContextBySessionId.end())
	{
		m_mPlayerContextBySessionId.erase(mit);
	}

	// put context to pool
	pPlayerContext->Clear();
	m_PlayerContextPool.Free(pPlayerContext);
}

DWORD CacheServerLoop::_Loop()
{
	DBEvent* pEvent = NULL;
	while ((pEvent = m_pDBConnPool->PopFromDBEventReturnList()) != NULL)
	{
		_OnDBEventResult(pEvent);
	}

	while (!m_PlayerFinalizingQueue.empty())
	{
		DeletePlayer(m_PlayerFinalizingQueue.back());
		m_PlayerFinalizingQueue.pop();
	}

	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		m_iShutdownStatus = READY_FOR_SHUTDOWN;
	}

	return 100;
}

void CacheServerLoop::_OnDBEventResult(DBEvent* pEvent)
{
	switch(pEvent->m_iEventId)
	{
	case DB_EVENT_GETAVATARLIST:
		_OnPlayerEventResult((PlayerDBEvent*)pEvent);
		break;

	default:
		break;
	}
}

void CacheServerLoop::_OnPlayerEventResult(PlayerDBEvent* pEvent)
{
	stdext::hash_map<uint32, CachePlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pEvent->m_iSessionId);
	if (mit == m_mPlayerContextBySessionId.end())
	{
		LOG_ERR(LOG_DB, _T("acc=? sid=%08x can't find player"), pEvent->m_iSessionId);
		return;
	}

	CachePlayerContext* pCachePlayerContext = mit->second;

	switch(pEvent->m_iEventId)
	{
	case DB_EVENT_GETAVATARLIST:
		pCachePlayerContext->OnPlayerEventGetAvatarListResult((PlayerDBEventGetAvatarList*)pEvent);
		break;

	default:
		break;
	}
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

	case COMMAND_ONLOGOUTREQ:
		_OnCommandOnLogoutReq((LogicCommandOnLogoutReq*)pCommand);
		break;

	case COMMAND_PACKETFORWARD:
		_OnCommandPacketForward((LogicCommandPacketForward*)pCommand);
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

void CacheServerLoop::_OnCommandOnLogoutReq(LogicCommandOnLogoutReq* pCommand)
{
	stdext::hash_map<uint32, CachePlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit != m_mPlayerContextBySessionId.end())
	{
		LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive logout request"), mit->second->m_strAccountName, pCommand->m_iSessionId);
		mit->second->OnLogoutReq();
	}
}

void CacheServerLoop::_OnCommandPacketForward(LogicCommandPacketForward* pCommand)
{
	stdext::hash_map<uint32, CachePlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit != m_mPlayerContextBySessionId.end())
	{
		if (!Receiver::OnPacketReceived((void*)mit->second, pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData))
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x on packet received failed"), mit->second->m_strAccountName, pCommand->m_iSessionId);
		}
	}
}