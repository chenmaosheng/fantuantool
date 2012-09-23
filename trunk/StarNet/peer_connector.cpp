#include "peer_connector.h"
#include "connection.h"
#include "handler.h"

void PeerConnector::Init(uint32 iIP, uint16 iPort)
{
	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(iPort);
	m_SockAddr.sin_addr.s_addr = iIP;
}

void PeerConnector::Destroy()
{
	Connection* pConnection = (Connection*)m_ConnId;
	pConnection->AsyncDisconnect();
	Connection::Close(pConnection);
}

ConnId PeerConnector::GetConnId()
{
	Connection* pConnection = (Connection*)m_ConnId;
	if (pConnection)
	{
		return m_ConnId;
	}

	if (Connect())
	{
		return m_ConnId;
	}

	return NULL;
}

bool PeerConnector::Connect()
{
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	return 
}

