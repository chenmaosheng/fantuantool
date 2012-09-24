#ifndef _H_PEER_CLIENT_CONNECTOR
#define _H_PEER_CLIENT_CONNECTOR

#include "peer_packet.h"

class PeerClientConnector
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

#endif
