#include "peer_client.h"
#include "connection.h"
#include "peer_stream.h"
#include "worker.h"
#include "context_pool.h"
#include "acceptor.h"

void PeerClient::OnPeerData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	bool bRet = false;
	Connection* pConnection = (Connection*)m_ConnId;
	// check if peer client is connected
	if (!pConnection->IsConnected())
	{
		SN_LOG_ERR(_T("Peer client is not connected"));
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

bool PeerClient::Dispatch(PeerPacket* pPeerPacket)
{
	return Dispatch(pPeerPacket->m_iFilterId, pPeerPacket->m_iFuncId, pPeerPacket->m_iLen, pPeerPacket->m_Buf);
}

int32 PeerOutputStream::Send(PEER_CLIENT pPeerClient)
{
	((Connection*)((PeerClient*)pPeerClient)->m_ConnId)->AsyncSend(PEER_PACKET_HEAD + m_pPacket->m_iLen, (char*)m_pPacket);
	m_pPacket = NULL;
	return 0;
}

bool PeerClientSet::Init(uint32 iIP, uint16 iPort)
{
	m_iIP = iIP;
	m_iPort = iPort;

	// create worker with 1 thread and context pool
	m_pWorker = Worker::CreateWorker(1);
	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);

	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = NULL;

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(iPort);
	addr.sin_addr.s_addr = iIP;

	// create acceptor
	m_pAcceptor = Acceptor::CreateAcceptor(&addr, m_pWorker, m_pContextPool, &handler);
	if (m_pAcceptor)
	{
		m_pAcceptor->Start();
		SN_LOG_STT(_T("Create Peer acceptor success"));
		return true;
	}

	SN_LOG_ERR(_T("Create Peer acceptor failed"));

	return false;
}

void PeerClientSet::Destroy()
{
	if (m_pAcceptor)
	{
		m_pAcceptor->Stop();
		for (std::vector<PeerClient*>::iterator it = m_vPeerClients.begin(); it != m_vPeerClients.end(); ++it)
		{
			((Connection*)(*it)->m_ConnId)->AsyncDisconnect();
		}
		Acceptor::DestroyAcceptor(m_pAcceptor);
	}

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

	SN_LOG_STT(_T("Destroy Peer acceptor success"));
}

bool PeerClientSet::AddConnector(PeerClient* pConnector)
{
	for (std::vector<PeerClient*>::iterator it = m_vPeerClients.begin(); it != m_vPeerClients.end(); ++it)
	{
		if ((*it)->m_ConnId == pConnector->m_ConnId)
		{
			SN_LOG_ERR(_T("This connection already exists, connId=%d"), pConnector->m_ConnId);
			return false;
		}
	}

	m_vPeerClients.push_back(pConnector);

	SN_LOG_STT(_T("Add a new connector success, connId=%d"), pConnector->m_ConnId);
	return true;
}

void PeerClientSet::DeleteConnector(ConnID connId)
{
	for (std::vector<PeerClient*>::iterator it = m_vPeerClients.begin(); it != m_vPeerClients.end(); ++it)
	{
		if ((*it)->m_ConnId == connId)
		{
			SN_LOG_STT(_T("delete a connector success, connId=%d"), connId);
			m_vPeerClients.erase(it);
			return;
		}
	}
}

bool CALLBACK PeerClientSet::OnConnection(ConnID connId)
{
	PeerClient* pConnector = (PeerClient*)_aligned_malloc(sizeof(PeerClient), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnector)
	{
		if (PeerClientSet::Instance()->AddConnector(pConnector))
		{
			pConnector->m_ConnId = connId;
			pConnector->m_iRecvBufLen = 0;
			((Connection*)connId)->SetClient(pConnector);
			return true;
		}
		else
		{
			_aligned_free(pConnector);
		}
	}
	return false;
}

void CALLBACK PeerClientSet::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	PeerClient* pConnector = (PeerClient*)pConnection->client_;
	if (pConnector)
	{
		PeerClientSet::Instance()->DeleteConnector(connId);
		_aligned_free(pConnector);
	}

	Connection::Close(pConnection);
}

void CALLBACK PeerClientSet::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connId;
	PeerClient* pConnector = (PeerClient*)pConnection->GetClient();
	pConnector->OnPeerData(iLen, pBuf);
}
