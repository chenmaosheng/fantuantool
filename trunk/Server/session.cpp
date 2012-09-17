#include "session.h"
#include "server_base.h"
#include "connection.h"

ServerBase* Session::server_ = NULL;

void Session::Initialize(ServerBase* pServer)
{
	server_ = pServer;
}

Session::Session()
{
	sessionId_ = 0;
	connId_ = NULL;
}

Session::~Session()
{
	connId_ = NULL;
}

void Session::Clear()
{
	connId_ = NULL;
}

int32 Session::OnConnection(ConnID connId)
{
	int32 iRet = 0;
	int32 i = sizeof(SessionId);

	// reset sequence
	((SessionId*)(&sessionId_))->sValue_.sequence_++;

	// coordinate conn and session
	connId_ = connId;

	// todo: consider avoid to use Connection here
	//Connection::SetClient(connId_, this);
	//Connection::SetRefMax(connId_, 256);

	return 0;
}

void Session::OnDisconnect()
{
	
}