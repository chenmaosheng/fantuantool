#ifndef _H_PEER_SERVER
#define _H_PEER_SERVER

#include "peer_packet.h"
#include "singleton.h"
#include <vector>

class PeerServer
{
public:
	enum
	{
		NOT_CONNECT,
		CONNECTING,
		CONNECTED,
		CONNECT_FAILED,
		DISCONNECTED,
		DESTORYING,
	};

	void Init(uint32 iIP, uint16 iPort);
	void Destroy();
	PSOCKADDR_IN GetSockAddr();

	ConnID GetConnId();
	bool Connect();
	void OnPeerData(uint32 iLen, char* pBuf);
	bool Dispatch(PeerPacket*);
	bool Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf);

	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	ConnID m_ConnId;
	SOCKADDR_IN m_SockAddr;
	uint32 m_iRecvBufLen;
	char m_RecvBuf[MAX_PEER_BUFFER];
	int32 m_iState;
};

class Worker;
class ContextPool;
class PeerServer;
class PeerServerSet : public Singleton<PeerServerSet>
{
public:
	PeerServerSet();
	~PeerServerSet();

	PeerServer* GetPeerServer(uint32 iIP, uint16 iPort);

public:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;

private:
	std::vector<PeerServer*> m_vPeerServers;
};

#endif
