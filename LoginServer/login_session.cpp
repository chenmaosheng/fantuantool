#include "login_session.h"
#include "login_server_loop.h"
#include "master_peer_send.h"
#include "login_server.h"

LoginServerLoop* LoginSession::m_pMainLoop = NULL;

LoginSession::LoginSession()
{
	Clear();
}

LoginSession::~LoginSession()
{
	Clear();
}

void LoginSession::Clear()
{
	super::Clear();
	m_strAccountName[0] = '\0';
}

int32 LoginSession::OnConnection(ConnID connId)
{
	return super::OnConnection(connId);
}

void LoginSession::OnDisconnect()
{
	super::OnDisconnect();
}

void LoginSession::Disconnect()
{
	super::Disconnect();
}

void LoginSession::LoginReq(const char* strNickname)
{
	int32 iRet = 0;

	iRet = MasterPeerSend::LoginReq(g_pServer->m_pMasterServer, m_iSessionId, strNickname);
	if (iRet != 0)
	{
		Disconnect();
	}
}



