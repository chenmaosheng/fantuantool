#include "peer_client.h"
#include "starnet.h"
#include "worker.h"
#include "context_pool.h"
#include "peer_client_connector.h"

PeerClient::PeerClient()
{
	StarNet::Init();

	m_pWorker = Worker::CreateWorker(1);
	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);
}

PeerClient::~PeerClient()
{
	for (std::vector<PeerClientConnector*>::iterator it = m_vPeerClientConnectors.begin(); it != m_vPeerClientConnectors.end(); ++it)
	{
		(*it)->Destroy();
		_aligned_free(*it);
	}
	m_vPeerClientConnectors.clear();

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
}

PeerClientConnector* PeerClient::GetConnector(uint32 iIP, uint16 iPort)
{
	for (std::vector<PeerClientConnector*>::iterator it = m_vPeerClientConnectors.begin(); it != m_vPeerClientConnectors.end(); ++it)
	{
		if ((*it)->GetSockAddr()->sin_port == htons(iPort) &&
			(*it)->GetSockAddr()->sin_addr.s_addr == iIP)
		{
			return *it;
		}
	}

	PeerClientConnector* pConnector = (PeerClientConnector*)_aligned_malloc(sizeof(PeerClientConnector), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnector)
	{
		pConnector->Init(iIP, iPort);
		return pConnector;
	}

	return NULL;
}