#include "master_server.h"
#include "master_server_loop.h"
#include "master_peer_dispatch.h"
#include "master_server_config.h"

MasterServer* g_pServer = NULL;
MasterServerConfig* g_pConfig = NULL;

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

	iRet = StartPeerServer(g_pConfig->m_iPeerIP, g_pConfig->m_iPeerPort);
	if (iRet != 0)
	{
		return -2;
	}

	LOG_STT(LOG_SERVER, _T("StartPeerServer success, IP=%d, port=%d"), g_pConfig->m_iPeerIP, g_pConfig->m_iPeerPort);

	iRet = StartMainLoop();
	if (iRet != 0)
	{
		return -3;
	}

	return 0;
}

void MasterServer::Destroy()
{
	StopMainLoop();

	StopPeerServer();

	super::Destroy();
}

void MasterServer::Shutdown()
{

}

ServerConfig* MasterServer::CreateConfig(uint32 iRealmId, const TCHAR* strServerName)
{
	g_pConfig = new MasterServerConfig(strServerName);
	return g_pConfig;
}

int32 MasterServer::InitMainLoop()
{
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
