#include "session.h"
#include "server_base.h"
#include "connection.h"
#include "basic_packet.h"
#include "context_pool.h"
#include "single_buffer.h"
#include "command.h"
#include "data_stream.h"
#include <malloc.h>

ServerBase* Session::m_pServer = NULL;

void Session::Initialize(ServerBase* pServer)
{
	m_pServer = pServer;
}

Session::Session()
{
	m_iSessionId = 0;
	m_pConnection = NULL;
	m_iRecvBufLen = 0;

	//m_pDataBuf = new SingleBuffer(MAX_INPUT_BUFFER);
}

Session::~Session()
{
	//SAFE_DELETE(m_pDataBuf);
	m_pConnection = NULL;
}

void Session::Clear()
{
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
}

int32 Session::OnConnection(ConnID connId)
{
	int32 iRet = 0;
	int32 i = sizeof(SessionId);

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	// coordinate conn and session
	m_pConnection = (Connection*)connId;

	m_pConnection->SetClient(this);
	m_pConnection->SetRefMax(256);

	return 0;
}

void Session::OnDisconnect()
{
	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
		m_pConnection = NULL;
	}

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	Clear();
}

//void Session::OnData(uint16 iLen, char *pBuf)
//{
//	uint16 iCopyLen = 0;
//	uint16 iPacketLen = 0;
//	Packet* pPacket = NULL;
//
//	do
//	{
//		// the incoming length is no more than the last of buffer
//		if (!m_pDataBuf->Push(pBuf, iLen))
//		{
//			iCopyLen = m_pDataBuf->GetLastSize();
//			m_pDataBuf->Push(pBuf, iCopyLen);
//			pBuf += iCopyLen;
//		}
//
//		while (m_pDataBuf->GetCurrSize() > sizeof(uint16))
//		{
//			m_pDataBuf->Pop(&iPacketLen, sizeof(uint16));
//			if (m_pDataBuf->GetCurrSize() >= iPacketLen)
//			{
//				
//			}
//		}
//
//	}while(iLen);
//}

void Session::OnData(uint16 iLen, char* pBuf)
{
	uint16 iCopyLen = 0;

	do
	{
		// the incoming length is no more than the last of buffer
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

		while (m_iRecvBufLen > SERVER_PACKET_HEAD)
		{
			ServerPacket* pServerPacket = (ServerPacket*)m_RecvBuf;
			uint16 iFullLength = pServerPacket->m_iLen+SERVER_PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)
			{
				OutputStream stream(pServerPacket->m_iLen, pServerPacket->m_Buf);
				uint16 iLength = 0;
				stream.Serialize(iLength);
				char* nickname = (char*)alloca(iLength + 1);
				stream.Serialize(iLength, nickname);
				nickname[iLength] = '\0';
				
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
	}while (iLen);
}

//void Session::OnData(uint16 iLen, char *pBuf)
//{
//	do
//	{
//		if (m_iRecvBufLen)
//		{
//			uint16 iCopyLen = 0;
//			// if the last buffer is less than head
//			if (m_iRecvBufLen < CS_PACKET_HEAD)
//			{
//				iCopyLen = CS_PACKET_HEAD - m_iRecvBufLen;
//				// if received buffer is less than need-copy length
//				if (iLen < iCopyLen)
//				{
//					iCopyLen = iLen;
//				}
//				// copy to the next buffer position
//				memcpy(m_RecvBuf + m_iRecvBufLen, Buf, iCopyLen);
//			}
//			else
//			{
//				CS_Packet* pPacket = (CS_Packet*)m_RecvBuf;
//				// the last buf just cost the head and the new buf is larger than packet itself
//				if (m_iRecvBufLen == CS_PACKET_HEAD && iLen >= pPacket->m_iLen)
//				{
//					// todo: do the packet
//					m_iRecvBufLen = 0;
//					iLen -= pPacket->m_iLen;
//					pBuf += pPacket->m_iLen;
//				}
//				else
//				{
//					// need-copy length equals to full packet size minus received length
//					iCopyLen = CS_PACKET_HEAD + pPacket->m_iLen - m_iRecvBufLen;
//					if (iLen < iCopyLen)
//					{
//						iCopyLen = iLen;
//					}
//					memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iCopyLen);
//					m_iRecvBufLen += iCopyLen;
//					iLen -= iCopyLen;
//					pBuf += iCopyLen;
//					if (m_iRecvBufLen == CS_PACKET_HEAD + pPacket->m_iLen)
//					{
//						// todo: do the packet
//						m_iRecvBufLen = 0;
//					}
//				}
//			}
//		}
//		else
//		{
//			// if received buffer is less than head
//			if (iLen < CS_PACKET_HEAD)
//			{
//				memcpy(m_RecvBuf, pBuf, iLen);
//				m_iRecvBufLen += iLen;
//				return;
//			}
//
//			CS_Packet* pPacket = (CS_Packet*)pBuf;
//			if (pPacket->m_iLen > MAX_INPUT_BUFFER)
//			{
//				Disconnect();
//				return;
//			}
//
//			uint16 iRealLen = CS_PACKET_HEAD + pPacket->m_iLen;
//			// if received buffer is no less than length of packet
//			if (iLen >= iRealLen)
//			{
//				// todo: do this packet
//				iLen -= iRealLen;
//				pBuf += iRealLen;
//			}
//			else
//			{
//				memcpy(m_RecvBuf, pBuf, iLen);
//				m_iRecvBufLen += iLen;
//				return;
//			}
//		}
//	}while(iLen != 0)
//}

void Session::Disconnect()
{
	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
	}
	else
	{
		OnDisconnect();
	}
}

int32 Session::SendData(uint16 filterId, uint16 len, const char *data)
{
	char* buf = m_pServer->GetContextPool()->PopOutputBuffer();
	if (!buf)
	{
		return -1;
	}

	/*CS_Packet* pPacket = (CS_Packet*)buf;
	pPacket->m_iLen = len;
	pPacket->m_iFilterId = filterId;
	memcpy(pPacket->m_Buf, data, len);*/

	memcpy(buf, data, len);
	m_pConnection->AsyncSend(len, buf);

	return 0;
}

int32 Session::HandleData(uint16 iLen, char *pBuf)
{
	return 0;
}

void Session::SaveSendData(uint16 iFilterId, uint16 iLen, char *pBuf)
{
}