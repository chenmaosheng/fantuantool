/*****************************************************************************************
	filename:	peer_server.h
	created:	09/27/2012
	author:		chen
	purpose:	serve as server when in peer mode

*****************************************************************************************/

#ifndef _H_PEER_SERVER
#define _H_PEER_SERVER

#include "peer_packet.h"
#include "singleton.h"
#include <vector>

// record the server info from peer
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

	// initialize the state
	void Init(uint32 iIP, uint16 iPort);
	// disconnect from peer server
	void Destroy();
	PSOCKADDR_IN GetSockAddr();

	ConnID GetConnId();
	// connect to peer server
	bool Connect();
	// receive data from peer server
	void OnPeerData(uint32 iLen, char* pBuf, uint32=0);
	// dispatch received data
	bool Dispatch(PeerPacket*);
	bool Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf);

	// connection handler
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32& iLen, char* pBuf, uint32&);
	static void CALLBACK OnConnectFailed(void*);

private:
	ConnID m_ConnId;		// connection id
	SOCKADDR_IN m_SockAddr;
	uint32 m_iRecvBufLen;	// already received buffer length
	char m_RecvBuf[MAX_PEER_BUFFER];	// the whole received buffer
	int32 m_iState;	// current state about connect
};

class Worker;
class ContextPool;
class PeerServerSet : public Singleton<PeerServerSet>
{
public:
	PeerServerSet();
	~PeerServerSet();

	// get peer server by ip and port
	PeerServer* GetPeerServer(uint32 iIP, uint16 iPort);

public:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;

private:
	std::vector<PeerServer*> m_vPeerServers;
};

#endif
