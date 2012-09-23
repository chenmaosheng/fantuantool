#ifndef _H_PEER_CONNECTOR
#define _H_PEER_CONNECTOR

#include "peer_packet.h"

class PeerConnector
{
public:
	void Init(uint32 iIP, uint16 iPort);
	void Destroy();

	ConnId GetConnId();
	bool Connect();
	void OnData(uint32 iLen, char* pBuf);
	bool Dispatch();
	bool Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf);

	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint16 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	ConnId m_ConnId;
	SOCKADDR_IN m_SockAddr;
	uint32 m_iRecvBufLen;
	PeerPacket m_Packet;
};

#endif
