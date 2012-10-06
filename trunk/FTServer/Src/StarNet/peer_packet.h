/*****************************************************************************************
	filename:	peer_packet.h
	created:	09/27/2012
	author:		chen
	purpose:	serialize into/from data, only for peer

*****************************************************************************************/

#ifndef _H_PEER_PACKET
#define _H_PEER_PACKET

#include "starnet_common.h"

struct PeerPacket
{
	uint16 m_iFilterId;
	uint16 m_iFuncId;
	uint32 m_iLen;	// buffer length, not include filterId, funcId and length itself
	char m_Buf[MAX_PEER_BUFFER];
};

#define PEER_PACKET_HEAD sizeof(uint16)*2 + sizeof(uint32)

#endif
