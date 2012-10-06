#include "cache_server.h"
#include "cache_server_config.h"
#include "cache_server_loop.h"

CacheServer* g_pServer = NULL;
CacheServerConfig* g_pServerConfig = NULL;

CacheServer::CacheServer()
{
	m_pMasterServer = NULL;
}

int32 CacheServer::Init(const TCHAR* strServerName)
{
	int32 iRet = 0;
	iRet = super::Init(strServerName);
	if (iRet != 0)
	{
		return -1;
	}

	// start peer server for peer clients
	iRet = StartPeerServer(g_pServerConfig->m_iPeerIP, g_pServerConfig->m_iPeerPort);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("StarPeerServer failed, IP=%d, port=%d"), g_pServerConfig->m_iPeerIP, g_pServerConfig->m_iPeerPort);
		return -2;
	}

	LOG_STT(LOG_SERVER, _T("StartPeerServer success, IP=%d, port=%d"), g_pServerConfig->m_iPeerIP, g_pServerConfig->m_iPeerPort);

	// start master server's main loop
	iRet = StartMainLoop();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Start cache server's main loop failed"));
		return -3;
	}

	LOG_STT(LOG_SERVER, _T("Initialize cache server success"));

	return 0;
}

void CacheServer::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy master server"));

	// stop all the master server issues
	StopMainLoop();

	StopPeerServer();

	super::Destroy();
}

void CacheServer::Shutdown()
{
	LOG_STT(LOG_SERVER, _T("Shutdown master server"));

	m_pMainLoop->PushShutdownCommand();

	while (!m_pMainLoop->IsReadyForShutdown())
	{
		Sleep(100);
	}
}

ServerConfig* CacheServer::CreateConfig(uint32 iRealmId, const TCHAR* strServerName)
{
	g_pServerConfig = new CacheServerConfig(strServerName);
	return g_pServerConfig;
}

int32 CacheServer::InitMainLoop()
{
	// get master peer server by name
	m_pMasterServer = GetPeerServer(_T("Master"));
	if (!m_pMasterServer)
	{
		LOG_ERR(LOG_SERVER, _T("Get master peer server failed"));
		return -1;
	}

	m_pMainLoop = new CacheServerLoop;

	return m_pMainLoop->Init();
}

void CacheServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
