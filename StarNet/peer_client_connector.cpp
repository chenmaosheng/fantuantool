#include "peer_client_connector.h"
#include "connection.h"
#include "handler.h"
#include "peer_client.h"
#include "peer_stream.h"

void PeerClientConnector::Init(uint32 iIP, uint16 iPort)
{
	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(iPort);
	m_SockAddr.sin_addr.s_addr = iIP;

	m_iState = NOT_CONNECT;
}

void PeerClientConnector::Destroy()
{
	Connection* pConnection = (Connection*)m_ConnId;
	if (m_iState == CONNECTED)
	{
		pConnection->AsyncDisconnect();
		while (m_iState == CONNECTED)
		{
			Sleep(100);
		}
	}
	
	if (m_iState == DISCONNECTED)
	{
		Connection::Close(pConnection);
	}
}

PSOCKADDR_IN PeerClientConnector::GetSockAddr()
{
	return &m_SockAddr;
}

ConnID PeerClientConnector::GetConnId()
{
	Connection* pConnection = (Connection*)m_ConnId;
	switch (m_iState)
	{
	case CONNECTED:
		return m_ConnId;

	case DISCONNECTED:
		{
			Connection::Close(pConnection);
			m_iState = NOT_CONNECT;
		}
		break;

	case NOT_CONNECT:
		{
			m_iState = CONNECTING;
			if (Connect())
			{
				while (m_iState == CONNECTING)
				{
					Sleep(100);
				}

				if (m_iState == CONNECTED)
				{
					return m_ConnId;
				}
			}
			else
			{
				m_iState = NOT_CONNECT;
			}
		}
		break;
	}
	return NULL;
}

void PeerClientConnector::OnPeerData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	int32 iRet = 0;
	Connection* pConnection = (Connection*)m_ConnId;
	if (!pConnection->IsConnected())
	{
		return;
	}

	do 
	{
		if (m_iRecvBufLen + iLen <= sizeof(m_RecvBuf))
		{
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iLen);
			m_iRecvBufLen += iLen;
			pBuf += iLen;
			iLen = 0;
		}
		else
		{
			iCopyLen = m_iRecvBufLen + iLen - sizeof(m_RecvBuf);
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iCopyLen);
			pBuf += iCopyLen;
			iLen -= iCopyLen;
			m_iRecvBufLen += iCopyLen;
		}

		while (m_iRecvBufLen > PEER_PACKET_HEAD)
		{
			PeerPacket* pPeerPacket = (PeerPacket*)m_RecvBuf;
			uint16 iFullLength = pPeerPacket->m_iLen + PEER_PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)
			{
				iRet = Dispatch(pPeerPacket);
				if (iRet != 0)
				{
					return;
				}

				if (m_iRecvBufLen > iFullLength)
				{
					memmove(m_RecvBuf, m_RecvBuf + iFullLength, m_iRecvBufLen - iFullLength);
				}
				m_iRecvBufLen -= iFullLength;
			}
			else
			{
				break;
			}
		}
	} while (iLen);
}

bool PeerClientConnector::Dispatch(PeerPacket* pPeerPacket)
{
	return Dispatch(pPeerPacket->m_iFilterId, pPeerPacket->m_iFuncId, pPeerPacket->m_iLen, pPeerPacket->m_Buf);
}

bool CALLBACK PeerClientConnector::OnConnection(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	PeerClientConnector* pConnector = (PeerClientConnector*)pConnection->GetClient();
	pConnector->m_ConnId = connId;
	pConnector->m_iState = CONNECTED;
	pConnector->m_iRecvBufLen = 0;

	return true;
}

void CALLBACK PeerClientConnector::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	PeerClientConnector* pConnector = (PeerClientConnector*)pConnection->GetClient();
	pConnector->m_iState = DISCONNECTED;
}

void CALLBACK PeerClientConnector::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connId;
	PeerClientConnector* pConnector = (PeerClientConnector*)pConnection->GetClient();
	pConnector->OnPeerData(iLen, pBuf);
}

void CALLBACK PeerClientConnector::OnConnectFailed(void* pClient)
{
	PeerClientConnector* pConnector = (PeerClientConnector*)pClient;
	if (pConnector->m_iState == DESTORYING)
	{
		pConnector->m_iState = NOT_CONNECT;
	}
	else
	if (pConnector->m_iState == CONNECTING)
	{
		pConnector->m_iState = CONNECT_FAILED;
	}
	else
	if (!pConnector->Connect())
	{
		pConnector->m_iState = NOT_CONNECT;
	}
}

bool PeerClientConnector::Connect()
{
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	return Connection::Connect(&m_SockAddr, &handler, PeerClient::Instance()->m_pContextPool, PeerClient::Instance()->m_pWorker, this);
}

int32 PeerOutputStream::Send(PEER_CLIENT_CONNECTOR pClientConnector)
{
	ConnID connId = ((PeerClientConnector*)pClientConnector)->GetConnId();
	if (connId)
	{
		((Connection*)connId)->AsyncSend(PEER_PACKET_HEAD + m_pPacket->m_iLen, (char*)m_pPacket);
		m_pPacket = NULL;
		return 0;
	}

	return -1;
}
