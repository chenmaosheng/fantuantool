#ifndef _H_PEER_SERVER
#define _H_PEER_SERVER

#include <vector>
#include "common.h"
#include "singleton.h"

class Worker;
class ContextPool;
class Acceptor;
class PeerServerConnector;
class PeerServer : public Singleton<PeerServer>
{
public:
	bool Init(uint32 iIP, uint16 iPort);
	void Destroy();
	bool AddConnector(PeerServerConnector*);
	void DeleteConnector(ConnID);

	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);

public:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	Acceptor* m_pAcceptor;
	uint32 m_iIP;
	uint16 m_iPort;

private:
	std::vector<PeerServerConnector*> m_vPeerConnectors;
};

#endif
