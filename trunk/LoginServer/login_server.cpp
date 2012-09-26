#include "login_server.h"
#include "login_server_loop.h"
#include "login_packet_dispatch.h"
#include "login_server_config.h"

LoginServer* g_pServer = NULL;
LoginServerConfig* g_pConfig = NULL;

LoginServer::LoginServer()
{
	m_pMasterServer = NULL;
}

int32 LoginServer::Init(const TCHAR* strServerName)
{
	return super::Init(strServerName);
}

void LoginServer::Destroy()
{
	super::Destroy();
}

ServerConfig* LoginServer::CreateConfig(uint32 iRealmId, const TCHAR* strServerName)
{
	g_pConfig = new LoginServerConfig(strServerName);
	return g_pConfig;
}

int32 LoginServer::GetServerAndPeerConfig(uint32& iPeerIP, uint16& iPeerPort, uint32& iServerIP, uint16& iServerPort, uint32& iThreadCount)
{
	iPeerIP = g_pConfig->m_iPeerIP;
	iPeerPort = g_pConfig->m_iPeerPort;
	iServerIP = g_pConfig->m_iServerIP;
	iServerPort = g_pConfig->m_iServerPort;
	iThreadCount = g_pConfig->m_iThreadCount;

	return 0;
}

int32 LoginServer::InitMainLoop()
{
	m_pMasterServer = GetPeerServer(_T("Master"));
	if (!m_pMasterServer)
	{
		return -1;
	}

	m_pMainLoop = new LoginServerLoop;

	return m_pMainLoop->Init();
}

void LoginServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
