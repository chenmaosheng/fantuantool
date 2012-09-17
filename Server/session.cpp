#include "session.h"
#include "server_base.h"
#include "connection.h"
#include "packet.h"
#include "context_pool.h"

ServerBase* Session::m_pServer = NULL;


void Session::Initialize(ServerBase* pServer)
{
	m_pServer = pServer;
}

Session::Session()
{
	m_iSessionId = 0;
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
}

Session::~Session()
{
	m_pConnection = NULL;
}

void Session::Clear()
{
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
}

int32 Session::OnConnection(ConnID connId)
{
	int32 iRet = 0;
	int32 i = sizeof(SessionId);

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	// coordinate conn and session
	m_pConnection = (Connection*)connId;

	// todo: consider avoid to use Connection here
	//Connection::SetClient(m_pConnection, this);
	//Connection::SetRefMax(m_pConnection, 256);

	return 0;
}

void Session::OnDisconnect()
{
	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
		m_pConnection = NULL;
	}

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	Clear();
}

void Session::OnData(uint16 iLen, char *pBuf)
{
}

void Session::Disconnect()
{
	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
	}
	else
	{
		OnDisconnect();
	}
}

int32 Session::SendData(uint16 filterId, uint16 len, const char *data)
{
	char* buf = m_pServer->GetContextPool()->PopOutputBuffer();
	if (!buf)
	{
		return -1;
	}

	CS_Packet* pPacket = (CS_Packet*)buf;
	pPacket->m_iLen = len;
	pPacket->m_iFilterId = filterId;
	memcpy(pPacket->m_Buf, data, len);
	m_pConnection->AsyncSend(pPacket->m_iLen, buf);

	return 0;
}

int32 Session::HandleData(uint16 iLen, char *pBuf)
{
	
	return 0;
}

void Session::SaveSendData(uint16 iFilterId, uint16 iLen, char *pBuf)
{
}