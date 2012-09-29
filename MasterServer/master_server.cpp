#include "master_server.h"
#include "master_server_loop.h"
#include "master_peer_dispatch.h"
#include "master_server_config.h"

MasterServer* g_pServer = NULL;
MasterServerConfig* g_pServerConfig = NULL;

MasterServer::MasterServer()
{
}

int32 MasterServer::Init(const TCHAR* strServerName)
{
	int32 iRet = 0;
	iRet = super::Init(strServerName);
	if (iRet != 0)
	{
		return -1;
	}

	iRet = StartPeerServer(g_pServerConfig->m_iPeerIP, g_pServerConfig->m_iPeerPort);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("StarPeerServer failed, IP=%d, port=%d"), g_pServerConfig->m_iPeerIP, g_pServerConfig->m_iPeerPort);
		return -2;
	}

	LOG_STT(LOG_SERVER, _T("StartPeerServer success, IP=%d, port=%d"), g_pServerConfig->m_iPeerIP, g_pServerConfig->m_iPeerPort);

	iRet = StartMainLoop();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Start master server's main loop failed"));
		return -3;
	}

	LOG_STT(LOG_SERVER, _T("Initialize master server success"));

	return 0;
}

void MasterServer::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy master server"));

	StopMainLoop();

	StopPeerServer();

	super::Destroy();
}

void MasterServer::Shutdown()
{
	LOG_STT(LOG_SERVER, _T("Shutdown master server"));
}

ServerConfig* MasterServer::CreateConfig(uint32 iRealmId, const TCHAR* strServerName)
{
	g_pServerConfig = new MasterServerConfig(strServerName);
	return g_pServerConfig;
}

int32 MasterServer::InitMainLoop()
{
	m_pLoginServer = GetPeerServer(_T("Login"));
	if (!m_pLoginServer)
	{
		LOG_ERR(LOG_SERVER, _T("Get login peer server failed"));
		return -1;
	}

	m_pMainLoop = new MasterServerLoop;

	return m_pMainLoop->Init();
}

void MasterServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
