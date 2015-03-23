#include "easy_basesession.h"
#include "easy_baseserver.h"
#include "easy_connection.h"
#include "easy_packet.h"
#include "easy_basecommand.h"
#include "easy_baseloop.h"
#include "easy_context.h"
#include "easy_packethandler.h"

uint32 EasyBaseSession::m_iSessionIdStart = 0;

EasyBaseSession::EasyBaseSession()
{
	m_iSessionId = 0;
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
}

EasyBaseSession::~EasyBaseSession()
{
	m_pConnection = NULL;
}

int32 EasyBaseSession::OnConnection(ConnID connId)
{
	m_iSessionId = ++m_iSessionIdStart;
	m_pConnection = (EasyConnection*)connId;
	// coordinate conn and EasyBaseSession
	m_pConnection->client_ = this;
	return 0;
}

void EasyBaseSession::OnDisconnect()
{
	if (m_pConnection)
	{
		CloseConnection(m_pConnection);
		m_pConnection = NULL;
	}
}

void EasyBaseSession::OnData(uint16 iLen, char* pBuf)
{
	uint16 iCopyLen = 0;
	int32 iRet = 0;

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
		}	// step1: received a raw buffer

		while (m_iRecvBufLen >= PACKET_HEAD)	// step2: check if buffer is larger than header
		{
			EasyPacket* pPacket = (EasyPacket*)m_RecvBuf;
			uint16 iFullLength = pPacket->m_iLen+PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)	// step3: cut specific size from received buffer
			{
				iRet = _HandlePacket(pPacket);
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
	}while (iLen);
}

void EasyBaseSession::Disconnect()
{
	if (m_pConnection)
	{
		AsyncDisconnect(m_pConnection);
	}
	else
	{
		OnDisconnect();
	}
}

int32 EasyBaseSession::SendData(uint16 iTypeId, uint16 len, const char *data)
{
	EasyContext* pContext = CreateContext(OPERATION_SEND);
	EasyPacket* pPacket = (EasyPacket*)pContext->buffer_;
	pPacket->m_iLen = len;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, data, len);
	AsyncRawSend(m_pConnection, pPacket->m_iLen + PACKET_HEAD, pContext->buffer_);

	return 0;
}

int32 EasyBaseSession::_HandlePacket(EasyPacket* pPacket)
{
	PacketHandler::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}
