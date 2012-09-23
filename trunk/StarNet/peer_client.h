#ifndef _H_PEER_CLIENT
#define _H_PEER_CLIENT

#include <vector>

class Worker;
class ContextPool;
class PeerClient
{
public:
	PeerClient();
	~PeerClient();

	PeerConnector* GetConnector(uint32 iIP, uint16 iPort);

private:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	std::vector<PeerConnector*> m_vPeerConnectors;
};

#endif
