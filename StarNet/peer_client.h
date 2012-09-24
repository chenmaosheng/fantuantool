#ifndef _H_PEER_CLIENT
#define _H_PEER_CLIENT

#include <vector>
#include "common.h"
#include "singleton.h"

class Worker;
class ContextPool;
class PeerClientConnector;
class PeerClient : public Singleton<PeerClient>
{
public:
	PeerClient();
	~PeerClient();

	PeerClientConnector* GetConnector(uint32 iIP, uint16 iPort);

public:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;

private:
	std::vector<PeerClientConnector*> m_vPeerClientConnectors;
};

#endif
