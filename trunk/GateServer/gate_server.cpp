#include "gate_server.h"
#include "gate_server_loop.h"

GateServer* g_pServer = NULL;

GateServer::GateServer()
{
	m_pMasterServer = NULL;
}

int32 GateServer::Init()
{
	return super::Init();
}

void GateServer::Destroy()
{
	DestroyMainLoop();
	super::Destroy();
}

int32 GateServer::InitMainLoop()
{
	m_pMasterServer = GetPeerServer((uint16)0);
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

void GateServer::InitPacketDispatch()
{
}