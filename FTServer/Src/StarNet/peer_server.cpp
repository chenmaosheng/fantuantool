#include "peer_server.h"
#include "connection.h"
#include "handler.h"
#include "peer_stream.h"
#include "starnet.h"
#include "worker.h"
#include "context_pool.h"

void PeerServer::Init(uint32 iIP, uint16 iPort)
{
	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(iPort);
	m_SockAddr.sin_addr.s_addr = iIP;

	// initial state
	m_iState = NOT_CONNECT;

	SN_LOG_STT(_T("Initial the state of PeerServer, wait for connecting"));
}

void PeerServer::Destroy()
{
	SN_LOG_STT(_T("Ready to disconnect from server"));

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

	SN_LOG_STT(_T("Disconnected from server"));
}

PSOCKADDR_IN PeerServer::GetSockAddr()
{
	return &m_SockAddr;
}

ConnID PeerServer::GetConnId()
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
			// try to connect until connected
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

	SN_LOG_ERR(_T("There is something wrong when get connId"));
	return NULL;
}

void PeerServer::OnPeerData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	bool bRet = false;
	Connection* pConnection = (Connection*)m_ConnId;
	// check if peer client is connected
	if (!pConnection->IsConnected())
	{
		return;
	}

	do 
	{
		// check if received buffer is not enough
		// if that, we should split the incoming buffer and handle the rest of them
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

		// check if currently received buffer is longer than packet header
		// if that, loop retrieve packet from buffer until the length is shorter than header
		while (m_iRecvBufLen > PEER_PACKET_HEAD)
		{
			PeerPacket* pPeerPacket = (PeerPacket*)m_RecvBuf;
			uint16 iFullLength = pPeerPacket->m_iLen + PEER_PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)
			{
				bRet = Dispatch(pPeerPacket);
				if (!bRet)
				{
					SN_LOG_ERR(_T("Dispatch failed"));
					return;
				}

				// after dispatched, move memory in order to cut the finished buffer
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

bool PeerServer::Dispatch(PeerPacket* pPeerPacket)
{
	return Dispatch(pPeerPacket->m_iFilterId, pPeerPacket->m_iFuncId, pPeerPacket->m_iLen, pPeerPacket->m_Buf);
}

bool CALLBACK PeerServer::OnConnection(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	PeerServer* pConnector = (PeerServer*)pConnection->GetClient();
	pConnector->m_ConnId = connId;
	pConnector->m_iState = CONNECTED;
	pConnector->m_iRecvBufLen = 0;

	return true;
}

void CALLBACK PeerServer::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	PeerServer* pConnector = (PeerServer*)pConnection->GetClient();
	pConnector->m_iState = DISCONNECTED;
}

void CALLBACK PeerServer::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connId;
	PeerServer* pConnector = (PeerServer*)pConnection->GetClient();
	pConnector->OnPeerData(iLen, pBuf);
}

void CALLBACK PeerServer::OnConnectFailed(void* pClient)
{
	PeerServer* pConnector = (PeerServer*)pClient;
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

bool PeerServer::Connect()
{
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	return Connection::Connect(&m_SockAddr, &handler, PeerServerSet::Instance()->m_pContextPool, PeerServerSet::Instance()->m_pWorker, this);
}

int32 PeerOutputStream::Send(PEER_SERVER pPeerServer)
{
	ConnID connId = ((PeerServer*)pPeerServer)->GetConnId();
	if (connId)
	{
		((Connection*)connId)->AsyncSend(PEER_PACKET_HEAD + m_pPacket->m_iLen, (char*)m_pPacket);
		m_pPacket = NULL;
		return 0;
	}

	return -1;
}

PeerServerSet::PeerServerSet()
{
	StarNet::Init();

	// create worker with 1 thread and context pool
	m_pWorker = Worker::CreateWorker(1);
	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);

	SN_LOG_STT(_T("Initialize PeerServerSet success"));
}

PeerServerSet::~PeerServerSet()
{
	for (std::vector<PeerServer*>::iterator it = m_vPeerServers.begin(); it != m_vPeerServers.end(); ++it)
	{
		(*it)->Destroy();
		_aligned_free(*it);
	}
	m_vPeerServers.clear();

	if (m_pContextPool)
	{
		ContextPool::DestroyContextPool(m_pContextPool);
		m_pContextPool = NULL;
	}

	if (m_pWorker)
	{
		Worker::DestroyWorker(m_pWorker);
		m_pWorker = NULL;
	}

	StarNet::Destroy();

	SN_LOG_STT(_T("Destroy PeerServerSet success"));
}

PeerServer* PeerServerSet::GetPeerServer(uint32 iIP, uint16 iPort)
{
	for (std::vector<PeerServer*>::iterator it = m_vPeerServers.begin(); it != m_vPeerServers.end(); ++it)
	{
		if ((*it)->GetSockAddr()->sin_port == htons(iPort) &&
			(*it)->GetSockAddr()->sin_addr.s_addr == iIP)
		{
			return *it;
		}
	}

	PeerServer* pConnector = (PeerServer*)_aligned_malloc(sizeof(PeerServer), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnector)
	{
		pConnector->Init(iIP, iPort);
		return pConnector;
	}

	SN_LOG_ERR(_T("Something wrong when get peer server"));
	return NULL;
}

PeerOutputStream::PeerOutputStream()
{
	m_pPacket = (PeerPacket*)PeerServerSet::Instance()->m_pContextPool->PopOutputBuffer();
	if (m_pPacket)
	{
		m_pPacket->m_iLen = 0;
	}
}

PeerOutputStream::~PeerOutputStream()
{
	if (m_pPacket)
	{
		PeerServerSet::Instance()->m_pContextPool->PushOutputBuffer((char*)m_pPacket);
	}
}