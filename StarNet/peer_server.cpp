#include "peer_server.h"
#include "peer_server_connector.h"
#include "worker.h"
#include "context_pool.h"
#include "connection.h"
#include "acceptor.h"

bool PeerServer::Init(uint32 iIP, uint16 iPort)
{
	m_iIP = iIP;
	m_iPort = iPort;

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

	m_pAcceptor = Acceptor::CreateAcceptor(&addr, m_pWorker, m_pContextPool, &handler);
	if (m_pAcceptor)
	{
		m_pAcceptor->Start();
		return true;
	}
	
	return false;
}

void PeerServer::Destroy()
{
	if (m_pAcceptor)
	{
		m_pAcceptor->Stop();
		for (std::vector<PeerServerConnector*>::iterator it = m_vPeerConnectors.begin(); it != m_vPeerConnectors.end(); ++it)
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
}

bool PeerServer::AddConnector(PeerServerConnector* pConnector)
{
	for (std::vector<PeerServerConnector*>::iterator it = m_vPeerConnectors.begin(); it != m_vPeerConnectors.end(); ++it)
	{
		if ((*it)->m_ConnId == pConnector->m_ConnId)
		{
			return false;
		}
	}

	m_vPeerConnectors.push_back(pConnector);
	return true;
}

void PeerServer::DeleteConnector(ConnID connId)
{
	for (std::vector<PeerServerConnector*>::iterator it = m_vPeerConnectors.begin(); it != m_vPeerConnectors.end(); ++it)
	{
		if ((*it)->m_ConnId == connId)
		{
			m_vPeerConnectors.erase(it);
			return;
		}
	}
}

bool CALLBACK PeerServer::OnConnection(ConnID connId)
{
	PeerServerConnector* pConnector = (PeerServerConnector*)_aligned_malloc(sizeof(PeerServerConnector), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnector)
	{
		if (PeerServer::Instance()->AddConnector(pConnector))
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

void CALLBACK PeerServer::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	PeerServerConnector* pConnector = (PeerServerConnector*)pConnection->client_;
	if (pConnector)
	{
		PeerServer::Instance()->DeleteConnector(connId);
		_aligned_free(pConnector);
	}

	Connection::Close(pConnection);
}

void CALLBACK PeerServer::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connId;
	PeerServerConnector* pConnector = (PeerServerConnector*)pConnection->GetClient();
	pConnector->OnPeerData(iLen, pBuf);
}
