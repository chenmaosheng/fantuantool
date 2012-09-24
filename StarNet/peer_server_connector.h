#ifndef _H_PEER_SERVER_CONNECTOR
#define _H_PEER_SERVER_CONNECTOR

#include "peer_packet.h"

class PeerServerConnector
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

#endif
