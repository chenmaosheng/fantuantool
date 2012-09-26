#include "gate_server.h"
#include "gate_server_loop.h"
#include "gate_server_config.h"

GateServer* g_pServer = NULL;
GateServerConfig* g_pConfig = NULL;

GateServer::GateServer()
{
	m_pMasterServer = NULL;
}

int32 GateServer::Init(const TCHAR* strServerName)
{
	return super::Init(strServerName);
}

void GateServer::Destroy()
{
	DestroyMainLoop();
	super::Destroy();
}

ServerConfig* GateServer::CreateConfig(uint32 iRealmId, const TCHAR* strServerName)
{
	g_pConfig = new GateServerConfig(strServerName);
	return g_pConfig;
}

int32 GateServer::GetServerAndPeerConfig(uint32& iPeerIP, uint16& iPeerPort, uint32& iServerIP, uint16& iServerPort, uint32& iThreadCount)
{
	iPeerIP = g_pConfig->m_iPeerIP;
	iPeerPort = g_pConfig->m_iPeerPort;
	iServerIP = g_pConfig->m_iServerIP;
	iServerPort = g_pConfig->m_iServerPort;
	iThreadCount = g_pConfig->m_iThreadCount;

	return 0;
}

int32 GateServer::InitMainLoop()
{
	m_pMasterServer = GetPeerServer(_T("Master"));
	if (!m_pMasterServer)
	{
		return -1;
	}

	m_pMainLoop = new GateServerLoop;

	return m_pMainLoop->Init();
}

void GateServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
