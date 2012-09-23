#ifndef _H_PEER_ACCEPTOR
#define _H_PEER_ACCEPTOR

#include "peer_packet.h"

class PeerAcceptor
{
public:
	void OnData(uint32 iLen, char* pBuf);
	bool Dispatch();
	bool Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf);

private:
	ConnId m_ConnId;
	uint32 m_iRecvBufLen;
	PeerPacket m_Packet;
};

#endif
