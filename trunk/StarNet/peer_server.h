#ifndef _H_PEER_SERVER
#define _H_PEER_SERVER

class PeerAcceptor;
class PeerServer
{
public:
	PeerServer();
	~PeerServer();

	int32 Init(uint32 iIP, uint16 iPort);
	void Destroy();
	void AddAcceptor(PeerAcceptor*);
	void DeleteAcceptor(ConnId);

private:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	std::vector<PeerAcceptor*> m_vPeerAcceptors;
};

#endif
