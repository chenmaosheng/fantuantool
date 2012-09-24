#include "peer_server_connector.h"
#include "connection.h"
#include "peer_stream.h"

void PeerServerConnector::OnPeerData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	int32 iRet = 0;
	Connection* pConnection = (Connection*)m_ConnId;
	if (!pConnection->IsConnected())
	{
		return;
	}

	do 
	{
		if (m_iRecvBufLen + iLen <= sizeof(m_RecvBuf))
		{
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iLen);
			m_iRecvBufLen += iLen;
			pBuf += iLen;
			iLen = 0;
		}
		else
		{
			iCopyLen = m_iRecvBufLen + iLen - sizeof(m_RecvBuf);
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iCopyLen);
			pBuf += iCopyLen;
			iLen -= iCopyLen;
			m_iRecvBufLen += iCopyLen;
		}

		while (m_iRecvBufLen > PEER_PACKET_HEAD)
		{
			PeerPacket* pPeerPacket = (PeerPacket*)m_RecvBuf;
			uint16 iFullLength = pPeerPacket->m_iLen + PEER_PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)
			{
				iRet = Dispatch(pPeerPacket);
				if (iRet != 0)
				{
					return;
				}

				if (m_iRecvBufLen > iFullLength)
				{
					memmove(m_RecvBuf, m_RecvBuf + iFullLength, m_iRecvBufLen - iFullLength);
				}
				m_iRecvBufLen -= iFullLength;
			}
			else
			{
				break;
			}
		}
	} while (iLen);
}

bool PeerServerConnector::Dispatch(PeerPacket* pPeerPacket)
{
	return Dispatch(pPeerPacket->m_iFilterId, pPeerPacket->m_iFuncId, pPeerPacket->m_iLen, pPeerPacket->m_Buf);
}

int32 PeerOutputStream::Send(PEER_SERVER_CONNECTOR pServerConnector)
{
	((Connection*)((PeerServerConnector*)pServerConnector)->m_ConnId)->AsyncSend(PEER_PACKET_HEAD + m_pPacket->m_iLen, (char*)m_pPacket);
	m_pPacket = NULL;
	return 0;
}
