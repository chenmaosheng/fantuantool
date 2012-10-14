#include "region_server.h"
#include "region_server_loop.h"
#include "region_server_config.h"

RegionServer* g_pServer = NULL;
RegionServerConfig* g_pServerConfig = NULL;

RegionServer::RegionServer()
{
	m_pMasterServer = NULL;
	m_pCacheServer = NULL;
}

int32 RegionServer::Init(const TCHAR* strServerName)
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

	// start region server's main loop
	iRet = StartMainLoop();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Start region server's main loop failed"));
		return -3;
	}

	LOG_STT(LOG_SERVER, _T("Initialize region server success"));

	return 0;
}

void RegionServer::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy region server"));

	// stop all the region server issues
	StopMainLoop();

	StopPeerServer();

	super::Destroy();
}

void RegionServer::Shutdown()
{
	LOG_STT(LOG_SERVER, _T("Shutdown region server"));

	m_pMainLoop->PushShutdownCommand();

	while (!m_pMainLoop->IsReadyForShutdown())
	{
		Sleep(100);
	}
}

ServerConfig* RegionServer::CreateConfig(uint32 iRealmId, const TCHAR* strServerName)
{
	g_pServerConfig = new RegionServerConfig(strServerName);
	return g_pServerConfig;
}

int32 RegionServer::InitMainLoop()
{
	// get master peer server by name
	m_pMasterServer = GetPeerServer(_T("Master"));
	if (!m_pMasterServer)
	{
		LOG_ERR(LOG_SERVER, _T("Get master peer server failed"));
		return -1;
	}

	// get cache peer server by name
	m_pCacheServer = GetPeerServer(_T("Cache"));
	if (!m_pCacheServer)
	{
		LOG_ERR(LOG_SERVER, _T("Get cache peer server failed"));
		return -1;
	}

	m_pMainLoop = new RegionServerLoop;

	return m_pMainLoop->Init();
}

void RegionServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
