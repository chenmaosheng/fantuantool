#ifndef _H_PEER_CLIENT
#define _H_PEER_CLIENT

#include "peer_packet.h"
#include <vector>
#include "common.h"
#include "singleton.h"

class PeerClient
{
public:
	void OnPeerData(uint32 iLen, char* pBuf);
	bool Dispatch(PeerPacket*);
	bool Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf);

public:
	ConnID m_ConnId;
	uint32 m_iRecvBufLen;

private:
	char m_RecvBuf[MAX_PEER_BUFFER];
	PeerPacket m_Packet;
};

class Worker;
class ContextPool;
class Acceptor;
class PeerClient;
class PeerClientSet : public Singleton<PeerClientSet>
{
public:
	bool Init(uint32 iIP, uint16 iPort);
	void Destroy();
	bool AddConnector(PeerClient*);
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
	std::vector<PeerClient*> m_vPeerClients;
};

#endif
