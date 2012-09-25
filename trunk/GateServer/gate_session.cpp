#include "gate_session.h"
#include "gate_server_loop.h"
#include "gate_server.h"

GateServerLoop* GateSession::m_pMainLoop = NULL;

GateSession::GateSession()
{
	Clear();
}

GateSession::~GateSession()
{
	Clear();
}

void GateSession::Clear()
{
	super::Clear();
}

int32 GateSession::OnConnection(ConnID connId)
{
	return super::OnConnection(connId);
}

void GateSession::OnDisconnect()
{
	super::OnDisconnect();
}

void GateSession::Disconnect()
{
	super::Disconnect();
}


